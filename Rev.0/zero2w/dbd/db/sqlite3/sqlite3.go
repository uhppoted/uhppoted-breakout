package sqlite3

import (
	"database/sql"
	"time"

	"dbd/log"
)

const MaxLifetime = 5 * time.Minute
const MaxIdle = 2
const MaxOpen = 5
const LogTag = "sqlite3"

const tableController = "Controller"
const tableCards = "Cards"
const tableEvents = "Events"

type driver interface {
	open() (*sql.DB, error)
	query(sql string, args ...any) (*sql.Rows, error)
	insert(sql string, values ...any) (int64, error)
	update(sql string, values ...any) error
}

type impl struct {
	dsn    string
	driver driver
}

func Mattn(dsn string) impl {
	return impl{
		dsn: dsn,
		driver: &mattn{
			dsn:         dsn,
			maxLifetime: MaxLifetime,
			maxIdle:     MaxIdle,
			maxOpen:     MaxOpen,
		},
	}
}

func ModernC(dsn string) impl {
	return impl{
		dsn: dsn,
		driver: &modernc{
			dsn:         dsn,
			maxLifetime: MaxLifetime,
			maxIdle:     MaxIdle,
			maxOpen:     MaxOpen,
		},
	}
}

func (db impl) open() (*sql.DB, error) {
	return db.driver.open()
}

func (db impl) query(sql string, args ...any) (*sql.Rows, error) {
	return db.driver.query(sql, args...)
}

func (db impl) insert(sql string, values ...any) (int64, error) {
	return db.driver.insert(sql, values...)
}

func (db impl) update(sql string, values ...any) error {
	return db.driver.update(sql, values...)
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
