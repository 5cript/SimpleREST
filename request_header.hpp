#pragma once

#include <string>
#include <unordered_map>

namespace Rest {

    /**
     *  A holder for http requests.
     *  Containing the first line of the header.
     */
    struct RequestHeader
    {
        std::string requestType;
        std::string httpVersion;
        std::string url;

        std::unordered_map <std::string, std::string> entries;
    };

} // namespace Rest
