#include <chrono>
#include <algorithm>
#include <string>

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

#include <cassert>
#include <iostream>
#include "auth.hpp"

int main() {
    auth::session at_client( "joe@doe.com", "sesame", "@pc-workstation" );
    auth::session at_server( "joe@doe.com", "sesame", "@server" );

    // similar sessions, strictly equal once public_key is assigned
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
