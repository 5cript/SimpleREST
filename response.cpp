#include "response.hpp"

#include "response_code.hpp"
#include "mime.hpp"

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
    Response& Response::type(std::string const& type)
    {
        auto mime = extensionToMimeType(std::string(".") + type);
        if (mime.empty())
            mime = type;
        header_.responseHeaderPairs["Content-Type"] = type;
        return *this;
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
        send("");
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
