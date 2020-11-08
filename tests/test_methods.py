import os
import unittest
from torri import Torri
from timeit import default_timer as timer
from cv2 import cv2
import numpy as  np


class TestMethods(unittest.TestCase):

    def test_case_one(self):
        t = Torri()
        print(t.decode_jpeg('Not exist path!', 1920, 1080))
        self.assertTrue(True)

    def test_case_two(self):
        t = Torri()
        print(t.gencmd('get_throttled'))
        self.assertTrue(True)

    @unittest.skip  # Manual run only
    def test_case_three(self):
        t = Torri()
        time = timer()
        data = t.decode_jpeg('/media/pi/Transcend/.mock-camera-images/2020-06-22-07-10-39.72866b38fcdb4b8ba0c76f2ba48d7c67-v.jpg', 1920, 1080)
        print(f'Execution time decoding is {timer() - time}s')
        #width = 1920
        #height = 1080
        #size = width * height
        rgb_data = np.array(data, copy=False, dtype=np.uint8)
        rgb_data = rgb_data.reshape((1080, 1920, 3))
        print(f'Execution time total is {timer() - time}s')

        success, image_byte_array = cv2.imencode('.jpeg',
                                                rgb_data,
                                                [cv2.IMWRITE_JPEG_QUALITY, 100])
        with open('test.jpeg', 'wb') as writer:
            writer.write(image_byte_array)
            writer.flush()
            os.fsync(writer.fileno())

        self.assertTrue(True)

    @unittest.skip  # Manual run only
    def test_case_four(self):
        time = timer()
        with open('/media/pi/Transcend/.mock-camera-images/2020-06-22-07-10-39.72866b38fcdb4b8ba0c76f2ba48d7c67-v.jpg', 'rb') as reader:
            image_data = np.frombuffer(reader.read(), dtype=np.uint8)
            rgb_data = cv2.imdecode(image_data, 1)
        print(f'Execution time is {timer() - time}s')
        print(f'RGB data shape is : {rgb_data.shape}')

        success, image_byte_array = cv2.imencode('.jpeg',
                                                rgb_data,
                                                [cv2.IMWRITE_JPEG_QUALITY, 100])
        with open('test-control.jpeg', 'wb') as writer:
            writer.write(image_byte_array)
            writer.flush()
            os.fsync(writer.fileno())

        self.assertTrue(True)


if __name__ == '__main__':
    unittest.main()
