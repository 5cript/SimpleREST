#include "request.hpp"

namespace Rest {
//#######################################################################################################
    Request::Request(std::shared_ptr <RestConnection>& connection,
                     std::unordered_map <std::string, std::string> parameters,
                     Url url)
        : connection_(connection)
        , parameters_(std::move(parameters))
        , url_(std::move(url))
    {

    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getUrl() const
    {
        return connection_->getRequestHeader().url;
    }
//-------------------------------------------------------------------------------------------------------
    std::unordered_map <std::string, std::string> Request::getQuery() const
    {
        return url_.query;
    }
//-------------------------------------------------------------------------------------------------------
    bool Request::isSecure() const
    {
        return false;
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getPath() const
    {
        return url_.path;
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getParameter(std::string const& id)
    {
        return parameters_[id];
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::param(std::string const& id)
    {
        return getParameter(id);
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getRemoteAddress() const
    {
        return connection_->getAddress();
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::ip() const
    {
        return getRemoteAddress();
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getHeaderField(std::string const& key)
    {
        // it does not hurt to add it if nonexistant, because it will be empty
        return connection_->getRequestHeader().entries[key];
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getString()
    {
        return connection_->readString();
    }
//-------------------------------------------------------------------------------------------------------
    std::ostream& Request::getStream(std::ostream& stream)
    {
        return connection_->readStream(stream);
    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getType()
    {
        return connection_->getRequestHeader().requestType;
    }
//#######################################################################################################
}
