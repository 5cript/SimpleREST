#include "fs_utility.hpp"

namespace Rest
{
//#####################################################################################################################
    std::string extractFileExtension(std::string const& fileName)
    {
        std::string extension;
        auto slpos = fileName.rfind("/");
        if (slpos == std::string::npos)
            slpos = fileName.rfind("\\");
        if (slpos != std::string::npos)
            extension = fileName.substr(slpos, extension.length() - slpos);
        else
            return "";

        auto dotpos = extension.find(".");
        if (dotpos == std::string::npos)
            return "";
        else
            return extension.substr(dotpos, extension.length() - dotpos);
    }
//#####################################################################################################################
}
