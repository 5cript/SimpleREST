#include "async_connection.hpp"
#include "response_code.hpp"
#include "server.hpp"
#include "mime.hpp"
#include "detail/fs_utility.hpp"

#include <fstream>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <utility>
#include <condition_variable>

#include <iostream>

namespace Rest
{
//#####################################################################################################################
    std::size_t findHeaderEnd(std::string const& head)
    {
        return head.find("\r\n\r\n");
    }
//#####################################################################################################################
    AsyncRestConnection::AsyncRestConnection(RestServer <AsyncRestConnection>* owner, UserId const& id)
        : owner_(owner)
        , id_(id)
        , socket_(*owner->getIoService())
        , endpoint_()
        , headerBuffer_()
        , bodyBeginning_()
        , readBuffer_()
        , request_()
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::free()
    {
        owner_->deregisterClient(this);
    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::setEndpoint(boost::asio::ip::tcp::acceptor::endpoint_type remote)
    {
        endpoint_ = remote;
    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::write(std::string const& str, std::function <void()> completionHandler)
    {
        write(str.c_str(), str.length(), completionHandler);
    }
//---------------------------------------------------------------------------------------------------------------------
    UserId AsyncRestConnection::getId() const
    {
        return id_;
    }
//---------------------------------------------------------------------------------------------------------------------
    RequestHeader AsyncRestConnection::getRequestHeader() const
    {
        return request_;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::size_t AsyncRestConnection::getBodySize() const
    {
        auto contentLen = request_.entries.find("Content-Length");
        if (contentLen == request_.entries.end())
            return std::atoll(contentLen->second.c_str());
        return 0;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool AsyncRestConnection::isBodyEmpty()
    {
        return getBodySize() == 0;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string AsyncRestConnection::getAddress() const
    {
        return endpoint_.address().to_string();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint32_t AsyncRestConnection::getPort() const
    {
        return endpoint_.port();
    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::sendFile(std::string const& fileName, std::function <void()> completionHandler, bool autoDetectContentType, ResponseHeader response)
    {
        std::ifstream reader(fileName, std::ios_base::binary);

        if (!reader.good())
            throw std::runtime_error("Could not open file.");

        reader.seekg(0, reader.end);
        auto size = reader.tellg();
        reader.seekg(0, reader.beg);

        if (autoDetectContentType) {
            auto extension = extractFileExtension(fileName);
            auto type = extensionToMimeType(extension);
            if (!type.empty())
                response["Content-Type"] = type;
        }
        response["Content-Length"] = std::to_string(size);

        if (size == 0)
        {
            response.responseCode = 204;
            response.responseString = "No Content";
        }
        write(response.toString(), [this, &]() {
            if (size == 0)
                return;

            char buffer[65536];
            do {
                reader.read(buffer, 65536);
                if (gcount() != 65536)
                    break;
                write(buffer, reader.gcount(), [](){});
            } while (reader.gcount() == 65536);

            write(buffer, reader.gcount(), completionHandler);
        });

    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::sendString(std::string const& text, std::function <void()> completionHandler, ResponseHeader response)
    {
        response.responseHeaderPairs["Content-Length"] = std::to_string(text.length());
        if (response.responseHeaderPairs.find("Content-Type") == std::end(response.responseHeaderPairs))
            response.responseHeaderPairs["Content-Type"] = "text/plain; charset=UTF-8";

        write(response.toString(), [this, &]() {
            if (response.responseCode != 204)
                write(text, completionHandler);
            });
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::sendHeader(ResponseHeader response, std::function <void()> completionHandler)
    {
        write(response.toString(), completionHandler);
    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::read(std::function <void(char const*, long)> writer, std::chrono::duration <long> const& timeout,
                                   std::function <void()> completionHandler, std::size_t totalBytesRead,
                                   std::chrono::time_point<std::chrono::system_clock> opStart)
    {
        if (getBodySize() == 0)
        {
            writer(nullptr, 0); // writer call is guaranteed, or should it?
            return;
        }

        if (totalBytesRead == 0)
        {
            if (!bodyBeginning_.empty())
                writer(bodyBeginning_.data(), bodyBeginning_.size());
        }

        auto self(shared_from_this()); // makes it impossible for this instance to die, before AIO is finished.

        std::size_t contentLength = getBodySize();
        socket_.async_read_some(boost::asio::buffer(readBuffer_),
            [&, this, self]
            (boost::system::error_code ec, std::size_t bytesTransferred)
            {
                if (!ec)
                {
                    writer(readBuffer_.data(), bytesTransferred);

                    if ((totalBytesRead + bytesTransferred + bodyBeginning_.size() < contentLength)
                        && (std::chrono::system_clock::now() - opStart) < timeout)
                        read(writer, timeout, completionHandler, totalBytesRead + bytesTransferred, opStart);
                }
                else
                {
                    completionHandler();
                }

                // if (ec == boost::asio::error::operation_aborted) nop();
            }
        );
    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::write(const char* buffer, std::size_t size, std::function <void()> completionHandler)
    {
        auto self(shared_from_this());

        std::shared_ptr <std::vector<char>> buf {
            new std::vector <char> (buffer, buffer + size)
        };
        boost::asio::async_write(socket_, boost::asio::buffer (*buf.get()),
            [this, self, buf, completionHandler](boost::system::error_code ec, std::size_t)
            {
                // buffer keeps alive, that is guaranteed.
                completionHandler();
            }
        );
    }
//---------------------------------------------------------------------------------------------------------------------
    void AsyncRestConnection::readHead(std::function <void(std::shared_ptr <AsyncRestConnection>)> cb)
    {
        auto self(shared_from_this()); // makes it impossible for this instance to die, before AIO is finished.

        socket_.async_read_some(boost::asio::buffer(readBuffer_),
            [this, self, cb](boost::system::error_code ec, std::size_t bytesTransferred)
            {
                if (ec && ec != boost::asio::error::operation_aborted)
                {
                    return;
                }

                headerBuffer_.append(readBuffer_.data(), bytesTransferred);
                auto pos = findHeaderEnd(headerBuffer_);
                if (pos == std::string::npos)
                {
                    if (headerBuffer_.length() >= 16 * 1024)
                    {
                        // head too large
                        ResponseHeader head;
                        head.responseCode = 431;
                        head.responseString = translateResponseCode(431);
                        sendHeader(head);
                    }
                    else
                        readHead(cb);
                }
                else
                {
                    bodyBeginning_ = std::vector <char> {
                        std::begin(readBuffer_) + pos,
                        std::begin(readBuffer_) + bytesTransferred
                    };

                    std::stringstream stream;
                    stream.write(headerBuffer_.substr(0, pos + 4).c_str(), pos + 4);
                    stream.seekg(0);

                    // read first line of request.
                    stream >> request_.requestType;
                    stream >> request_.url;
                    stream >> request_.httpVersion;

                    if (!stream || request_.httpVersion.substr(0, 5) != "HTTP/")
                    {
                        throw InvalidRequest("Request does not contain a valid HTTP request header.");
                    }

                    // validate http version
                    //------------------------------------------------------------
                    auto version = request_.httpVersion.substr(5, request_.httpVersion.length() - 5);

                    if (version != "1.0" && version != "1.1")
                        throw InvalidRequest("HTTP version is not supported");
                    //------------------------------------------------------------

                    // Process the response headers, which are terminated by a blank line.
                    std::string headerLine;
                    std::getline(stream, headerLine);
                    while (std::getline(stream, headerLine))
                    {
                        if (headerLine == "\r")
                            break;

                        auto pos = headerLine.find(":");
                        if (pos == std::string::npos)
                            throw InvalidRequest("HTTP header entry does not contain colon");

                        auto lhs = headerLine.substr(0, pos);
                        auto rhs = headerLine.substr(pos + 1, headerLine.length() - pos);

                        request_.entries[lhs] = rhs;
                    }
                }

                cb(self);
            }
        );
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string AsyncRestConnection::readString(std::function <void(std::string)> completionHandler, std::chrono::duration <long> const& timeout)
    {
        std::shared_ptr <std::string> result {new std::string};
        read([&](char const* buffer, long amount) { result->append(buffer, amount); }, [&](){
            completionHandler(*result);
        }, timeout);
    }
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}
