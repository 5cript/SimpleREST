#include "url_parser.hpp"

#include <regex>
#include <stdexcept>

namespace Rest
{
//#######################################################################################################
    void ReducedUrlParser::parseQuery(std::string const& query, Url& url)
    {
        std::regex rgx( R"((\w+=(?:[\w-])+)(?:(?:&|;)(\w+=(?:[\w-])+))*)" );
        std::smatch match;

        if (std::regex_match(query, match, rgx))
        {
            for (auto i = std::begin(match) + 1; i < std::end(match); ++i)
            {
                auto pos = i->str().find_first_of('=');
                url.query[i->str().substr(pos+1)] = i->str().substr(0, pos);
            }
        }
    }
//-------------------------------------------------------------------------------------------------------
    Url ReducedUrlParser::parse(std::string const& urlString)
    {
        Url url;
        url.url = urlString;

        // regex for extracting path, query, fragment
        // (?:(?:(\/(?:(?:[a-zA-Z0-9]|[-_~!$&']|[()]|[*+,;=:@])+(?:\/(?:[a-zA-Z0-9]|[-_~!$&']|[()]|[*+,;=:@])+)*)?)|\/)?(?:(\?(?:\w+=(?:[\w-])+)(?:(?:&|;)(?:\w+=(?:[\w-])+))*))?(?:(#(?:\w|\d|=|\(|\)|\\|\/|:|,|&|\?)+))?)
        std::regex rgx( R"((?:(?:(\/(?:(?:[a-zA-Z0-9]|[-_~!$&']|[()]|[*+,;=:@])+(?:\/(?:[a-zA-Z0-9]|[-_~!$&']|[()]|[*+,;=:@])+)*)?)|\/)?(?:(\?(?:\w+=(?:[\w-])+)(?:(?:&|;)(?:\w+=(?:[\w-])+))*))?(?:(#(?:\w|\d|=|\(|\)|\\|\/|:|,|&|\?)+))?))" );
        std::smatch match;

        if (std::regex_match(urlString, match, rgx))
        {
            for (auto i = std::begin(match) + 1; i < std::end(match); ++i)
            {
                if (i->str().front() == '/')
                    url.path = i->str();
                else if (i->str().front() == '?')
                    ReducedUrlParser::parseQuery(i->str().substr(1, i->str().length() - 1), url);
                else if (i->str().front() == '#')
                    url.fragment = i->str().substr(1, i->str().length() - 1);
            }
        }
        else
            throw std::invalid_argument("Not a valid sub url");

        return url;
    }
//#######################################################################################################
}
