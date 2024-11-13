#!/usr/bin/env python3

import asyncio
import serial_asyncio
import aioconsole

PORT = '/dev/serial0'
BAUDRATE = 115200

GET = [22,22,2,48,40,16,2,16,1,0,4,16,6,112,117,98,108,105,99,160,27,16,2,16,1,13,16,2,16,1,0,16,2,16,1,0,48,16,16,48,14,16,6,10,43,16,6,16,1,4,16,1,132,128,0,16,2,16,1,16,5,0,3,33,189]


class OutputProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
        print('... connected')

    def data_received(self, data):
        # print('... received:', repr(data))
        print('... received',' '.join('{:02x}'.format(x) for x in data))

    def connection_lost(self, exc):
        print('... closed')
        self.transport.loop.stop()

    def pause_writing(self):
        print('... paused')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('... resumed')

async def ping(transport):
      ping = b'\x16\x16\x05'
      print('... ping    ', ' '.join('{:02x}'.format(x) for x in ping))
      transport.write(ping)

async def get(transport):
      request = bytes(GET)
      print('... get     ', f'length:{len(request)}', f"\n                bytes: {' '.join('{:02x}'.format(x) for x in request)}")
      transport.write(request)

async def readln(transport):
    while True:
        cmd = await aioconsole.ainput(">> ")
        if cmd == 'exit':
           break
        elif cmd == 'ping':
           asyncio.create_task(ping(transport))
        elif cmd == 'get':
           asyncio.create_task(get(transport))

async def main():
    loop = asyncio.get_running_loop()
    sio = serial_asyncio.create_serial_connection(loop, OutputProtocol, PORT, baudrate=BAUDRATE)
    transport, protocol = await sio

    cli = asyncio.create_task(readln(transport))

    await asyncio.gather(cli)

asyncio.run(main())
