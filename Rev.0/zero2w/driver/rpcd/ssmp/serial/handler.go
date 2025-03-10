package serial

import ()

type handler struct {
	onENQ     func()
	onACK     func()
	onMessage func(header []uint8, data []uint8)
}

func (h handler) OnENQ() {
	if h.onENQ != nil {
		h.onENQ()
	}
}

func (h handler) OnACK() {
	if h.onACK != nil {
		h.onACK()
	}
}

func (h handler) OnMessage(header []uint8, content []uint8) {
	if h.onMessage != nil {
		h.onMessage(header, content)
	}
}
