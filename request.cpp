#include "request.hpp"

namespace Rest {
//#######################################################################################################
    Request::Request(std::shared_ptr <RestConnection>& connection,
                     std::unordered_map <std::string, std::string> parameters)
        : connection_(connection)
        , parameters_(std::move(parameters))
    {

    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getParameter(std::string const& id)
    {
        return parameters_[id];
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