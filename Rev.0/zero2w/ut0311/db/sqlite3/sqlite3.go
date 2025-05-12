package sqlite3

import (
	// "database/sql"
	"fmt"

	_ "github.com/mattn/go-sqlite3"

	"emulator/entities"
)

// const MaxLifetime = 5 * time.Minute
// const MaxIdle = 2
// const MaxOpen = 5
// const LogTag = "sqlite3"
//
// type record map[string]any

type impl struct {
	dsn string
}

func NewDB(dsn string) impl {
	return impl{
		dsn: dsn,
	}
}

func (db impl) PutEvent(event entities.Event) (uint32, error) {
	return 0, fmt.Errorf("** NOT IMPLEMENTED **")
}

// func open(path string, maxLifetime time.Duration, maxOpen int, maxIdle int) (*sql.DB, error) {
//     dbc, err := sql.Open("sqlite3", path)
//     if err != nil {
//         return nil, err
//     }
//
//     dbc.SetConnMaxLifetime(maxLifetime)
//     dbc.SetMaxOpenConns(maxOpen)
//     dbc.SetMaxIdleConns(maxIdle)
//
//     return dbc, nil
// }
