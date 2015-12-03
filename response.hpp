#ifndef RESPONSE_HPP_INCLUDED
#define RESPONSE_HPP_INCLUDED

#include "forward.hpp"
#include "connection.hpp"
#include "response_header.hpp"

#include <string>

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
        void send(std::string const& message);

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

#endif // RESPONSE_HPP_INCLUDED
