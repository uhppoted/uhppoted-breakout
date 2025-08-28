package sqlite3

import (
	"context"
	"fmt"
	"time"

	"dbd/entities"
)

const sqlGetCards = `SELECT COUNT(Card) FROM Cards WHERE Controller=?;`

const sqlGetCard = `SELECT Card, CAST(StartDate AS VARCHAR), CAST(EndDate AS VARCHAR),Door1, Door2, Door3, Door4, PIN FROM Cards WHERE Controller=? AND Card=?;`

const sqlGetCardByIndex = `SELECT Card, CAST(StartDate AS VARCHAR), CAST(EndDate AS VARCHAR),Door1, Door2, Door3, Door4, PIN FROM Cards WHERE Controller=? AND id=?;`

const sqlPutCard = `INSERT INTO Cards (Controller, Card, StartDate, EndDate, Door1, Door2, Door3, Door4, PIN)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
        ON CONFLICT(Controller, Card) DO UPDATE SET
        StartDate = excluded.StartDate,
        EndDate   = excluded.EndDate,
        Door1     = excluded.Door1,
        Door2     = excluded.Door2,
        Door3     = excluded.Door3,
        Door4     = excluded.Door4,
        PIN       = excluded.PIN;`

const sqlDeleteCard = `DELETE FROM Cards WHERE Controller=? AND Card=?;`
const sqlDeleteAllCards = `DELETE FROM Cards WHERE Controller=?;`

func (db impl) GetCards(controller uint32) (uint32, error) {
	if rs, err := db.query(sqlGetCards, controller); err != nil {
		return 0, err
	} else {
		defer rs.Close()

		for rs.Next() {
			record := struct {
				cards uint32
			}{}

			pointers := []any{
				&record.cards,
			}

			if err := rs.Scan(pointers...); err != nil {
				return 0, err
			} else {
				return record.cards, nil
			}
		}

		return 0, entities.ErrRecordNotFound
	}
}

func (db impl) GetCard(controller uint32, card uint32) (*entities.Card, error) {
	if rs, err := db.query(sqlGetCard, controller, card); err != nil {
		return nil, err
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
				return nil, err
			}

			startDate := time.Time{}
			if v, err := time.ParseInLocation(time.DateOnly, record.startDate[:10], time.Local); err != nil {
				warnf("%v", err)
			} else {
				startDate = v
			}

			endDate := time.Time{}
			if v, err := time.ParseInLocation(time.DateOnly, record.endDate[:10], time.Local); err != nil {
				warnf("%v", err)
			} else {
				endDate = v
			}

			return &entities.Card{
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

		return nil, nil
	}
}

func (db impl) GetCardByIndex(controller uint32, index uint32) (*entities.Card, error) {
	if rs, err := db.query(sqlGetCardByIndex, controller, index); err != nil {
		return nil, err
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
				return nil, err
			}
			startDate := time.Time{}
			if v, err := time.ParseInLocation(time.DateOnly, record.startDate[:10], time.Local); err != nil {
				warnf("%v", err)
			} else {
				startDate = v
			}

			endDate := time.Time{}
			if v, err := time.ParseInLocation(time.DateOnly, record.endDate[:10], time.Local); err != nil {
				warnf("%v", err)
			} else {
				endDate = v
			}

			return &entities.Card{
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

		return nil, nil
	}
}

func (db impl) PutCard(controller uint32, card entities.Card) (uint32, error) {
	values := []any{
		controller,
		card.Card,
		card.StartDate.Format(time.DateOnly),
		card.EndDate.Format(time.DateOnly),
		card.Permissions[1],
		card.Permissions[2],
		card.Permissions[3],
		card.Permissions[4],
		card.PIN,
	}

	if id, err := db.insert(sqlPutCard, values...); err != nil {
		return 0, err
	} else {
		return uint32(id), nil
	}
}

func (db impl) DeleteCard(controller uint32, card uint32) (bool, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	zero := false
	query := sqlDeleteCard

	if dbc, err := db.open(); err != nil {
		return zero, err
	} else if prepared, err := dbc.Prepare(query); err != nil {
		return zero, err
	} else if rs, err := prepared.ExecContext(ctx, controller, card); err != nil {
		return zero, err
	} else if rs == nil {
		return zero, fmt.Errorf("invalid resultset (%v)", rs)
	} else if rows, err := rs.RowsAffected(); err != nil {
		return zero, fmt.Errorf("invalid resultset (%v)", rs)
	} else {
		return rows > 0, nil
	}
}

func (db impl) DeleteAllCards(controller uint32) (bool, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 100*time.Millisecond)

	defer cancel()

	zero := false
	query := sqlDeleteAllCards

	if dbc, err := db.open(); err != nil {
		return zero, err
	} else if prepared, err := dbc.Prepare(query); err != nil {
		return zero, err
	} else if rs, err := prepared.ExecContext(ctx, controller); err != nil {
		return zero, err
	} else if rs == nil {
		return zero, fmt.Errorf("invalid resultset (%v)", rs)
	} else if _, err := rs.RowsAffected(); err != nil {
		return zero, fmt.Errorf("invalid resultset (%v)", rs)
	} else {
		return true, nil
	}
}
