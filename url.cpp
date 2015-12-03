#include "url.hpp"

#include <boost/algorithm/string/split.hpp>
#include <deque>

namespace Rest
{
//#######################################################################################################
    std::vector <std::unique_ptr <PathPart>> Url::parsePath(bool disableIds) const
    {
        std::deque <std::string> splitted;
        boost::algorithm::split(splitted, path, [](char c) {return c == '/';});
        splitted.pop_front();

        std::vector <std::unique_ptr <PathPart>> splitPath;
        for (auto const& i : splitted) {
            if (!disableIds && i.front() == ':')
                splitPath.emplace_back(new PathParameter(i.substr(1, i.length() - 1)));
            else
                splitPath.emplace_back(new PathString(i));
        }
        return splitPath;
    }
//#######################################################################################################
    PathParameter::PathParameter(std::string id)
        : value_()
        , id_(id)
    {

    }
//-------------------------------------------------------------------------------------------------------
    std::string PathParameter::getValue() const
    {
        return value_;
    }
//-------------------------------------------------------------------------------------------------------
    std::string PathParameter::getId() const
    {
        return id_;
    }
//-------------------------------------------------------------------------------------------------------
    void PathParameter::setValue(std::string const& value)
    {
        value_ = value;
    }
//-------------------------------------------------------------------------------------------------------
    PathType PathParameter::getType() const
    {
        return PathType::PARAMETER;
    }
//#######################################################################################################
    PathString::PathString(std::string value)
        : value_(value)
    {

    }
//-------------------------------------------------------------------------------------------------------
    std::string PathString::getValue() const
    {
        return value_;
    }
//-------------------------------------------------------------------------------------------------------
    PathType PathString::getType() const
    {
        return PathType::STRING;
    }
//#######################################################################################################
}
