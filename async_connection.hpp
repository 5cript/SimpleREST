#pragma once

#include "user_id.hpp"
#include "forward.hpp"
#include "header.hpp"
#include "exceptions.hpp"
#include "response_header.hpp"
#include "request_header.hpp"

#ifndef Q_MOC_RUN // A Qt workaround, for those of you who use Qt
#   include "SimpleJSON/parse/jsd.h"
#   include "SimpleJSON/parse/jsd_convenience.h"
#   include "SimpleJSON/stringify/jss.h"
#   include "SimpleJSON/stringify/jss_fusion_adapted_struct.h"
#endif

#include <boost/asio.hpp>

#include <string>
#include <sstream>
#include <memory>
#include <cmath>
#include <chrono>
#include <functional>
#include <array>
#include <vector>

namespace Rest {

    using namespace std::literals;

    /**
     *  An asynchronous Rest connection to a client.
     */
    class AsyncRestConnection : public std::enable_shared_from_this <AsyncRestConnection>
    {
        friend RestServer <AsyncRestConnection>;

    public:
        ~AsyncRestConnection() = default;

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
        void sendJson(T const& object, std::function <void()> completionHandler, ResponseHeader response = {})
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

            write(response.toString(), [this, b{std::move(body)}, completionHandler](){
                write(b.str(), completionHandler);
            });
        }

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
        void sendFile(std::string const& fileName, std::function <void()> completionHandler, bool autoDetectContentType = true, ResponseHeader response = {});

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
        void sendString(std::string const& text, ResponseHeader response, std::function <void()> completionHandler);

        /**
         *  Sends only the header and an empty body.
         *
         *  @param response The header information to send.
         */
        void sendHeader(ResponseHeader response, std::function <void()> completionHandler);

        /**
         *  Reads the body as a text string.
         *  Please be aware the reading the stream content reads it all.
         *  We would not recommend to mix data in a single request, use multiple
         *  request or the convenience of JSON.
         *
         *  @return The body.
         */
        void readString(std::function <void(std::st)> completionHandler, std::chrono::duration <long> const& timeout = 3s);

        /**
         *  Reads the socket content to a stream.
         *  This is useful to save the body to a file.
         *  Please be aware the reading the stream content reads it all.
         *  We would not recommend to mix data in a single request, use multiple
         *  request or the convenience of JSON.
         *
         *  @return The passed stream
         */
        std::ostream& readStream(std::ostream& stream, std::function <void()> completionHandler, std::chrono::duration <long> const& timeout = 3s);

        /**
         *  Reads the body and tries to parse it as JSON.
         *  Please be aware the reading the stream content reads it all.
         *  We would not recommend to mix data in a single request.
         *
         *  @param object Writes the JSON into this object.
         */
        template <typename T>
        void readJson(std::function <void(T const&)> completionHandler, std::chrono::duration <long> const& timeout = 3s)
        {
            readString(timeout);
            auto json = "{content:" +  + "}";
            auto tree = JSON::parse_json(json);
            JSON::parse(object, "content", tree);
        }

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
        AsyncRestConnection(RestServer <AsyncRestConnection>* owner, UserId const& id);

        /**
         *  Reads and parses the head.
         */
        void readHead(std::function <void(std::shared_ptr <AsyncRestConnection>)> cb);

        /**
         *  Closes the connection and removes it from the server list.
         */
        void free();

        /**
         *  Pass this to async_accept
         */
        auto& getAcceptHandle() {
            return socket_;
        }

        /**
         *  Sets the remote endpoint for access.
         */
        void setEndpoint(boost::asio::ip::tcp::acceptor::endpoint_type remote);

        /**
         *  Internal function that reduces code duplication
         */
        void read(std::function <void(char const*, long)> writer, std::chrono::duration <long> const& timeout,
                  std::function <void()> completionHandler, std::size_t totalBytesRead = 0,
                  std::chrono::time_point<std::chrono::system_clock> opStart = std::chrono::system_clock::now());

        /**
         *  Internal function to write data to the socket.
         */
        void write(const char* buffer, std::size_t size, std::function <void()> completionHandler);

        /**
         *  Internal function to write data to the socket.
         */
        void write(std::string const& str, std::function <void()> completionHandler);

    private:
        RestServer <AsyncRestConnection>* owner_;
        UserId id_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::ip::tcp::acceptor::endpoint_type endpoint_;
        std::string headerBuffer_;
        std::vector <char> bodyBeginning_;
        std::array <char, 8192> readBuffer_;

        RequestHeader request_;
    };
} // namespace Rest
