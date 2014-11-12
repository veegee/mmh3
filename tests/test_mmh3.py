import mmh3


class Test64Bit:
    def test_hash64(self):
        s = 'hello'
        h1, h2 = mmh3.hash64(s)
        assert (h1, h2) == (-3758069500696749310, 6565844092913065241)
