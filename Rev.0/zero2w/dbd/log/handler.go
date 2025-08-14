package log

import (
	"context"
	"fmt"
	"log/slog"
	"time"
)

type handler struct {
}

func (h *handler) Enabled(ctx context.Context, level slog.Level) bool {
	return true
}

func (h *handler) Handle(ctx context.Context, record slog.Record) error {
	timestamp := record.Time.Format(time.DateTime)

	fmt.Printf("%v %-5v  %v\n", timestamp, record.Level, record.Message)

	return nil
}

func (h *handler) WithAttrs(attrs []slog.Attr) slog.Handler {
	return h
}

func (h *handler) WithGroup(name string) slog.Handler {
	return h
}
