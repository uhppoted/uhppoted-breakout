package sqlite3

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"os"
	"time"

	_ "github.com/mattn/go-sqlite3"

	"dbd/log"
)

const MaxLifetime = 5 * time.Minute
const MaxIdle = 2
const MaxOpen = 5
const LogTag = "sqlite3"

const tableController = "Controller"
const tableCards = "Cards"
const tableEvents = "Events"

type impl struct {
	dsn         string
	maxLifetime time.Duration
	maxOpen     int
	maxIdle     int
}

func NewDB(dsn string) impl {
	return impl{
		dsn:         dsn,
		maxLifetime: MaxLifetime,
		maxIdle:     MaxIdle,
		maxOpen:     MaxOpen,
	}
}

func (db impl) open() (*sql.DB, error) {
	dsn := fmt.Sprintf("%v?_journal_mode=WAL", db.dsn)

	if dbc, err := sql.Open("sqlite3", dsn); err != nil {
		return nil, err
	} else {
		dbc.SetConnMaxLifetime(db.maxLifetime)
		dbc.SetMaxOpenConns(db.maxOpen)
		dbc.SetMaxIdleConns(db.maxIdle)

		return dbc, nil
	}
}

func (db impl) insert(sql string, values ...any) (int64, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	if _, err := os.Stat(db.dsn); errors.Is(err, os.ErrNotExist) {
		return 0, fmt.Errorf("sqlite3 database %v does not exist", db.dsn)
	} else if err != nil {
		return 0, err
	}

	if dbc, err := db.open(); err != nil {
		return 0, err
	} else if dbc == nil {
		return 0, fmt.Errorf("invalid sqlite3 DB (%v)", dbc)
	} else if tx, err := dbc.BeginTx(ctx, nil); err != nil {
		return 0, err
	} else {
		defer tx.Rollback()

		if result, err := tx.ExecContext(ctx, sql, values...); err != nil {
			return 0, err
		} else if err := tx.Commit(); err != nil {
			return 0, err
		} else if id, err := result.LastInsertId(); err != nil {
			return 0, err
		} else {
			return id, nil
		}
	}
}

func (db impl) update(sql string, values ...any) error {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	if _, err := os.Stat(db.dsn); errors.Is(err, os.ErrNotExist) {
		return fmt.Errorf("sqlite3 database %v does not exist", db.dsn)
	} else if err != nil {
		return err
	}

	if dbc, err := db.open(); err != nil {
		return err
	} else if dbc == nil {
		return fmt.Errorf("invalid sqlite3 DB (%v)", dbc)
	} else if tx, err := dbc.BeginTx(ctx, nil); err != nil {
		return err
	} else {
		defer tx.Rollback()

		if _, err := tx.ExecContext(ctx, sql, values...); err != nil {
			return err
		} else if err := tx.Commit(); err != nil {
			return err
		} else {
			return nil
		}
	}
}

func debugf(format string, args ...any) {
	log.Debugf(LogTag, format, args...)
}

func infof(format string, args ...any) {
	log.Infof(LogTag, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LogTag, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LogTag, format, args...)
}
