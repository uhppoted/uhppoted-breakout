package logger

import (
	"fmt"
	"log/slog"
	"slices"
	"time"

	"github.com/pkg/term"
)

const TERMINAL_CLEAR = "\033c\033[2J"
const TERMINAL_QUERY_CODE = "\033[c"
const TERMINAL_QUERY_STATUS = "\033[5n"
const TERMINAL_QUERY_SIZE = "\033[s\033[999;999H\033[6n\033[u"
const TERMINAL_SET_SCROLL_AREA = "\033[s\033[0;%dr\033[u"
const TERMINAL_ECHO = "\033[s\033[%d;0H\033[0K>> %s\033[u"
const TERMINAL_CLEARLINE = "\033[s\033[%d;0H\033[0K>> \033[u"
const TERMINAL_DISPLAY = "\033[s\033[%d;0H\033[0K>> %s\033[u"
const TERMINAL_AT = "\033[s\033[%d;%dH\033[0K%s\033[u"

const ESC byte = 27
const CR byte = 13
const LF byte = 10

type Logger struct {
	Device  string
	escaped bool

	logmsg struct {
		buffer [256]byte
		ix     int
	}

	escape struct {
		buffer [32]byte
		ix     int
	}
}

func (l Logger) Run() {
	pipe := make(chan []byte)

	defer func() {
		close(pipe)
	}()

	go func() {
		for {
			select {
			case msg := <-pipe:
				l.received(msg)
			}
		}
	}()

	for {
		if err := l.listen(pipe); err != nil {
			errorf("breakout", "%v", err)
		}

		time.Sleep(1 * time.Second)
	}
}

func (l Logger) received(msg []byte) {
	for _, ch := range msg {
		if l.escaped || ch == ESC {
			l.escaped = true

			if l.escape.ix < len(l.escape.buffer) {
				l.escape.buffer[l.escape.ix] = ch
				l.escape.ix++

				if slices.Equal(l.escape.buffer[0:l.escape.ix], []byte(TERMINAL_QUERY_STATUS)) {
					l.escaped = false
					l.escape.ix = 0
					fmt.Println(">> query status")
				} else {
				}
			} else {
				fmt.Printf(">> N:%v len:%v  %v\n", l.escape.ix, len(l.escape.buffer), l.escape.buffer)
				l.escaped = false
				l.escape.ix = 0
			}
		} else if (ch == CR || ch == LF || l.logmsg.ix >= len(l.logmsg.buffer)) && l.logmsg.ix > 0 {
			fmt.Printf("%s\n", string(l.logmsg.buffer[0:l.logmsg.ix]))
			l.logmsg.ix = 0
		} else {
			l.logmsg.buffer[l.logmsg.ix] = ch
			l.logmsg.ix++
		}
	}
}

func (l Logger) listen(pipe chan []byte) error {
	if t, err := term.Open(l.Device, term.Speed(115200), term.RawMode); err != nil {
		return err
	} else {
		infof("breakout", "connected")

		// NTS: term.Close hangs indefinitely if a read is pending
		//      (seems happy enough to reopen the connection in any event)
		// defer func() {
		//  if err := t.Close(); err != nil {
		//      fmt.Printf("  *** ERROR %v\n", err)
		//  }
		// }()

		if err := t.SetRaw(); err != nil {
			return err
		}

		buffer := make([]byte, 256)

		for {
			if N, err := t.Read(buffer); err != nil {
				return err
			} else if N > 0 {
				pipe <- buffer[0:N]
			}
		}
	}

	return nil
}

func debugf(tag string, format string, args ...any) {
	f := fmt.Sprintf("%-16v %v", tag, format)
	msg := fmt.Sprintf(f, args...)

	slog.Debug(msg)
}

func infof(tag string, format string, args ...any) {
	f := fmt.Sprintf("%-16v %v", tag, format)
	msg := fmt.Sprintf(f, args...)

	slog.Info(msg)
}

func warnf(tag string, format string, args ...any) {
	f := fmt.Sprintf("%-16v %v", tag, format)
	msg := fmt.Sprintf(f, args...)

	slog.Warn(msg)
}

func errorf(tag string, format string, args ...any) {
	f := fmt.Sprintf("%-16v %v", tag, format)
	msg := fmt.Sprintf(f, args...)

	slog.Error(msg)
}
