#ifndef URL_PARSER_HPP_INCLUDED
#define URL_PARSER_HPP_INCLUDED

#include "url.hpp"

#include <string>
#include <unordered_map>
#include <cstdint>

namespace Rest
{
    /**
     *  This parser starts with /path ... and not with the scheme.
     *  Because our Rest InterfaceProvider needs it that way.
     */
    class ReducedUrlParser
    {
    public:
        /**
         *  parses a urlString to an url object.
         */
        static Url parse(std::string const& urlString);

    private:
        static void parseQuery(std::string const& query, Url& url);
    };
}

#endif // URL_PARSER_HPP_INCLUDED
