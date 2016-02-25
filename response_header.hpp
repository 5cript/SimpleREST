#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace Rest {

    /**
     *  A pure holder for response information.
     */
    struct ResponseHeader
    {
        std::string httpVersion = "HTTP/1.1";
        uint16_t responseCode = 200;
        std::string responseString = "OK";
        std::unordered_map <std::string, std::string> responseHeaderPairs;

        /**
         *  Turns the response header into a conforming header.
         *  Can be prepended to the body.
         *
         *  @return A string representation of this.
         */
        std::string toString() const;

        /**
         *  Shorthand operator for header key value pairs.
         */
        std::string& operator[](std::string const& key);

        /**
         *  Checks whether a certain key value pair exists.
         *  Mostly used internally.
         */
        bool isSet(std::string const& key);
    };

} // namespace Rest
