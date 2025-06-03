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

	"eventd/entities"
	"eventd/log"
)

const MaxLifetime = 5 * time.Minute
const MaxIdle = 2
const MaxOpen = 5
const LogTag = "sqlite3"

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

func (db impl) GetEvent(index uint32) (entities.Event, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	query := fmt.Sprintf(`SELECT CAST(Timestamp AS VARCHAR),Type,Granted,Door,Direction,CardNumber,Reason FROM Events WHERE EventID=?;`)

	if _, err := os.Stat(db.dsn); errors.Is(err, os.ErrNotExist) {
		return entities.Event{}, fmt.Errorf("sqlite3 database %v does not exist", db.dsn)
	} else if err != nil {
		return entities.Event{}, err
	}

	if dbc, err := db.open(); err != nil {
		return entities.Event{}, err
	} else if dbc == nil {
		return entities.Event{}, fmt.Errorf("invalid sqlite3 DB (%v)", dbc)
	} else if prepared, err := dbc.Prepare(query); err != nil {
		return entities.Event{}, err
	} else if rs, err := prepared.QueryContext(ctx, index); err != nil {
		return entities.Event{}, err
	} else if rs == nil {
		return entities.Event{}, fmt.Errorf("invalid resultset (%v)", rs)
	} else {
		defer rs.Close()

		for rs.Next() {
			record := struct {
				timestamp string
				event     uint8
				granted   uint8
				door      uint8
				direction uint8
				card      uint32
				reason    uint8
			}{}

			pointers := []any{
				&record.timestamp,
				&record.event,
				&record.granted,
				&record.door,
				&record.direction,
				&record.card,
				&record.reason,
			}

			if err := rs.Scan(pointers...); err != nil {
				return entities.Event{}, err
			} else {
				timestamp := time.Time{}
				if v, err := time.ParseInLocation("2006-01-02 15:04:05", record.timestamp[0:19], time.Local); err != nil {
					warnf("%v", err)
				} else {
					timestamp = v
				}

				granted := false
				if record.granted == 1 {
					granted = true
				}

				return entities.Event{
					Index:     index,
					Timestamp: timestamp,
					Type:      entities.EventType(record.event),
					Granted:   granted,
					Door:      record.door,
					Direction: record.direction,
					Card:      record.card,
					Reason:    entities.EventReason(record.reason),
				}, nil
			}
		}

		return entities.Event{}, entities.ErrRecordNotFound
	}
}

func (db impl) GetEvents() (uint32, uint32, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	query := fmt.Sprintf(`SELECT MIN(EventID) AS First,MAX(EventID) AS Last FROM Events;`)

	if _, err := os.Stat(db.dsn); errors.Is(err, os.ErrNotExist) {
		return 0, 0, fmt.Errorf("sqlite3 database %v does not exist", db.dsn)
	} else if err != nil {
		return 0, 0, err
	}

	if dbc, err := db.open(); err != nil {
		return 0, 0, err
	} else if dbc == nil {
		return 0, 0, fmt.Errorf("invalid sqlite3 DB (%v)", dbc)
	} else if prepared, err := dbc.Prepare(query); err != nil {
		return 0, 0, err
	} else if rs, err := prepared.QueryContext(ctx); err != nil {
		return 0, 0, err
	} else if rs == nil {
		return 0, 0, fmt.Errorf("invalid resultset (%v)", rs)
	} else {
		defer rs.Close()

		for rs.Next() {
			var first uint32
			var last uint32

			if err := rs.Scan(&first, &last); err != nil {
				return 0, 0, err
			} else {
				return first, last, nil
			}
		}

		return 0, 0, entities.ErrRecordNotFound
	}
}

func (db impl) PutEvent(event entities.Event) (uint32, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	record := map[string]any{
		"Timestamp":  fmt.Sprintf("2006-01-02 15:04:05", event.Timestamp),
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

func (db impl) GetEventIndex(controller uint32) (uint32, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	query := fmt.Sprintf(`SELECT EventIndex FROM EventIndex WHERE controller=?;`)

	if _, err := os.Stat(db.dsn); errors.Is(err, os.ErrNotExist) {
		return 0, fmt.Errorf("sqlite3 database %v does not exist", db.dsn)
	} else if err != nil {
		return 0, err
	}

	if dbc, err := db.open(); err != nil {
		return 0, err
	} else if dbc == nil {
		return 0, fmt.Errorf("invalid sqlite3 DB (%v)", dbc)
	} else if prepared, err := dbc.Prepare(query); err != nil {
		return 0, err
	} else if rs, err := prepared.QueryContext(ctx, controller); err != nil {
		return 0, err
	} else if rs == nil {
		return 0, fmt.Errorf("invalid resultset (%v)", rs)
	} else {
		defer rs.Close()

		for rs.Next() {
			var index uint32

			if err := rs.Scan(&index); err != nil {
				return 0, err
			} else {
				return index, nil
			}
		}

		return 0, nil
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
