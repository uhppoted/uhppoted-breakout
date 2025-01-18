'''
SSMP bisync message codec.

Wraps a message in a SYN-SYN-SOH..STX..ETX-CRC sequence.

'''

from typing import Final

from .CRC import CRC16

SYN: Final = 22
SOH: Final = 1
STX: Final = 2
ETX: Final = 3


def encode(header, packet):
    '''
    Wraps a message in a SYN-SYN-SOH..STX..ETX-CRC sequence.

        Parameters:
            header (bytearray)  Optional message header.
            packet (bytearray)  Message content.

        Returns:
            Header + packet wrapped in a bisync envelope.
    '''

    encoded = bytearray(0)
    encoded.append(SYN)
    encoded.append(SYN)

    if header:
        encoded.append(SOH)
        encoded.extend(header)

    encoded.append(STX)
    if packet:
        encoded.extend(packet)
    encoded.append(ETX)

    crc = CRC16(0xffff, encoded[2:])

    encoded.append((crc >> 8) & 0x00ff)
    encoded.append((crc >> 0) & 0x00ff)

    return encoded
