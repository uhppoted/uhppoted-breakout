#!/usr/bin/env python3

import asyncio
import serial_asyncio
import aioconsole

PORT = '/dev/tty.usbmodem14201'
BAUDRATE = 115200


class SSMP(asyncio.Protocol):

    def connection_made(self, transport):
        self.transport = transport
        print('... connected')

    def data_received(self, data):
        # print('... received:', repr(data))
        print('... received', ' '.join('{:02x}'.format(x) for x in data))

    def connection_lost(self, exc):
        print('... closed')
        self.transport.loop.stop()

    def pause_writing(self):
        print('... paused')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('... resumed')


async def readln(transport):
    while True:
        cmd = await aioconsole.ainput(">> ")
        if cmd == 'exit':
            break


async def main():
    print()
    print("   ** uhppoted-breakout - driver simulator v0.0.0")
    print()

    loop = asyncio.get_running_loop()
    sio = serial_asyncio.create_serial_connection(loop, SSMP, PORT, baudrate=BAUDRATE)
    transport, protocol = await sio

    cli = asyncio.create_task(readln(transport))

    await asyncio.gather(cli)


asyncio.run(main())
