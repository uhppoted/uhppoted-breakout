package sqlite3

import (
	"dbd/entities"
)

const sqlGetDoor = `SELECT Mode,Delay FROM Doors WHERE Controller=? AND Door=?;`

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
				Mode:  record.mode,
				Delay: record.delay,
			}, nil
		}

		return nil, entities.ErrRecordNotFound
	}
}
