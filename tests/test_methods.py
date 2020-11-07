import unittest
from torri import Torri

class TestMethods(unittest.TestCase):

    def test_case_one(self):
        t = Torri()
        print(t.decode_jpeg('Not exist path!'))
        self.assertTrue(True)

    def test_case_two(self):
        t = Torri()
        print(t.gencmd('get_throttled'))
        self.assertTrue(True)

if __name__ == '__main__':
    unittest.main()