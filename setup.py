#!/usr/bin/env python
# -*- coding: utf-8 -*-

from setuptools import setup
import io
import os


def read(file_name):
    """Read a text file and return the content as a string."""
    with io.open(os.path.join(os.path.dirname(__file__), file_name),
                 encoding='utf-8') as f:
        return f.read()

setup(name='torri',
      description='Library for background reading/writing images and decoding/encoding with mmal for RPi',
      author='Ivan Usalko',
      author_email='ivict@rambler.ru',
      url="http://github.com/i-usalko/torri",
      version='0.1.1',
      py_modules=['torri'],
      long_description=read('README.md'),
      long_description_content_type='text/markdown',
      keywords="RPi MMAL Images",
      license="APACHE-2.0"
      python_requires='>=2.7, !=3.0.*, !=3.1.*, !=3.2.*, !=3.3.*, !=3.4.*',
      classifiers=[
          'Intended Audience :: Developers',
          'License :: OSI Approved :: MIT License',
          'Programming Language :: Python',
          'Programming Language :: Python :: 2',
          'Programming Language :: Python :: 2.7',
          'Programming Language :: Python :: 3',
          'Programming Language :: Python :: 3.5',
          'Programming Language :: Python :: 3.6',
          'Programming Language :: Python :: 3.7',
          'Programming Language :: Python :: 3.8',
          'Programming Language :: Python :: Implementation :: CPython',
      ],
      )
