#pragma once

#include "user_id.hpp"
#include "forward.hpp"
#include "header.hpp"
#include "exceptions.hpp"
#include "response_header.hpp"
#include "request_header.hpp"

#ifndef Q_MOC_RUN // A Qt workaround, for those of you who use Qt
#   ifdef SREST_SUPPORT_JSON
#       include "SimpleJSON/parse/jsd.hpp"
#       include "SimpleJSON/parse/jsd_convenience.hpp"
#       include "SimpleJSON/stringify/jss.hpp"
#       include "SimpleJSON/stringify/jss_fusion_adapted_struct.hpp"
#   endif
#
#	ifdef SREST_SUPPORT_XML
#   	include "SimpleXML/xmlify/xmlify.hpp"
#	endif // SREST_SUPPORT_XML
#endif



#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <cmath>
#include <chrono>
#include <functional>

namespace Rest {

    using namespace std::literals;

    /**
     *  A Rest connection to a client.
     */
    class RestConnection : public std::enable_shared_from_this <RestConnection>
    {
        friend RestServer;

    public:
        ~RestConnection() = default;

        /**
         *  Returns the socket iostream for writing.
         *  Do not read the header from it.
         *  It is recommended to call flush after writing everything to the stream.
         *
         *  @return A tcp::iostream. See boost asio documentation.
         */
        boost::asio::ip::tcp::iostream& getStream();

        /**
         *  Returns the connected clients id.
         *
         *  @return Returns the user id.
         */
        UserId getId() const;

        /**
         *  Returns the request header
         *
         *  @return A header containing the key:value pairs.
         */
        RequestHeader getRequestHeader() const;

        /**
         *  Returns the size of the body.
         *
         *  @return body size.
         */
        std::size_t getBodySize() const;

        /**
         *  Returns the remote address.
         *
         *  @return Remote peer address as string.
         */
        std::string getAddress() const;

        /**
         *  Returns the remote endpoints port.
         *
         *  @return Remote peer port.
         */
        uint32_t getPort() const;

#ifdef SREST_SUPPORT_JSON
        /**
         *  Send JSON response. uses SimpleJSON library to stringify the object.
         *  Automatically sets the following header key/value pairs
         *
         *  Content-Type: text/json; charset=UTF-8
         *  Content-Length: ...
         *  Connection: close
         *
         *  @param object An object to stringify.
         *  @param responseHeader A response header containing header information,
         *         such as response code, version and response message.
         */
        template <typename T>
        void sendJson(T const& object, ResponseHeader response = {})
        {
            using namespace std::literals;

            response.responseHeaderPairs["Content-Type"s] = "text/json; charset=UTF-8"s;

            std::stringstream body;
            body << '{';
            JSON::try_stringify(body, "", object);
            body << '}';

            auto bodySize = body.tellp();
            response.responseHeaderPairs["Content-Length"s] = std::to_string(bodySize);
            response.responseHeaderPairs["Connection"s] = "close"s;

            body.seekg(0);

            stream_ << response.toString();
            stream_ << body.rdbuf();
            stream_.flush();
        }
#endif // SREST_SUPPORT_JSON

#ifdef SREST_SUPPORT_XML
        /**
         *  Send XML response. uses SimpleXML library
         *  Automatically sets the following header key/value pairs
         *
         *  Content-Type: text/xml; charset=UTF-8
         *  Content-Length: ...
         *  Connection: close
         *
         *  @param object An object to xmlify.
         *  @param name The name of the root xml node, as such is required.
         *  @param responseHeader A response header containing header information,
         *         such as response code, version and response message.
         */
        template <typename T>
        void sendXml(T const& object, std::string const& name = "body", ResponseHeader response = {})
        {
            using namespace std::literals;

            response.responseHeaderPairs["Content-Type"s] = "text/xml; charset=UTF-8"s;

            std::stringstream body;
            body << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
            SXML::xmlify(body, name, object);

            auto bodySize = body.tellp();
            response.responseHeaderPairs["Content-Length"s] = std::to_string(bodySize);
            response.responseHeaderPairs["Connection"s] = "close"s;

            body.seekg(0);

            stream_ << response.toString();
            stream_ << body.rdbuf();
            stream_.flush();
        }
#endif // SREST_SUPPORT_XML

        /**
         *  Sends a file as content.
         *  The response will be 204 for empty files and whats provided otherwise,
         *  which might be 200 if no explicit response header is provided.
         *  It does not send error codes on itself, but throws when the file cannot be opened.
         *
         *  Automatically sets the following header key/value pairs
         *
         *  Content-Length: ...
         *
         *  @param fileName A file to send.
         *  @param responseHeader A response header containing header information,
         *         such as response code, version and response message.
         */
        void sendFile(std::string const& fileName, bool autoDetectContentType = true, ResponseHeader response = {});

        /**
         *  Sends a string. You must set the content type yourself on the response parameter.
         *  Therefore the parameter is not optional.
         *  The response will be 204 for empty strings and whats provided otherwise,
         *  which might be 200 if no explicit response header is provided.
         *
         *  Automatically sets the following header key/value pairs
         *
         *  Content-Length: text.length()
         *
         *  @param text A text to send.
         *  @param response A response header containing header information,
         *         such as response code, version and response message.
         */
        void sendString(std::string const& text, ResponseHeader response);

        /**
         *  Sends only the header and an empty body.
         *
         *  @param response The header information to send.
         */
        void sendHeader(ResponseHeader response);

        /**
         *  Reads the body as a text string.
         *  Please be aware the reading the stream content reads it all.
         *  We would not recommend to mix data in a single request, use multiple
         *  request or the convenience of JSON.
         *
         *  @return The body.
         */
        std::string readString(std::chrono::duration <long> const& timeout = 3s);

        /**
         *  Reads the socket content to a stream.
         *  This is useful to save the body to a file.
         *  Please be aware the reading the stream content reads it all.
         *  We would not recommend to mix data in a single request, use multiple
         *  request or the convenience of JSON.
         *
         *  @return The passed stream
         */
        std::ostream& readStream(std::ostream& stream, std::chrono::duration <long> const& timeout = 3s);

#ifdef SREST_SUPPORT_JSON
        /**
         *  Reads the body and tries to parse it as JSON.
         *  Please be aware the reading the stream content reads it all.
         *  We would not recommend to mix data in a single request.
         *
         *  @param object Writes the JSON into this object.
         */
        template <typename T>
        void readJson(T& object, std::chrono::duration <long> const& timeout = 3s)
        {
            auto json = "{\"content\":" + readString(timeout) + "}";
            auto tree = JSON::parse_json(json);
            JSON::parse(object, "content", tree);
        }
#endif

        /**
         *  Returns whether or not the body contains any data.
         *
         *  @return bodySize == 0
         */
        bool isBodyEmpty();

    private:
        /**
         *  Connections can only be created by a server.
         *  Users shall never create a connection on their own,
         *  this makes no sense.
         */
        RestConnection(RestServer* owner, UserId const& id);

        /**
         *  Reads and parses the head.
         */
        void readHead();

        /**
         *  Closes the connection and removes it from the server list.
         */
        void free();

        /**
         *  Sets the remote endpoint for access.
         */
        void setEndpoint(boost::asio::ip::tcp::acceptor::endpoint_type remote);

        /**
         *  Internal function that reduces code duplication
         */
        void read(std::function <void(char const*, long)> writer, std::chrono::duration <long> const& timeout);

    private:
        RestServer* owner_;
        UserId id_;
        boost::asio::ip::tcp::iostream stream_;
        boost::asio::ip::tcp::acceptor::endpoint_type endpoint_;

        RequestHeader request_;
    };

} // namespace Rest

