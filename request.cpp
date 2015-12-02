#include "request.hpp"

namespace Rest {
//#######################################################################################################
    Request::Request(std::shared_ptr <RestConnection>& connection,
                     std::map <std::string, std::string> parameters)
        : connection_(connection)
        , parameters_(parameters)
    {

    }
//-------------------------------------------------------------------------------------------------------
    std::string Request::getParameter(std::string const& id)
    {
        return parameters_[id];
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
//#######################################################################################################
}
