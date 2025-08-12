package sqlite

import (
	"context"
	"errors"
	"fmt"
	"os"
	"time"

	_ "github.com/mattn/go-sqlite3"

	"dbd/entities"
)

const sqlGetCard = `SELECT Card, CAST(StartDate AS VARCHAR), CAST(EndDate AS VARCHAR),Door1, Door2, Door3, Door4, PIN FROM Cards WHERE Controller=? AND Card=?;`

func (db impl) GetCard(controller uint32, card uint32) (entities.Card, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	zero := entities.Card{}
	query := sqlGetCard

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
	} else if rs, err := prepared.QueryContext(ctx, controller, card); err != nil {
		return zero, err
	} else if rs == nil {
		return zero, fmt.Errorf("invalid resultset (%v)", rs)
	} else {
		defer rs.Close()

		for rs.Next() {
			record := struct {
				card      uint32
				startDate string
				endDate   string
				door1     uint8
				door2     uint8
				door3     uint8
				door4     uint8
				PIN       uint32
			}{}

			pointers := []any{
				&record.card,
				&record.startDate,
				&record.endDate,
				&record.door1,
				&record.door2,
				&record.door3,
				&record.door4,
				&record.PIN,
			}

			if err := rs.Scan(pointers...); err != nil {
				return zero, err
			} else {
				startDate := time.Time{}
				if v, err := time.ParseInLocation("2006-01-02", record.startDate, time.Local); err != nil {
					warnf("%v", err)
				} else {
					startDate = v
				}

				endDate := time.Time{}
				if v, err := time.ParseInLocation("2006-01-02", record.endDate, time.Local); err != nil {
					warnf("%v", err)
				} else {
					endDate = v
				}

				println(">>> debug/woot")

				return entities.Card{
					Card:      record.card,
					StartDate: startDate,
					EndDate:   endDate,
					Permissions: map[uint8]uint8{
						1: record.door1,
						2: record.door2,
						3: record.door3,
						4: record.door4,
					},
					PIN: record.PIN,
				}, nil
			}
		}

		return zero, entities.ErrRecordNotFound
	}
}
