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
