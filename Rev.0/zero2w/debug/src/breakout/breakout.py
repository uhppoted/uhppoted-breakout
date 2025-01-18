#!/usr/bin/env python3

import asyncio
import serial_asyncio
import aioconsole

PORT = '/dev/serial0'
BAUDRATE = 115200

GET = [22,22,2,48,40,16,2,16,1,0,4,16,6,112,117,98,108,105,99,160,27,16,2,16,1,13,16,2,16,1,0,16,2,16,1,0,48,16,16,48,14,16,6,10,43,16,6,16,1,4,16,1,132,128,0,16,2,16,1,16,5,0,3,33,189]
INTEGER = [22,22,2,16,2,16,1,13,3,193,16]
NULL = [22,22,2,16,5,0,3,18,236]
OCTETS = [22,22,2,4,16,6,112,117,98,108,105,99,3,170,217]
OID = [22,22,2,16,6,10,43,16,6,16,1,4,16,1,132,128,0,16,2,16,1,3,162,165]
SEQUENCE = [22,22,2,48,14,16,6,10,43,16,6,16,1,4,16,1,132,128,0,16,2,16,1,16,5,0,3,211,137]
SEQUENCES = [22,22,2,48,16,16,48,14,16,6,10,43,16,6,16,1,4,16,1,132,128,0,16,2,16,1,16,5,0,3,236,207]
PDU = [22,22,2,160,27,16,2,16,1,13,16,2,16,1,0,16,2,16,1,0,48,16,16,48,14,16,6,10,43,16,6,16,1,4,16,1,132,128,0,16,2,16,1,16,5,0,3,152,19]

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

async def pingx(transport):
      while True:
          ping = b'\x16\x16\x05'
          print('... ping    ', ' '.join('{:02x}'.format(x) for x in ping))
          transport.write(ping)
          await asyncio.sleep(1.1)

async def get(transport):
      request = bytes(GET)
      print('... get     ', f'length:{len(request)}', f"\n                bytes: {' '.join('{:02x}'.format(x) for x in request)}")
      transport.write(request)

async def getx(transport):
      while True:
          request = bytes(GET)
          print('... get     ', f'length:{len(request)}', f"\n                bytes: {' '.join('{:02x}'.format(x) for x in request)}")
          transport.write(request)
          await asyncio.sleep(1.3)

async def poke(transport):
      msg = bytes(b'***')
      print('... poke    ',' '.join('{:02x}'.format(x) for x in msg))
      transport.write(msg)

async def debug(transport):
      msg = bytes(SEQUENCES)
      print('... debug    ',' '.join('{:02x}'.format(x) for x in msg[0:10]),'....')
      transport.write(msg)

async def debugx(transport):
      counter = 0
      msg = bytes(SEQUENCES)
      while True:
          counter += 1
          print('... debug    ',counter, ' '.join('{:02x}'.format(x) for x in msg[0:10]),'....')
          transport.write(msg)
          await asyncio.sleep(1)

async def readln(transport):
    while True:
        cmd = await aioconsole.ainput(">> ")
        if cmd == 'exit':
           break
        elif cmd == 'ping':
           asyncio.create_task(ping(transport))
        elif cmd == 'pingx':
           asyncio.create_task(pingx(transport))
        elif cmd == 'get':
           asyncio.create_task(get(transport))
        elif cmd == 'getx':
           asyncio.create_task(getx(transport))
        elif cmd == 'poke':
           asyncio.create_task(poke(transport))
        elif cmd == 'debug':
           asyncio.create_task(debug(transport))
        elif cmd == 'debugx':
           asyncio.create_task(debugx(transport))

async def main():
    loop = asyncio.get_running_loop()
    sio = serial_asyncio.create_serial_connection(loop, OutputProtocol, PORT, baudrate=BAUDRATE)
    transport, protocol = await sio

    cli = asyncio.create_task(readln(transport))

    await asyncio.gather(cli)

asyncio.run(main())
