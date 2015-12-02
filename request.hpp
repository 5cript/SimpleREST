#ifndef REQUEST_HPP_INCLUDED
#define REQUEST_HPP_INCLUDED

#include <string>

namespace RestApi {

    /**
     *  A holder for http requests.
     *  Containing the first line of the header.
     */
    struct Request
    {
        std::string requestType;
        std::string httpVersion;
        std::string url;
    };

} // namespace RestApi



#endif // REQUEST_HPP_INCLUDED
