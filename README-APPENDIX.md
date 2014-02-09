
### How-to: User registration
- First-time users get registered into your app by entering their user and a safe gateway.
- Safe gateway may be an email, a phone mobile to deliver text messages, etc.
- Server stores password in a local storage device, and sends an activation/pass-changing link thru the gateway.
- From this point, we assume both network sides know about the pass (or hashed-pass) for a given user.

#### How-to: Network handshaking
- Client: sends user+auth request to server;
- Server: delays(time), configs a temporary session t(user,stored.pass), sends ack + t.public_key;
- Client: configs a session s(user, pass), and sets s.set_public_key() with received key;
- After this point, network handshaking is done. From now:
  - To encrypt/decrypt a network message, retrieve `get_passphrase()` contents, which should be equal in both client and server.
  - To mutate this very same passphrase, call `.mutate()`. This could be done at both sides after every succesful network negotiation.

#### How-to: Network communication flow
- Client: encrypts request with passphrase, sends user+enrypted-text.
- Server: receives user + encrypted-text, tries to decrypt text.
  - if ok, process answer, mutates session, crypt answer with passphrase.
  - if not ok, delay and close socket.
- Client: mutates session, decyphers text, process message.

### Full backend/frontend pseudo-code
```
namespace auth {
    namespace provider {
        unsigned long long now() {
            auto epoch = std::chrono::system_clock::now().time_since_epoch();
            return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
        }
        unsigned long long expire_time() {
            return 300000; //300s
        }
        std::string hash( const std::string &text ) {
            // This is just a sample. Since std::hash<>() is tied up to compiler-vendor
            // specifics and may change anytime, consider replacing this with a
            // SHA1/MD5/CRC32 hash here.
            return std::to_string( std::hash<std::string>()(text) );
        }
        std::string salt() {
            return hash( std::to_string( now() - rand() ) );
        }
    };
}

// @client handshaking
public_key = send( server, auth_req() );
session s( user, pass, public_key );

// @client flow
for( ;; ) {
    send(server, encrypt( request, s.get_passphrase() ));
    if( recv(server, answer) ) {
        // ok
        answer = decrypt( answer, s.next_passphrase() );
        process( answer );
    } else {
        // server closed connection or auth is wrong
        break;
    }
}

// @server handshaking
map< id, session > sessions;
user = recv( client, auth_req() );
delay();
session t = sessions[ ++id ];
t.setup(user,stored_passwords[user]);
send( client, session.get_public_key() )

// @server
for( ;; ) {
    auto text = recv( client );
    auto msg = decrypt( req.text, sessions[ req.user ].get_passphrase() );
    if( msg.ok ) {
        auto answer = process( msg );
        send( client, encrypt( answer, sessions[ req.user ].next_passphrase() ) );
    } else {
        delay(); // you could wipe all your timedout temporary sessions in here
        close( client );
        break;
    }
}
```
