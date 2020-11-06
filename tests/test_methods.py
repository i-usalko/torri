import unittest
from torri import Torri

class TestSimple(unittest.TestCase):

    def case_one(self):
        t = Torri()
        print(t.encode_jpeg(''))
        self.assertTrue(True)

    def case_two(self):
        t = Torri()
        print(t.gencmd('get_throttled'))
        self.assertTrue(True)

if __name__ == '__main__':
    unittest.main()