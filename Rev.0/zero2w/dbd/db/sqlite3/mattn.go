package sqlite3

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"os"
	"time"

	_ "github.com/mattn/go-sqlite3"
)

type mattn struct {
	dsn         string
	maxLifetime time.Duration
	maxOpen     int
	maxIdle     int

	dbc *sql.DB
}

func (db *mattn) open() (*sql.DB, error) {
	if db.dbc == nil {
		dsn := fmt.Sprintf("%v?_journal_mode=WAL&_busy_timeout=5000", db.dsn)

		if _, err := os.Stat(db.dsn); errors.Is(err, os.ErrNotExist) {
			return nil, fmt.Errorf("sqlite3 database %v does not exist", db.dsn)
		} else if err != nil {
			return nil, err
		}

		if dbc, err := sql.Open("sqlite", dsn); err != nil {
			return nil, err
		} else if dbc == nil {
			return nil, fmt.Errorf("invalid sqlite3 DB connection (%v)", dbc)
		} else {
			dbc.SetConnMaxLifetime(db.maxLifetime)
			dbc.SetMaxOpenConns(db.maxOpen)
			dbc.SetMaxIdleConns(db.maxIdle)

			db.dbc = dbc
		}
	}

	return db.dbc, nil
}

func (db mattn) insert(sql string, values ...any) (int64, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	if dbc, err := db.open(); err != nil {
		return 0, err
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

func (db mattn) update(sql string, values ...any) error {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	if dbc, err := db.open(); err != nil {
		return err
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
