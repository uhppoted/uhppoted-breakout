package scmp

import ()

var OID_CONTROLLER_ID = OID{1, 3, 6, 1, 4, 1, 65536, 2, 1}
var OID_CONTROLLER_VERSION = OID{1, 3, 6, 1, 4, 1, 65536, 2, 2}
var OID_CONTROLLER_RELEASED = OID{1, 3, 6, 1, 4, 1, 65536, 2, 3}
var OID_CONTROLLER_ADDRESS = OID{1, 3, 6, 1, 4, 1, 65536, 2, 4}
var OID_CONTROLLER_NETMASK = OID{1, 3, 6, 1, 4, 1, 65536, 2, 5}
var OID_CONTROLLER_GATEWAY = OID{1, 3, 6, 1, 4, 1, 65536, 2, 6}
var OID_CONTROLLER_MAC = OID{1, 3, 6, 1, 4, 1, 65536, 2, 7}
var OID_CONTROLLER_DATETIME = OID{1, 3, 6, 1, 4, 1, 65536, 2, 8}
var OID_CONTROLLER_ERROR = OID{1, 3, 6, 1, 4, 1, 65536, 2, 9}
var OID_CONTROLLER_SPECIAL_INFO = OID{1, 3, 6, 1, 4, 1, 65536, 2, 10}
var OID_CONTROLLER_SEQUENCE_NUMBER = OID{1, 3, 6, 1, 4, 1, 65536, 2, 11}
var OID_CONTROLLER_EVENT_LISTENER = OID{1, 3, 6, 1, 4, 1, 65536, 2, 12}
var OID_CONTROLLER_UPLOAD_INTERVAL = OID{1, 3, 6, 1, 4, 1, 65536, 2, 13}

var OID_DOORS_1_MODE = OID{1, 3, 6, 1, 4, 1, 65536, 3, 1, 1}
var OID_DOORS_1_DELAY = OID{1, 3, 6, 1, 4, 1, 65536, 3, 1, 2}
var OID_DOORS_1_PASSCODES = OID{1, 3, 6, 1, 4, 1, 65536, 3, 1, 3}
var OID_DOORS_1_LOCK = OID{1, 3, 6, 1, 4, 1, 65536, 3, 1, 4}
var OID_DOORS_1_OPEN = OID{1, 3, 6, 1, 4, 1, 65536, 3, 1, 5}
var OID_DOORS_1_BUTTON = OID{1, 3, 6, 1, 4, 1, 65536, 3, 1, 6}

var OID_DOORS_2_MODE = OID{1, 3, 6, 1, 4, 1, 65536, 3, 2, 1}
var OID_DOORS_2_DELAY = OID{1, 3, 6, 1, 4, 1, 65536, 3, 2, 2}
var OID_DOORS_2_PASSCODES = OID{1, 3, 6, 1, 4, 1, 65536, 3, 2, 3}
var OID_DOORS_2_LOCK = OID{1, 3, 6, 1, 4, 1, 65536, 3, 2, 4}
var OID_DOORS_2_OPEN = OID{1, 3, 6, 1, 4, 1, 65536, 3, 2, 5}
var OID_DOORS_2_BUTTON = OID{1, 3, 6, 1, 4, 1, 65536, 3, 2, 6}

var OID_DOORS_3_MODE = OID{1, 3, 6, 1, 4, 1, 65536, 3, 3, 1}
var OID_DOORS_3_DELAY = OID{1, 3, 6, 1, 4, 1, 65536, 3, 3, 2}
var OID_DOORS_3_PASSCODES = OID{1, 3, 6, 1, 4, 1, 65536, 3, 3, 3}
var OID_DOORS_3_LOCK = OID{1, 3, 6, 1, 4, 1, 65536, 3, 3, 4}
var OID_DOORS_3_OPEN = OID{1, 3, 6, 1, 4, 1, 65536, 3, 3, 5}
var OID_DOORS_3_BUTTON = OID{1, 3, 6, 1, 4, 1, 65536, 3, 3, 6}

var OID_DOORS_4_MODE = OID{1, 3, 6, 1, 4, 1, 65536, 3, 4, 1}
var OID_DOORS_4_DELAY = OID{1, 3, 6, 1, 4, 1, 65536, 3, 4, 2}
var OID_DOORS_4_PASSCODES = OID{1, 3, 6, 1, 4, 1, 65536, 3, 4, 3}
var OID_DOORS_4_LOCK = OID{1, 3, 6, 1, 4, 1, 65536, 3, 4, 4}
var OID_DOORS_4_OPEN = OID{1, 3, 6, 1, 4, 1, 65536, 3, 4, 5}
var OID_DOORS_4_BUTTON = OID{1, 3, 6, 1, 4, 1, 65536, 3, 4, 6}

var OID_INPUTS_TAMPER_DETECT = OID{1, 3, 6, 1, 4, 1, 65536, 4, 1}
var OID_INPUTS_FIRE_ALARM = OID{1, 3, 6, 1, 4, 1, 65536, 4, 2}

var OID_CARDS_COUNT = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 1}
var OID_CARDS_CARD = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff}
var OID_CARDS_INDEX = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 3, 0xffffffff}
var OID_CARDS_CARD_NUMBER = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 1}
var OID_CARDS_CARD_START_DATE = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 2}
var OID_CARDS_CARD_END_DATE = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 3}
var OID_CARDS_CARD_DOOR1 = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 4, 1}
var OID_CARDS_CARD_DOOR2 = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 4, 2}
var OID_CARDS_CARD_DOOR3 = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 4, 3}
var OID_CARDS_CARD_DOOR4 = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 4, 4}
var OID_CARDS_CARD_PIN = OID{1, 3, 6, 1, 4, 1, 65536, 3, 5, 0, 2, 0xffffffff, 5}

var OID_EVENTS_FIRST = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0, 1}
var OID_EVENTS_LAST = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0, 2}
var OID_EVENTS_EVENT = OID{1, 3, 6, 1, 4, 1, 65536, 6}
var OID_EVENTS_EVENT_EVENT = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 1}
var OID_EVENTS_EVENT_GRANTED = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 2}
var OID_EVENTS_EVENT_DOOR = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 3}
var OID_EVENTS_EVENT_DIRECTION = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 4}
var OID_EVENTS_EVENT_CARD = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 5}
var OID_EVENTS_EVENT_TIMESTAMP = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 6}
var OID_EVENTS_EVENT_REASON = OID{1, 3, 6, 1, 4, 1, 65536, 6, 0xffffffff, 7}