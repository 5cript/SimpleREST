#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Rest
{
    class PathPart;

    /**
     *  Url holder struct.
     */
    struct Url
    {
        // not parseable yet
        std::string scheme;
        std::string user;
        std::string password;
        std::string host;
        uint32_t port;

        // read by reduced url parser
        std::string url;
        std::string fragment;
        std::string path;
        std::unordered_map <std::string, std::string> query;

        std::vector <std::unique_ptr <PathPart>> parsePath(bool disableIds = false) const;
    };

    enum class PathType
    {
        PARAMETER,
        STRING
    };

    class PathPart
    {
    public:
        virtual std::string getValue() const = 0;
        virtual PathType getType() const = 0;

        virtual ~PathPart() = default;
    };

    class PathParameter : public PathPart
    {
    public:
        PathParameter(std::string id);

        std::string getValue() const override;
        PathType getType() const override;
        std::string getId() const;
        void setValue(std::string const& value);

    private:
        std::string value_;
        std::string id_;
    };

    class PathString : public PathPart
    {
    public:
        PathString(std::string value);

        std::string getValue() const override;
        PathType getType() const override;

    private:
        std::string value_;
    };


}
