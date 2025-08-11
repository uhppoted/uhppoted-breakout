package sqlite

import (
	"context"
	"errors"
	"fmt"
	"os"
	"time"

	_ "modernc.org/sqlite"

	"eventd/entities"
)

const sqlGetEvent = `SELECT CAST(Timestamp AS VARCHAR),Type,Granted,Door,Direction,CardNumber,Reason FROM Events WHERE EventID=?;`

const sqlPutEvent = `INSERT INTO Events (Controller, EventID, Timestamp, Type, Granted, Door, Direction, CardNumber, Reason)
                           SELECT ?, COALESCE(MAX(EventID), 0) + 1, ?, ?, ?, ?, ?, ?, ?
                           FROM Events
                           WHERE Controller = ?;`

const sqlSetEventIndex = `INSERT INTO Controller (Controller, EventIndex) VALUES (?, ?)
                                 ON CONFLICT(Controller)
                                    DO UPDATE SET EventIndex = excluded.EventIndex;`

const sqlRecordSpecialEvents = `INSERT INTO Controller (Controller, RecordSpecialEvents) VALUES (?, ?) 
                                       ON CONFLICT(Controller)
                                          DO UPDATE SET RecordSpecialEvents = excluded.RecordSpecialEvents;`

func (db impl) GetEvent(index uint32) (entities.Event, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	zero := entities.Event{}
	query := sqlGetEvent

	if _, err := os.Stat(db.dsn); errors.Is(err, os.ErrNotExist) {
		return zero, fmt.Errorf("sqlite3 database %v does not exist", db.dsn)
	} else if err != nil {
		return zero, err
	}

	if dbc, err := db.open(); err != nil {
		return zero, err
	} else if dbc == nil {
		return zero, fmt.Errorf("invalid sqlite3 DB (%v)", dbc)
	} else if prepared, err := dbc.Prepare(query); err != nil {
		return zero, err
	} else if rs, err := prepared.QueryContext(ctx, index); err != nil {
		return zero, err
	} else if rs == nil {
		return zero, fmt.Errorf("invalid resultset (%v)", rs)
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
				return zero, err
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

		return zero, entities.ErrRecordNotFound
	}
}

func (db impl) GetEvents() (uint32, uint32, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	query := fmt.Sprintf(`SELECT MIN(EventID) AS First,MAX(EventID) AS Last FROM %[1]v;`, tableEvents)

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

func (db impl) PutEvent(controller uint32, event entities.Event) (uint32, error) {
	values := []any{
		controller,
		event.Timestamp.Format("2006-01-02 15:04:05"),
		event.Type,
		event.Granted,
		event.Door,
		event.Direction,
		event.Card,
		event.Reason,
		controller,
	}

	if id, err := db.insert(sqlPutEvent, values...); err != nil {
		return 0, err
	} else {
		return uint32(id), nil
	}
}

func (db impl) GetEventIndex(controller uint32) (uint32, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	query := fmt.Sprintf(`SELECT EventIndex FROM %[1]v WHERE controller=?;`, tableController)

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

func (db impl) SetEventIndex(controller uint32, index uint32) error {
	return db.update(sqlSetEventIndex, controller, index)
}

func (db impl) RecordSpecialEvents(controller uint32, enabled bool) error {
	return db.update(sqlRecordSpecialEvents, controller, enabled)
}
