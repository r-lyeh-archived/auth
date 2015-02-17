Auth <a href="https://travis-ci.org/r-lyeh/auth"><img src="https://api.travis-ci.org/r-lyeh/auth.svg?branch=master" align="right" /></a>
====

- Auth is a simple, lightweight and safe client-server authentication system. Written in C++11
- Auth features decoupled salt, encryption and network handshaking from implementation.
- Auth is tiny. Header-only.
- Auth is cross-platform.
- Auth is self-contained. No dependencies.
- Auth is BOOST licensed.

### Sample
```c++
int main() {
    auth::session at_client( "joe@doe.com", "sesame", "@pc-workstation" );
    auth::session at_server( "joe@doe.com", "sesame", "@server" );

    // similar sessions, not equal until public_key is assigned
    assert( at_client != at_server );
    at_client.set_public_key( at_server.get_public_key() );
    assert( at_client == at_server );

    // mutate passphrasses
    for( int i = 0; i < rand(); ++i ) {
        at_client.mutate(); assert( at_client != at_server );
        at_server.mutate(); assert( at_client == at_server );
    }

    // debug
    std::cout << at_client << std::endl;
    std::cout << at_server << std::endl;

    std::cout << "All ok." << std::endl;
}
```

### Possible output
```
[session:0034FDC0] {
.valid=1
.timestamp=873971735
.id=joe@doe.com;@pc-workstation
.user=joe@doe.com
.pass=3062624283
.public_key=554326941
.passphrase=4017519821
}

[session:0034FD40] {
.valid=1
.timestamp=873971735
.id=joe@doe.com;@server
.user=joe@doe.com
.pass=3062624283
.public_key=554326941
.passphrase=4017519821
}

All ok.
```

### API design
- You can compare sessions for equality and sort them, or insert them in a map.
- Sessions are not equal unless they have same `user` and `passphrase`.
- A passphrase is made of `pass` and `public_key`.
- A passphrase can mutate on both sides to change encryption on the fly.
- A server can hold different sessions that refer to the same user at the same time, ie when logging from different computers.
- Public keys can be sent thru insecure networks.

### API
- `void setup( string name, string pass, [string context], [string public_key] )` @todoc
- `void touch()` @todoc
- `bool is_timedout() const` @todoc
- `bool is_valid() const` @todoc
- `void invalidate()` @todoc
- `void reset()` @todoc
- `void mutate()` @todoc
- `void set_user_info( string name, string pass )` @todoc
- `string get_user_name() const` @todoc
- `string get_user_context() const` @todoc
- `string get_passphrase() const` @todoc
- `void set_public_key( string public_key )` @todoc
- `string get_public_key()` @todoc
- `size_t get_timestamp()`,  @todoc

### How-to: Building
- Implement namespace `auth::provider` somewhere both in client and server code.
- Check [provided sample](sample.cc) for a brief reference implementation.

### Complementary links
- https://github.com/r-lyeh/vault to handle ARC4 en/decryption.
- https://github.com/r-lyeh/cocoa to handle SHA1/CRC32 hashes.
- https://github.com/r-lyeh/sand to handle time and timestamps.

### Appendix: creating a strong solid login system
Check [related appendix](README-APPENDIX.md)
