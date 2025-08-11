package eventd

import (
	"net/rpc"

	"ut0311/entities"
)

func (e *Events) GetCard(card uint32) (entities.Card, error) {
	debugf("get-card %v", card)

	var record entities.Card

	if client, err := rpc.DialHTTP(e.dial.network, e.dial.address); err != nil {
		return entities.Card{}, err
	} else if err := client.Call("CardD.GetCard", card, &record); err != nil {
		return entities.Card{}, err
	} else {
		return record, nil
	}
}
