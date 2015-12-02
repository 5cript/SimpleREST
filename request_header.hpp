#ifndef REQUEST_HPP_INCLUDED
#define REQUEST_HPP_INCLUDED

#include <string>

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
    };

} // namespace Rest



#endif // REQUEST_HPP_INCLUDED
