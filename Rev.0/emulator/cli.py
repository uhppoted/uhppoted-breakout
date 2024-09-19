#!python3

import asyncio
import serial_asyncio

PORT = '/dev/tty.usbmodem14201'
BAUDRATE = 115200

async def ping(transport):
    while True:
        ping = b'\x16\x16\x05'
        print('... ping    ', ' '.join('{:02x}'.format(x) for x in ping))
        transport.write(ping)
        await asyncio.sleep(2)

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

loop = asyncio.get_event_loop()
coro = serial_asyncio.create_serial_connection(loop, OutputProtocol, PORT, baudrate=BAUDRATE)
transport, protocol = loop.run_until_complete(coro)

task = loop.create_task(ping(transport))

loop.run_forever()
loop.close()