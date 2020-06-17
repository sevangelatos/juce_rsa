# Python 3 bindings for juce::RSAKey

## Installation
Tested in both Windows and Linux.
To use the package, simply install it using pip:

```sh
python3 -m pip install juce_rsa
```

## Usage
```python
import juce_rsa

# You can create a new key pair or instantiate a juce_rsa.RSAKey()
pub, priv = juce_rsa.create_key_pair(1024)

# Use the public key to encrypt a secret
secret=3
encrypted = pub.apply(secret)

# Use the private key to decrypt the secret
decrypted = priv.apply(encrypted)

print(encrypted, secret, decrypted)
assert(secret == decrypted)
```

You can also instantiate RSAKey objects by using the string produced by the juce::RSAKey::toString() method.

```python
key = juce_rsa.RSAKey("648929cb8e96db2df812d9d775ffaacb93e1d7261f54c12f3cbc824d774d91995d942c4b487b3dd895b5b33ff2fb2c8bc2b41c4e9e238e3519f417918c479c3d, 7dab743e723c91f97617904d537f957e78da4cefa729f17b0beba2e0d520f6011d8f0ddd3bb30d3c2486972fc46d8abd93826595711fd429b6f2efbde2762895")
```
