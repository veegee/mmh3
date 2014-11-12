from distutils.core import setup, Extension

mmh3module = Extension('mmh3', sources = ['mmh3module.cpp', 'murmur_hash_3.cpp'])

setup(name = 'mmh3',
    version = '2.3',
    description = 'Python library for MurmurHash (MurmurHash3), a set of fast and robust hash functions.',
    license = 'Public Domain',
    author = 'V G',
    author_email = 'hajime.senuma@gmail.com',
    url = 'http://packages.python.org/mmh3',
    ext_modules = [mmh3module],
    keywords = "utility hash MurmurHash",
    long_description = open('README.rst').read(),
    classifiers = ['Development Status :: 5 - Production/Stable',
    'Intended Audience :: Developers',
    'License :: Public Domain',
    'Programming Language :: Python :: 2.7',
    'Programming Language :: Python :: 3',
    'Topic :: Utilities']
)
