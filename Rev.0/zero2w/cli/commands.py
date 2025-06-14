import os
import ipaddress
import datetime
import pprint
import sys
import pathlib

from uhppoted import uhppote
from uhppoted import encode
from uhppoted import decode

import tls
import pool

CONTROLLER = 405419896
DOOR = 3
MODE = 2
DELAY = 10
CARD = 10058400
CARD_INDEX = 17
TIME_PROFILE_ID = 29
AUTO_SEND = 15

ADDRESS = ipaddress.IPv4Address('192.168.1.100')
NETMASK = ipaddress.IPv4Address('255.255.255.0')
GATEWAY = ipaddress.IPv4Address('192.168.1.1')
LISTENER = (ipaddress.IPv4Address('192.168.1.100'), 60001)


def commands():
    return {
        'get-all-controllers': get_all_controllers,
        'get-controller': get_controller,
        'set-IPv4': set_IPv4,
        'get-time': get_time,
        'set-time': set_time,
        'get-listener': get_listener,
        'set-listener': set_listener,
        'get-door': get_door,
        'set-door': set_door,
        'get-status': get_status,
        'open-door': open_door,
        'get-cards': get_cards,
        'get-card': get_card,
        'get-card-by-index': get_card_by_index,
        'put-card': put_card,
        'delete-card': delete_card,
        'delete-all-cards': delete_all_cards,
        'get-event': get_event,
        'get-event-index': get_event_index,
        'set-event-index': set_event_index,
        'record-special-events': record_special_events,
        'get-time-profile': get_time_profile,
        'set-time-profile': set_time_profile,
        'clear-time-profiles': clear_time_profiles,
        'add-task': add_task,
        'refresh-tasklist': refresh_tasklist,
        'clear-tasklist': clear_tasklist,
        'set-pc-control': set_pc_control,
        'set-interlock': set_interlock,
        'activate-keypads': activate_keypads,
        'set-door-passcodes': set_door_passcodes,
        'restore-default-parameters': restore_default_parameters,
        'listen': listen,
    }


def exec(f, args):
    bind = args.bind
    broadcast = args.broadcast
    listen = args.listen
    debug = args.debug

    dest = args.destination
    timeout = args.timeout
    protocol = args.protocol

    u = uhppote.Uhppote(bind, broadcast, listen, debug)
    response = f(u, dest, timeout, args, protocol=protocol)

    if response != None:
        if type(response).__name__ == 'list':
            for v in response:
                pprint.pprint(v.__dict__, indent=2, width=1, sort_dicts=False)
        elif type(response).__name__ == 'bool':
            pprint.pprint(response, indent=2, width=1, sort_dicts=False)
        else:
            pprint.pprint(response.__dict__, indent=2, width=1, sort_dicts=False)


def get_all_controllers(u, dest, timeout, args, protocol='udp'):
    return u.get_all_controllers(timeout=timeout)


def get_controller(u, dest, timeout, args, protocol='udp'):
    if protocol == 'tls':
        request = encode.get_controller_request(CONTROLLER)
        bind = '0.0.0.0'

        reply = _tls(request, bind, dest, timeout, True)
        if reply != None:
            return decode.get_controller_response(reply)
        else:
            return None

    elif protocol == 'tcp::pool':
        request = encode.get_controller_request(CONTROLLER)
        bind = '0.0.0.0'

        reply = _pool(request, bind, dest, timeout, True)
        if reply != None:
            return decode.get_controller_response(reply)
        else:
            return None

    else:
        controller = (CONTROLLER, dest, protocol)

        return u.get_controller(controller, timeout=timeout)


def debug_get_controller(u, dest, timeout, args, protocol):
    return u.get_controller((405419897, '192.168.1.100:60000', 'tcp'), timeout=2.5)

    # request = encode.get_controller_request(405419897)
    # bind = '0.0.0.0'
    #
    # reply = _tls(request, bind, 'localhost:60443', timeout, True)
    # if reply != None:
    #     return decode.get_controller_response(reply)
    # else:
    #     return None


def set_IPv4(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    address = ADDRESS
    netmask = NETMASK
    gateway = GATEWAY

    return u.set_ip(controller, address, netmask, gateway, timeout=timeout)


def get_time(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.get_time(controller, timeout=timeout)


def set_time(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    now = datetime.datetime.now()

    return u.set_time(controller, now, timeout=timeout)


def get_listener(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.get_listener(controller, timeout=timeout)


def set_listener(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    (address, port) = LISTENER
    interval = AUTO_SEND

    return u.set_listener(controller, address, port, interval, timeout=timeout)


def get_door(u, dest, timeout, args, protocol='udp'):
    controller = (args.controller, dest, protocol)
    door = args.door

    return u.get_door_control(controller, door, timeout=timeout)


def set_door(u, dest, timeout, args, protocol='udp'):
    controller = (args.controller, dest, protocol)
    door = args.door
    mode = args.mode
    delay = args.delay

    if delay < 1 or delay > 60:
        raise ValueError(f'invalid door unlock duration {delay}')

    if mode == 'normally-open':
        return u.set_door_control(controller, door, 1, delay, timeout=timeout)
    elif mode == 'normally-closed':
        return u.set_door_control(controller, door, 2, delay, timeout=timeout)
    elif mode == 'controlled':
        return u.set_door_control(controller, door, 3, delay, timeout=timeout)
    else:
        raise ValueError(f'invalid door control mode {mode}')


def get_status(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.get_status(controller, timeout=timeout)


def open_door(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    door = DOOR

    return u.open_door(controller, door, timeout=timeout)


def get_cards(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.get_cards(controller, timeout=timeout)


def get_card(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    card = CARD

    response = u.get_card(controller, card, timeout=timeout)
    if response.card_number == 0:
        raise ValueError(f'card {card} not found')

    return response


def get_card_by_index(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    index = CARD_INDEX

    response = u.get_card_by_index(controller, index, timeout=timeout)
    if response.card_number == 0:
        raise ValueError(f'card @ index {index} not found')
    elif response.card_number == 0xffffffff:
        raise ValueError(f'card @ index {index} deleted')

    return response


def put_card(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    card = CARD
    start = datetime.datetime.strptime("2024-01-01", '%Y-%m-%d').date()
    end = datetime.datetime.strptime("2024-12-31", '%Y-%m-%d').date()
    door1 = 0  # no access
    door2 = 1  # 24/7 access
    door3 = 29  # time_profile
    door4 = 0  # no access
    PIN = 7531

    return u.put_card(controller, card, start, end, door1, door2, door3, door4, PIN, timeout=timeout)


def delete_card(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    card = CARD

    return u.delete_card(controller, card, timeout=timeout)


def delete_all_cards(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.delete_all_cards(controller, timeout=timeout)


def get_event(u, dest, timeout, args, protocol='udp'):
    controller = (args.controller, dest, protocol)
    index = args.event

    response = u.get_event(controller, index, timeout=timeout)
    if response.event_type == 0xff:
        raise ValueError(f'event @ index {index} overwritten')
    elif response.index == 0:
        raise ValueError(f'event @ index {index} not found')

    return response


def get_event_index(u, dest, timeout, args, protocol='udp'):
    controller = (args.controller, dest, protocol)

    return u.get_event_index(controller, timeout=timeout)


def set_event_index(u, dest, timeout, args, protocol='udp'):
    controller = (args.controller, dest, protocol)
    index = args.index

    return u.set_event_index(controller, index, timeout=timeout)


def record_special_events(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    enabled = args.enabled

    return u.record_special_events(controller, enabled, timeout=timeout)


def get_time_profile(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    profile_id = TIME_PROFILE_ID
    response = u.get_time_profile(controller, profile_id, timeout=timeout)

    if response.profile_id == 0:
        raise ValueError(f'time profile {profile_id} not defined')

    return response


def set_time_profile(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    profile_id = TIME_PROFILE_ID
    start = datetime.datetime.strptime("2022-01-01", '%Y-%m-%d').date()
    end = datetime.datetime.strptime("2022-12-31", '%Y-%m-%d').date()
    monday = True
    tuesday = False
    wednesday = True
    thursday = True
    friday = False
    saturday = False
    sunday = True
    segment1start = datetime.datetime.strptime("08:15", '%H:%M').time()
    segment1end = datetime.datetime.strptime("11:45", '%H:%M').time()
    segment2start = datetime.datetime.strptime("12:45", '%H:%M').time()
    segment2end = datetime.datetime.strptime("17:15", '%H:%M').time()
    segment3start = datetime.datetime.strptime("19:30", '%H:%M').time()
    segment3end = datetime.datetime.strptime("22:00", '%H:%M').time()
    linked_profile_ID = 23

    # yapf: disable
    return u.set_time_profile(controller,
                              profile_id,
                              start, end,
                              monday, tuesday, wednesday, thursday, friday, saturday, sunday,
                              segment1start, segment1end,
                              segment2start, segment2end,
                              segment3start, segment3end,
                              linked_profile_ID,
                              timeout=timeout)
    # yapf: enable


def clear_time_profiles(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.delete_all_time_profiles(controller, timeout=timeout)


def add_task(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    start_date = datetime.datetime.strptime("2022-01-01", '%Y-%m-%d').date()
    end_date = datetime.datetime.strptime("2022-12-31", '%Y-%m-%d').date()
    monday = True
    tuesday = False
    wednesday = True
    thursday = True
    friday = False
    saturday = False
    sunday = True
    start_time = datetime.datetime.strptime("08:15", '%H:%M').time()
    door = DOOR
    task_type = 2
    more_cards = 0

    # yapf: disable
    return u.add_task(controller,
                      start_date, end_date,
                      monday, tuesday, wednesday, thursday, friday, saturday, sunday,
                      start_time,
                      door,
                      task_type,
                      more_cards,
                      timeout=timeout)
    # yapf: enable


def refresh_tasklist(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.refresh_tasklist(controller, timeout=timeout)


def clear_tasklist(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.clear_tasklist(controller, timeout=timeout)


def set_pc_control(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    enabled = True

    return u.set_pc_control(controller, enabled, timeout=timeout)


def set_interlock(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    interlock = 3

    return u.set_interlock(controller, interlock, timeout=timeout)


def activate_keypads(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    reader1 = True
    reader2 = True
    reader3 = False
    reader4 = True

    return u.activate_keypads(controller, reader1, reader2, reader3, reader4, timeout=timeout)


def set_door_passcodes(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)
    door = DOOR
    passcode1 = 12345
    passcode2 = 0
    passcode3 = 999999
    passcode4 = 54321

    return u.set_door_passcodes(controller, door, passcode1, passcode2, passcode3, passcode4, timeout=timeout)


def restore_default_parameters(u, dest, timeout, args, protocol='udp'):
    controller = (CONTROLLER, dest, protocol)

    return u.restore_default_parameters(controller, timeout=timeout)


def listen(u, dest, timeout, args, protocol='udp'):
    return u.listen(onEvent)


def onEvent(event):
    if event != None:
        pprint.pprint(event.__dict__, indent=2, width=1)


# INTERNAL: TLS handler
def _tls(request, bind, dest, timeout, debug):
    transport = tls.TLS(bind, debug)

    return transport.send(request, dest, timeout)


# INTERNAL: pooled TCP handler
def _pool(request, bind, dest, timeout, debug):
    transport = pool.Pool(bind, debug)

    return transport.send(request, dest, timeout)
