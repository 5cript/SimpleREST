#include "response.hpp"

#include "response_code.hpp"

namespace Rest
{
//#######################################################################################################
    Response::Response(std::shared_ptr <RestConnection>& connection)
        : connection_(connection)
        , header_()
        , statusSet_(false)
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
    Response& Response::setHeaderEntry(std::string key, std::string value)
    {
        header_[key] = value;
        return *this;
    }
//-------------------------------------------------------------------------------------------------------
    void Response::end()
    {

    }
//-------------------------------------------------------------------------------------------------------
    void Response::redirect(std::string const& path)
    {
        setHeaderEntry("Location", path);
        if (!statusSet_)
            status(302);
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
        statusSet_ = true;
        return *this;
    }
//#######################################################################################################
}
