package sqlite3

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"os"
	"strings"
	"time"

	_ "github.com/mattn/go-sqlite3"

	"emulator/entities"
	"emulator/log"
)

const MaxLifetime = 5 * time.Minute
const MaxIdle = 2
const MaxOpen = 5
const LOGTAG = "sqlite3"

// type record map[string]any

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

func (db impl) PutEvent(event entities.Event) (uint32, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	record := map[string]any{
		"Timestamp":  fmt.Sprintf("%v", event.Timestamp),
		"Type":       event.Type,
		"Granted":    event.Granted,
		"Door":       event.Door,
		"Direction":  event.Direction,
		"CardNumber": event.Card,
		"Reason":     event.Reason,
	}

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
	} else if id, err := db.insert(dbc, tx, "events", record); err != nil {
		return 0, err
	} else if err := tx.Commit(); err != nil {
		return 0, err
	} else {
		event.Index = id
		infof("stored event %v", event)

		return id, nil
	}
}

func (db impl) open() (*sql.DB, error) {
	if dbc, err := sql.Open("sqlite3", db.dsn); err != nil {
		return nil, err
	} else {
		dbc.SetConnMaxLifetime(db.maxLifetime)
		dbc.SetMaxOpenConns(db.maxOpen)
		dbc.SetMaxIdleConns(db.maxIdle)

		return dbc, nil
	}
}

func (db impl) insert(dbc *sql.DB, tx *sql.Tx, table string, record map[string]any) (uint32, error) {
	columns := []string{}
	placeholders := []string{}
	values := []any{}

	for k, v := range record {
		columns = append(columns, k)
		values = append(values, v)
		placeholders = append(placeholders, "?")
	}

	sql := fmt.Sprintf("INSERT INTO %v (%v) VALUES (%v);",
		table,
		strings.Join(columns, ","),
		strings.Join(placeholders, ","))

	// ... execute
	if prepared, err := dbc.Prepare(sql); err != nil {
		return 0, err
	} else {
		if result, err := tx.Stmt(prepared).Exec(values...); err != nil {
			return 0, err
		} else if id, err := result.LastInsertId(); err != nil {
			return 0, err
		} else {
			return uint32(id), nil
		}
	}

	return 0, nil
}

func debugf(format string, args ...any) {
	log.Debugf(LOGTAG, format, args...)
}

func infof(format string, args ...any) {
	log.Infof(LOGTAG, format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf(LOGTAG, format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf(LOGTAG, format, args...)
}
