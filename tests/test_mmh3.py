import mmh3


class Test64Bit:
    def test_hash32(self):
        h = mmh3.hash('foo')
        assert h == -156908512

    def test_hash32_seed(self):
        h = mmh3.hash('foo', 42)
        assert h == -1322301282

    def test_hash64(self):
        h1, h2 = mmh3.hash64('hello')
        assert (h1, h2) == (-3758069500696749310, 6565844092913065241)

    def test_hash_bytes(self):
        h = mmh3.hash_bytes('foo')
        assert h == b'aE\xf5\x01W\x86q\xe2\x87}\xba+\xe4\x87\xaf~'
