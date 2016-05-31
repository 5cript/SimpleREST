#pragma once

#include "forward.hpp"
#include "connection.hpp"
#include "response_header.hpp"

#include <string>
#include <memory>

namespace Rest {
    class Response
    {
        friend InterfaceProvider;

    public:
        /**
         *  Sends a string back to the client.
         *
         *  @param message The string to send.
         */
        void send(std::string const& message = "");

#ifdef SREST_SUPPORT_JSON
        /**
         *  Stringifies an object and sends it back to the client.
         *
         *  @param obj The object to stringify and send.
         */
        template <typename T>
        void json(T const& obj)
        {
            connection_->sendJson(obj, header_);
        }

        /**
         *  Alias for json.
         *  @see json
         */
        template <typename T>
        void sendJson(T const& obj)
        {
            json(obj, header_);
        }
#endif // SREST_SUPPORT_JSON

#ifdef SREST_SUPPORT_XML
        /**
         *  Xmlifies an object and sends it back to the client.
         *
         *  @param obj The object to xmlify and send.
         *  @param rootName The name of the root xml node.
         */
        template <typename T>
        void xml(T const& obj, std::string const& rootName = "body")
        {
            connection_->sendXml(obj, rootName, header_);
        }

        /**
         *  Alias for xml
         *  @see xml
         */
         template <typename T>
         void sendXml(T const& obj, std::string const& rootName = "body")
         {
             xml(obj, rootName, header_);
         }
#endif // SREST_SUPPORT_XML

        /**
         *  Sends a file back to the client.
         *
         *  @param fileName A file to send.
         *  @param responseHeader A response header containing header information,
         *         such as response code, version and response message.
         */
        void sendFile(std::string const& fileName, bool autoDetectContentType = true);

        /**
         *  Sends a status code with the string representation as body.
         *  Equivalent to status(code).send(...)
         *
         *  status(403).send("Forbidden")
         *
         *  @param code A standard HTTP response code.
         */
        void sendStatus(int code);

        /**
         *  Used for empty bodies. This is actually a 'no operation' function,
         *  because things a finished, when your handler returns.
         *  But it might transform send functions into nop's the future.
         */
        void end();

        /**
         *  Sets the status code for the next send.
         *  Please not that the code defaults to 200 or 204 if not specified!
         *  This method is intended to be chained!
         *
         *  @param code A standard HTTP response code.
         *
         *  @return itself.
         */
        Response& status(int code);

        /**
         *  Sets a header key value pair. Adds it if it were not existing.
         *  Does not perform checkings. Make sure to do it correctly.
         *
         *  @param key Key of header entry.
         *  @param value Value of header entry.
         *
         *  @return itself.
         */
        Response& setHeaderEntry(std::string key, std::string value);

        /**
         *  Redirects to another url / page.
         *  Sets the Location header entry for that. The status code will default
         *  to 302. Another code you should look into is 301.
         *
         *  @param path The path to redirect to. See HTTP Location header field.
         */
        void redirect(std::string const& path);

        /**
         *  Sets the content type. such as "application/json".
         *  if the passed type is a file extension (without dot), it will choose the correct type.
         *  Check mime.cpp for a list of known extensions.
         *
         *  @param type custom Content-Type or file extension.
         */
        Response& type(std::string const& type);

        /**
         *  Returns the connection to the client.
         *  Can be useful to access more delicate and low level things.
         *  Please do not abuse!
         *
         *  @return Returns a RestConnection reference
         */
        RestConnection& getConnection();

    private:
        // cannot be created by user.
        Response(std::shared_ptr <RestConnection>& connection);

    private:
        std::shared_ptr <RestConnection> connection_;
        ResponseHeader header_;
        bool statusSet_;
    };
}
