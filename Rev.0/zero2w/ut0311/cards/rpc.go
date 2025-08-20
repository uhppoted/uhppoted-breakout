package cards

import (
	"net/rpc"
	"time"

	"ut0311/entities"
)

func (c *Cards) GetCards(controller uint32) (uint32, error) {
	debugf("get-cards %v", controller)

	var cards uint32

	if client, err := rpc.DialHTTP(c.dial.network, c.dial.address); err != nil {
		return 0, err
	} else if err := client.Call("CardD.GetCards", controller, &cards); err != nil {
		return 0, err
	} else {
		return cards, nil
	}
}

func (c *Cards) GetCard(controller uint32, card uint32) (entities.Card, error) {
	debugf("get-card %v %v", controller, card)

	var args = struct {
		Controller uint32
		Card       uint32
	}{
		Controller: controller,
		Card:       card,
	}

	var record entities.Card

	if client, err := rpc.DialHTTP(c.dial.network, c.dial.address); err != nil {
		return entities.Card{}, err
	} else if err := client.Call("CardD.GetCard", args, &record); err != nil {
		return entities.Card{}, err
	} else {
		return record, nil
	}
}

func (c *Cards) PutCard(controller uint32, card entities.Card) error {
	debugf("put-card %v %v", controller, card)

	var args = struct {
		Controller uint32
		Card       uint32
		StartDate  time.Time
		EndDate    time.Time
		Door1      uint8
		Door2      uint8
		Door3      uint8
		Door4      uint8
		PIN        uint32
	}{
		Controller: controller,
		Card:       card.Card,
		StartDate:  card.StartDate,
		EndDate:    card.EndDate,
		Door1:      card.Permissions[1],
		Door2:      card.Permissions[2],
		Door3:      card.Permissions[3],
		Door4:      card.Permissions[4],
		PIN:        card.PIN,
	}

	var index uint32

	if client, err := rpc.DialHTTP(c.dial.network, c.dial.address); err != nil {
		return err
	} else if err := client.Call("CardD.PutCard", args, &index); err != nil {
		return err
	} else {
		return nil
	}
}

func (c *Cards) DeleteCard(controller uint32, card uint32) (bool, error) {
	debugf("delete-card %v %v", controller, card)

	var args = struct {
		Controller uint32
		Card       uint32
	}{
		Controller: controller,
		Card:       card,
	}

	var ok bool

	if client, err := rpc.DialHTTP(c.dial.network, c.dial.address); err != nil {
		return false, err
	} else if err := client.Call("CardD.DeleteCard", args, &ok); err != nil {
		return false, err
	} else {
		return ok, nil
	}
}

func (c *Cards) DeleteAllCards(controller uint32) (bool, error) {
	debugf("delete-all-cards %v", controller)

	var ok bool

	if client, err := rpc.DialHTTP(c.dial.network, c.dial.address); err != nil {
		return false, err
	} else if err := client.Call("CardD.DeleteAllCards", controller, &ok); err != nil {
		return false, err
	} else {
		return ok, nil
	}
}
