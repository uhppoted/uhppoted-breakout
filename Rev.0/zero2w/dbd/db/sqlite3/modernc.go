package sqlite3

import (
	"context"
	"database/sql"
	"errors"
	"fmt"
	"os"
	"time"

	_ "modernc.org/sqlite"
)

type modernc struct {
	dsn         string
	maxLifetime time.Duration
	maxOpen     int
	maxIdle     int

	dbc *sql.DB
}

func (db *modernc) open() (*sql.DB, error) {
	if db.dbc == nil {
		dsn := fmt.Sprintf("%v?_pragma=journal_mode(WAL)&_pragma=busy_timeout(5000)", db.dsn)

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

func (db modernc) query(sql string, args ...any) (*sql.Rows, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	if dbc, err := db.open(); err != nil {
		return nil, err
	} else if prepared, err := dbc.Prepare(sql); err != nil {
		return nil, err
	} else if rs, err := prepared.QueryContext(ctx, args...); err != nil {
		return nil, err
	} else if rs == nil {
		return nil, fmt.Errorf("invalid resultset (%v)", rs)
	} else {
		return rs, nil
	}
}

func (db modernc) insert(sql string, values ...any) (int64, error) {
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

func (db modernc) update(sql string, values ...any) error {
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
