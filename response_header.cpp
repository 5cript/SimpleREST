#include "response_header.hpp"

#include <sstream>

namespace Rest
{
//#######################################################################################################
    std::string ResponseHeader::toString() const
    {
        std::stringstream builder;
        builder << httpVersion << " " << responseCode << " " << responseString << "\r\n";
        for (auto const& i : responseHeaderPairs) {
            builder << i.first << ": " << i.second << "\r\n";
        }
        builder << "\r\n";

        return builder.str();
    }
//-------------------------------------------------------------------------------------------------------
    bool ResponseHeader::isSet(std::string const& key)
    {
        return responseHeaderPairs.find(key) != std::end(responseHeaderPairs);
    }
//-------------------------------------------------------------------------------------------------------
    std::string& ResponseHeader::operator[](std::string const& key)
    {
        return responseHeaderPairs[key];
    }
//#######################################################################################################
} // namespace Rest


