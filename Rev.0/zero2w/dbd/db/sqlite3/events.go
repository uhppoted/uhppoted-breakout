package sqlite3

import (
	"time"

	"dbd/entities"
)

const sqlGetEvent = `SELECT CAST(Timestamp AS VARCHAR),Type,Granted,Door,Direction,CardNumber,Reason FROM Events WHERE Controller=? AND EventID=?;`

const sqlGetEvents = `SELECT MIN(EventID) AS First,MAX(EventID) AS Last FROM Events WHERE Controller=?;`

const sqlPutEvent = `INSERT INTO Events (Controller, EventID, Timestamp, Type, Granted, Door, Direction, CardNumber, Reason)
                           SELECT ?, COALESCE(MAX(EventID), 0) + 1, ?, ?, ?, ?, ?, ?, ?
                           FROM Events
                           WHERE Controller = ?;`

const sqlGetEventIndex = `SELECT EventIndex FROM Controller WHERE Controller=?;`

const sqlSetEventIndex = `INSERT INTO Controller (Controller, EventIndex) VALUES (?, ?)
                                 ON CONFLICT(Controller)
                                    DO UPDATE SET EventIndex = excluded.EventIndex;`

const sqlRecordSpecialEvents = `INSERT INTO Controller (Controller, RecordSpecialEvents) VALUES (?, ?) 
                                       ON CONFLICT(Controller)
                                          DO UPDATE SET RecordSpecialEvents = excluded.RecordSpecialEvents;`

func (db impl) GetEvent(controller uint32, index uint32) (entities.Event, error) {
	if rs, err := db.query(sqlGetEvent, controller, index); err != nil {
		return entities.Event{}, err
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
				if v, err := time.ParseInLocation(time.DateTime, record.timestamp[0:19], time.Local); err != nil {
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

func (db impl) GetEvents(controller uint32) (uint32, uint32, error) {
	if rs, err := db.query(sqlGetEvents, controller); err != nil {
		return 0, 0, err
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
		event.Timestamp.Format(time.DateTime),
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
	if rs, err := db.query(sqlGetEventIndex, controller); err != nil {
		return 0, err
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
