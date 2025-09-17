package sqlite3

import (
	"dbd/entities"
)

const sqlGetDoor = `SELECT Mode,Delay FROM Doors WHERE Controller=? AND Door=?;`

const sqlPutDoor = `INSERT INTO Doors (Controller, Door, Mode, Delay)
        VALUES (?, ?, ?, ?)
        ON CONFLICT(Controller, Door) DO UPDATE SET
        Mode  = excluded.Mode,
        Delay = excluded.Delay;`

const sqlGetInterlock = `SELECT Interlock FROM Controller WHERE Controller=?;`

const sqlPutInterlock = `INSERT INTO Controller (Controller, Interlock)
        VALUES (?, ?)
        ON CONFLICT(Controller) DO UPDATE SET
        Interlock = excluded.Interlock;`

const sqlGetAntiPassback = `SELECT AntiPassback FROM Controller WHERE Controller=?;`

func (db impl) GetDoor(controller uint32, door uint8) (*entities.Door, error) {
	if rs, err := db.query(sqlGetDoor, controller, door); err != nil {
		return nil, err
	} else {
		defer rs.Close()

		for rs.Next() {
			record := struct {
				mode  uint8
				delay uint8
			}{}

			pointers := []any{
				&record.mode,
				&record.delay,
			}

			if err := rs.Scan(pointers...); err != nil {
				return nil, err
			}

			return &entities.Door{
				Controller: controller,
				Door:       door,
				Mode:       record.mode,
				Delay:      record.delay,
			}, nil
		}

		return nil, entities.ErrRecordNotFound
	}
}

func (db impl) SetDoor(controller uint32, door uint8, mode uint8, delay uint8) (*entities.Door, error) {
	if _, err := db.upsert(sqlPutDoor, controller, door, mode, delay); err != nil {
		return nil, err
	} else {
		return &entities.Door{
			Controller: controller,
			Door:       door,
			Mode:       mode,
			Delay:      delay,
		}, nil
	}
}

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
