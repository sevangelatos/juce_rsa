#!/usr/bin/env python3
import unittest
import juce_rsa

class TestJuceRsaMethods(unittest.TestCase):

    def test_instantiate(self):
        key1 = juce_rsa.RSAKey()
        key2 = juce_rsa.RSAKey("deadbeef, 12345")
        key3 = repr(key2)

    def test_repr(self):
        key2 = juce_rsa.RSAKey("deadbeef, 12345")
        self.assertTrue(repr(key2) == 'RSAKey("deadbeef, 12345")')

    def test_pair_integral(self):
        pub, priv = juce_rsa.create_key_pair(256)
        secret = 1337
        encrypted = pub.apply(secret)
        self.assertFalse(encrypted == secret)
        decoded = priv.apply(encrypted)
        self.assertTrue(decoded == secret)

    def test_pair_hex(self):
        pub, priv = juce_rsa.create_key_pair(256)
        secret = hex(1337)
        encrypted = pub.apply(secret)
        self.assertFalse(encrypted == secret)
        decoded = priv.apply(encrypted)
        self.assertTrue(decoded == secret)

if __name__ == '__main__':
    unittest.main()
