#!/usr/bin/env python
# -*- coding: utf-8 -*-

from setuptools import setup, Extension
from Cython.Build import cythonize
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
    license='Apache License 2.0',
    python_requires='>=2.7, !=3.0.*, !=3.1.*, !=3.2.*, !=3.3.*, !=3.4.*',
    classifiers=[
        'Development Status :: 1 - Betta',
        'Intended Audience :: Developers',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: Implementation :: CPython',
    ],
    ext_modules = cythonize(
        [
            Extension('torri',
            sources=['torri.pyx'],
            libraries=['torri', 'vchiq_arm', 'bcm_host'],
            language='v',
            extra_compile_args=['-I./'],
            extra_link_args=[
                '-L./build/', '-Wl,-rpath,$ORIGIN/../../../',
                '-Wl,-rpath,/opt/vc/lib',
            ])
        ],
        compiler_directives={'language_level': '3'}
    ),
    data_files=[
        ('.', ['build/libtorri.so']),
    ]
)
