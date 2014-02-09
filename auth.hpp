/*
 * Simple, lightweight and safe client-server authentication system.
 * Copyright (c) 2009-2014 Mario 'rlyeh' Rodriguez

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of Joachim Henke nor the names of his contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.

 * - rlyeh ~~ listening to Owl Oxidant / My name is William Blake
 */

#pragma once

#include <stdio.h>
#include <string>
#include <vector>

namespace auth
{
    namespace provider {
        std::string salt();
        std::string hash( const std::string &text );
        unsigned long long now();
        unsigned long long expire_time();
    };

    class session
    {
        bool valid = 0;
        size_t timestamp = 0;
        std::string user, pass, id, public_key, passphrase;

        public:

            session() {
                set_public_key( std::string() );
            }

            explicit
            session( const std::string &name, const std::string &pass, const std::string &context = std::string(), const std::string &public_key = std::string() ) {
                setup( name, pass, context, public_key );
            }

            void setup( const std::string &name, const std::string &pass, const std::string &context = std::string(), const std::string &public_key = std::string() ) {
                this->user = name;
                this->id = user + ';' + context;
                this->pass = provider::hash( pass );
                set_public_key( public_key.empty() ? provider::salt() : public_key );
            }

            void touch() {
                timestamp = provider::now();
            }

            bool is_timedout() const {
                return( provider::now() > timestamp + provider::expire_time() );
            }

            bool is_valid() const {
                return valid;
            }

            void invalidate() {
                valid = false;
            }

            void reset() {
                valid = true;
                passphrase = public_key;
                mutate();
            }

            void mutate() {
                touch();
                passphrase = provider::hash( passphrase + pass );
            }

            std::string get_user_id() const {
                return id;
            }
            std::string get_user_name() const {
                return user;
            }

            std::string get_passphrase() const {
                return passphrase;
            }
            std::string next_passphrase() {
                mutate();
                return passphrase;
            }

            void set_public_key( const std::string &public_key ) {
                // we *must* reset after setting a new public key.
                this->public_key = public_key;
                reset();
            }

            std::string get_public_key() {
                return public_key;
            }

            size_t get_timestamp() {
                return timestamp;
            }

            template<class T>
            friend inline T& operator <<( T &ostream, const session &self ) {
                return ostream <<
                    "[session:" << &self << "] {" << std::endl <<
                    ".valid=" << self.valid << std::endl <<
                    ".timestamp=" << self.timestamp << std::endl <<
                    ".id=" << self.id << std::endl <<
                    ".user=" << self.user << std::endl <<
                    ".pass=" << self.pass << std::endl <<
                    ".public_key=" << self.public_key << std::endl <<
                    ".passphrase=" << self.passphrase << std::endl <<
                    "}" << std::endl, ostream;
            }

            bool operator<( const session &other ) const {
                return id < other.id;
            }
            bool operator==( const session &other ) const {
                return user == other.user
                    && public_key == other.public_key
                    && passphrase == other.passphrase
                    && valid == other.valid;
            }
            bool operator!=( const session &other ) const {
                return !operator==( other );
            }
    };
}
