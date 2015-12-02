#ifndef REQUEST_HPP_INCLUDED
#define REQUEST_HPP_INCLUDED

#include <string>

namespace REST {

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

} // namespace REST



#endif // REQUEST_HPP_INCLUDED
