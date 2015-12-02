#include "response.hpp"

#include "response_code.hpp"

namespace Rest
{
//#######################################################################################################
    Response::Response(std::shared_ptr <RestConnection>& connection)
        : connection_(connection)
    {

    }
//-------------------------------------------------------------------------------------------------------
    void Response::send(std::string const& message)
    {
        connection_->sendString(message, header_);
    }
//-------------------------------------------------------------------------------------------------------
    RestConnection& Response::getConnection()
    {
        return *connection_;
    }
//-------------------------------------------------------------------------------------------------------
    void Response::sendFile(std::string const& fileName, bool autoDetectContentType)
    {
        connection_->sendFile(fileName, autoDetectContentType, header_);
    }
//-------------------------------------------------------------------------------------------------------
    void Response::setHeaderEntry(std::string key, std::string value)
    {
        header_[key] = value;
    }
//-------------------------------------------------------------------------------------------------------
    void Response::sendStatus(int code)
    {
        status(code).send(header_.responseString);
    }
//-------------------------------------------------------------------------------------------------------
    Response& Response::status(int code)
    {
        header_.responseString = translateResponseCode(code);
        header_.responseCode = code;
        return *this;
    }
//#######################################################################################################
}
