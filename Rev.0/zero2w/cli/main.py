#!python3

import argparse
import sys
import time
import traceback

from trace import Trace
from commands import commands
from commands import exec
from commands import get_controller
from commands import debug_get_controller


def main():
    if len(sys.argv) < 2:
        usage()
        return -1

    parser = argparse.ArgumentParser(description='uhppoted-breakout CLI')

    parser.add_argument('command', type=str, help='command')

    parser.add_argument('--bind', type=str, default='0.0.0.0', help='UDP IPv4 bind address. Defaults to 0.0.0.0:0')

    parser.add_argument('--broadcast',
                        type=str,
                        default='255.255.255.255:60000',
                        help='UDP IPv4 broadcast address. Defaults to 255.255.255.255:60000')

    parser.add_argument('--listen',
                        type=str,
                        default='0.0.0.0:60001',
                        help='UDP IPv4 event listener bind address. Defaults to 0.0.0.0:60001')

    parser.add_argument('--debug',
                        action=argparse.BooleanOptionalAction,
                        default=False,
                        help='displays sent and received packets')

    parser.add_argument('--destination',
                        '-d',
                        type=str,
                        default=None,
                        help='(optional) controller IPv4 address:port. Defaults to broadcast address.')

    parser.add_argument('--timeout',
                        '-t',
                        type=float,
                        default=2.5,
                        help='(optional) operation timeout (in seconds). Defaults to 2.5.')

    parser.add_argument('--protocol', choices=['udp', 'tcp', 'tcp::pool', 'tls'], default='udp', help='transport protocol')

    args = parser.parse_args()
    cmd = args.command
    debug = args.debug

    if cmd == 'all':
        for c, fn in commands().items():
            if c != 'listen':
                try:
                    exec(fn, args)
                except Exception as x:
                    print()
                    print(f'*** ERROR  {cmd}: {x}')
                    print()
    elif cmd == 'debug':
        try:
           debug_rate_limit(args)
        except Exception as x:
            print()
            print(f'*** ERROR  {cmd}: {x}')
            print()
            if debug:
                print(traceback.format_exc())

            sys.exit(1)

    elif cmd in commands():
        try:
            exec(commands()[cmd], args)
        except Exception as x:
            print()
            print(f'*** ERROR  {cmd}: {x}')
            print()
            if debug:
                print(traceback.format_exc())

            sys.exit(1)
    else:
        print()
        print(f'  ERROR: invalid command ({cmd})')
        print()

def debug_rate_limit(args):
    start= time.time()
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    exec(get_controller, args)
    print("--- %s seconds ---" % (time.time() - start))

def debug_tcp_read(args):
    exec(debug_get_controller, args)

def debug_connection_pool(args):
    for i in range(3):
        exec(commands()['get-controller'], args)
        time.sleep(5)

def usage():
    print()
    print('  Usage: python3 main.py <command>')
    print()
    print('  Supported commands:')

    for cmd, _ in commands().items():
        print(f'    {cmd}')

    print()


if __name__ == '__main__':
    main()
