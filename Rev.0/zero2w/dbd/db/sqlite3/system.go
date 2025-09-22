package sqlite3

import (
	"time"

	"dbd/entities"
)

const sqlGetInterlock = `SELECT Interlock FROM Controller WHERE Controller=?;`

const sqlPutInterlock = `INSERT INTO Controller (Controller, Interlock)
        VALUES (?, ?)
        ON CONFLICT(Controller) DO UPDATE SET
        Interlock = excluded.Interlock;`

const sqlGetAntiPassback = `SELECT AntiPassback FROM Controller WHERE Controller=?;`

const sqlPutAntiPassback = `INSERT INTO Controller (Controller, AntiPassback)
        VALUES (?, ?)
        ON CONFLICT(Controller) DO UPDATE SET
        AntiPassback = excluded.AntiPassback;`

const sqlGetSwipe = `SELECT Door,Timestamp FROM Swipes WHERE Controller=? AND Card=?;`

const sqlPutSwipe = `INSERT INTO Swipes (Controller, Card, Door, Timestamp)
        VALUES (?, ?, ?, CURRENT_TIMESTAMP)
        ON CONFLICT(Controller,Card) DO UPDATE SET
        Door = excluded.Door,
        Timestamp = CURRENT_TIMESTAMP;`

func (db impl) GetInterlock(controller uint32) (*entities.Interlock, error) {
	if rs, err := db.query(sqlGetInterlock, controller); err != nil {
		return nil, err
	} else {
		defer rs.Close()

		for rs.Next() {
			record := struct {
				interlock uint8
			}{}

			pointers := []any{
				&record.interlock,
			}

			if err := rs.Scan(pointers...); err != nil {
				return nil, err
			}

			return &entities.Interlock{
				Controller: controller,
				Interlock:  record.interlock,
			}, nil
		}

		return nil, entities.ErrRecordNotFound
	}
}

func (db impl) SetInterlock(controller uint32, interlock uint8) (*entities.Interlock, error) {
	if _, err := db.upsert(sqlPutInterlock, controller, interlock); err != nil {
		return nil, err
	} else {
		return &entities.Interlock{
			Controller: controller,
			Interlock:  interlock,
		}, nil
	}
}

func (db impl) GetAntiPassback(controller uint32) (*entities.AntiPassback, error) {
	if rs, err := db.query(sqlGetAntiPassback, controller); err != nil {
		return nil, err
	} else {
		defer rs.Close()

		for rs.Next() {
			record := struct {
				antipassback uint8
			}{}

			pointers := []any{
				&record.antipassback,
			}

			if err := rs.Scan(pointers...); err != nil {
				return nil, err
			}

			return &entities.AntiPassback{
				Controller:   controller,
				AntiPassback: record.antipassback,
			}, nil
		}

		return nil, entities.ErrRecordNotFound
	}
}

func (db impl) SetAntiPassback(controller uint32, antipassback uint8) (*entities.AntiPassback, error) {
	if _, err := db.upsert(sqlPutAntiPassback, controller, antipassback); err != nil {
		return nil, err
	} else {
		return &entities.AntiPassback{
			Controller:   controller,
			AntiPassback: antipassback,
		}, nil
	}
}

func (db impl) GetSwipe(controller uint32, card uint32) (*entities.Swipe, error) {
	if rs, err := db.query(sqlGetSwipe, controller, card); err != nil {
		return nil, err
	} else {
		defer rs.Close()

		for rs.Next() {
			record := struct {
				door      uint8
				timestamp string
			}{}

			pointers := []any{
				&record.door,
				&record.timestamp,
			}

			if err := rs.Scan(pointers...); err != nil {
				return nil, err
			}

			timestamp := time.Time{}
			if v, err := time.ParseInLocation(time.DateTime, record.timestamp[0:19], time.Local); err == nil {
				timestamp = v
			} else if v, err := time.ParseInLocation("2006-01-02T15:04:05", record.timestamp[0:19], time.Local); err == nil {
				timestamp = v
			} else {
				warnf("error parsing timestamp %v", record.timestamp)
			}

			return &entities.Swipe{
				Controller: controller,
				Card:       card,
				Door:       record.door,
				Timestamp:  timestamp,
			}, nil
		}

		return nil, nil
	}
}

func (db impl) PutSwipe(controller uint32, card uint32, door uint8) (uint32, error) {
	values := []any{
		controller,
		card,
		door,
	}

	if id, err := db.upsert(sqlPutSwipe, values...); err != nil {
		return 0, err
	} else {
		return uint32(id), nil
	}
}
