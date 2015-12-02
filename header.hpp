#ifndef HEADER_HPP_INCLUDED
#define HEADER_HPP_INCLUDED

#include <unordered_map>

namespace Rest {

    /**
     *  A data holder class for header entries.
     */
    struct Header
    {
        // note it is unnecessarily complex to parse values and shove
        // values into actual named properties.
        // let the user take care of it and he can do what's needed.
        std::unordered_map <std::string, std::string> entries;
    };

} // namespace Rest



#endif // HEADER_HPP_INCLUDED
