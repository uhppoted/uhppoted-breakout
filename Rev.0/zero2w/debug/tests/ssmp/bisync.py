'''
SSMP bisync codec unit tests.

Tests the BISYNC encode and decode functions.
'''
import unittest

from breakout.ssmp import bisync


class TestBisyncEncode(unittest.TestCase):

    def test_encode_empty_packet(self):
        '''
        Test encoding for an empty SSMP packet without a header
        '''
        packet = []
        expected = bytearray([22, 22, 2, 3, 0xbe, 0xf8])
        bytes = bisync.encode(None, packet)
        self.assertEqual(expected, bytes)

    # def test_encode_GET(self):
    #     '''
    #     Tests encoding for an SSMP GET request.
    #     '''
    #     bytes = [
    #         48, 40, 2, 1, 0, 4, 6, 112, 117, 98, 108, 105, 99, 160, 27, 2, 1, 13, 2, 1, 0, 2, 1, 0, 48, 16, 48, 14, 6,
    #         10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1, 5, 0
    #     ]
    #     expected = [
    #         22, 22, 2, 48, 40, 16, 2, 16, 1, 0, 4, 16, 6, 112, 117, 98, 108, 105, 99, 160, 27, 16, 2, 16, 1, 13, 16, 2,
    #         16, 1, 0, 16, 2, 16, 1, 0, 48, 16, 16, 48, 14, 16, 6, 10, 43, 16, 6, 16, 1, 4, 16, 1, 132, 128, 0, 16, 2,
    #         16, 1, 16, 5, 0, 3, 33, 189
    #     ]
    #     self.fail('oooops')


if __name__ == '__main__':
    unittest.main()
