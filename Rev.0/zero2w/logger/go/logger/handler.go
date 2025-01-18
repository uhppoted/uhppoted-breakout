package logger

import (
	"context"
	"fmt"
	"log/slog"
)

type handler struct {
}

func (h *handler) Enabled(ctx context.Context, level slog.Level) bool {
	return true
}

func (h *handler) Handle(ctx context.Context, record slog.Record) error {
	timestamp := record.Time.Format("2006-01-02 15:04:05")

	fmt.Printf("%v %-5v  %v\n", timestamp, record.Level, record.Message)

	return nil
}

func (h *handler) WithAttrs(attrs []slog.Attr) slog.Handler {
	return h
}

func (h *handler) WithGroup(name string) slog.Handler {
	return h
}
