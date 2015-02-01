from distutils.core import setup, Extension


mmh3module = Extension('mmh3', sources = ['mmh3module.cpp', 'murmur_hash_3.cpp'])

classifiers = [
    'Development Status :: 5 - Production/Stable',
    'Intended Audience :: Developers',
    'License :: Public Domain',
    'Programming Language :: Python :: 3',
    'Topic :: Utilities'
]

setup(name = 'murmurhash3',
    version = '2.3.5',
    description = 'a library for MurmurHash3, a set of fast and robust hash functions',
    license = 'Public Domain',
    author = 'V G',
    author_email = 'veegee@veegee.org',
    url = 'http://packages.python.org/murmurhash3',
    ext_modules = [mmh3module],
    keywords = ['hash', 'MurmurHash'],
    long_description = open('README.rst').read(),
    classifiers = classifiers
)
