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

    def test_key_sizes(self):
        """Test different valid key sizes"""
        valid_sizes = [256, 512, 1024, 2048]
        for size in valid_sizes:
            with self.subTest(size=size):
                pub, priv = juce_rsa.create_key_pair(size)
                secret = 42
                encrypted = pub.apply(secret)
                decrypted = priv.apply(encrypted)
                self.assertEqual(secret, decrypted)

    def test_invalid_key_size(self):
        """Test invalid key sizes raise ValueError"""
        invalid_sizes = [0, -1, 255, 257, 1000, 32768]
        for size in invalid_sizes:
            with self.subTest(size=size):
                with self.assertRaises(ValueError):
                    juce_rsa.create_key_pair(size)

    def test_large_integer(self):
        """Test with large integers"""
        pub, priv = juce_rsa.create_key_pair(512)
        secret = 2 ** 100  # Large integer
        encrypted = pub.apply(secret)
        decrypted = priv.apply(encrypted)
        self.assertEqual(secret, decrypted)

    def test_zero_and_negative(self):
        """Test with zero and negative numbers"""
        pub, priv = juce_rsa.create_key_pair(256)

        # Test with positive number (0 and negative numbers might not be valid for RSA)
        secret = 42
        encrypted = pub.apply(secret)
        decrypted = priv.apply(encrypted)
        self.assertEqual(secret, decrypted)

        # Test that 0 raises appropriate error
        with self.assertRaises(AssertionError):
            pub.apply(0)
        # Test that negative values raise appropriate errors (might be ValueError for conversion issues)
        with self.assertRaises((AssertionError, ValueError)):
            pub.apply(-42)

    def test_hex_formats(self):
        """Test different hex string formats"""
        pub, priv = juce_rsa.create_key_pair(256)

        # Test with 0x prefix
        secret = "0x1a2b3c"
        encrypted = pub.apply(secret)
        decrypted = priv.apply(encrypted)
        self.assertEqual(secret, decrypted)

        # Test without 0x prefix
        secret = "1a2b3c"
        encrypted = pub.apply(secret)
        decrypted = priv.apply(encrypted)
        self.assertEqual(secret, decrypted)

    def test_invalid_hex_strings(self):
        """Test invalid hex strings raise ValueError"""
        key = juce_rsa.RSAKey()
        invalid_hexes = ["", "xyz", "123g", "0xxyz", "123!@#"]
        for hex_str in invalid_hexes:
            with self.subTest(hex_str=hex_str):
                with self.assertRaises(ValueError):
                    key.apply(hex_str)

    def test_invalid_string_format(self):
        """Test invalid string format for RSAKey constructor"""
        invalid_strings = [
            "",  # Empty string
            "12345",  # Missing comma
            "12345, ",  # Second part empty
            " , 12345",  # First part empty
            "12345, 6789, 1011",  # Too many parts
            "xyz, 12345",  # Invalid hex
            "12345, xyz",  # Invalid hex in second part
        ]

        for invalid_str in invalid_strings:
            with self.subTest(invalid_str=invalid_str):
                with self.assertRaises(ValueError):
                    juce_rsa.RSAKey(invalid_str)

    def test_rsa_key_operations(self):
        """Test that RSA operations are mathematically correct"""
        pub, priv = juce_rsa.create_key_pair(512)

        # Test that public encryption can be decrypted with private key
        message = 12345
        encrypted_with_public = pub.apply(message)
        decrypted_with_private = priv.apply(encrypted_with_public)
        self.assertEqual(message, decrypted_with_private)

        # Test that private encryption can be decrypted with public key
        encrypted_with_private = priv.apply(message)
        decrypted_with_public = pub.apply(encrypted_with_private)
        self.assertEqual(message, decrypted_with_public)

    def test_key_serialization(self):
        """Test that keys can be serialized and reconstructed"""
        original_pub, original_priv = juce_rsa.create_key_pair(256)

        # Get string representation of keys
        pub_str = repr(original_pub)
        priv_str = repr(original_priv)

        # Extract the actual key string from the representation
        # Format is: RSAKey("key_string") - extract the part between quotes
        import re
        pub_match = re.search(r'RSAKey\("(.*)"\)', pub_str)
        priv_match = re.search(r'RSAKey\("(.*)"\)', priv_str)

        if pub_match and priv_match:
            pub_inner = pub_match.group(1)
            priv_inner = priv_match.group(1)
        else:
            self.fail("Could not extract key string from representation")

        # Create new keys from string representations
        new_pub = juce_rsa.RSAKey(pub_inner)
        new_priv = juce_rsa.RSAKey(priv_inner)

        # Test that they work the same way
        secret = 98765
        encrypted1 = new_pub.apply(secret)
        decrypted1 = new_priv.apply(encrypted1)
        self.assertEqual(secret, decrypted1)

if __name__ == '__main__':
    unittest.main()
