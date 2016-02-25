#pragma once

#include <string>

namespace Rest {

    // source: http://www.freeformatter.com/mime-types-list.html
    /**
     *  Converts a file extension to a mime type if known.
     *  @param extension A file extension including dot in front.
     *
     *  @return a MIME type.
     */
    std::string extensionToMimeType(std::string const& extension);

} // namespace Rest
