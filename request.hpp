#pragma once

#include "forward.hpp"
#include "connection.hpp"
#include "request_header.hpp"
#include "url.hpp"

#include <string>
#include <unordered_map>

namespace Rest {
    class Request
    {
        friend InterfaceProvider;

    public:
        /**
         *  Gets an URL parameter from its id.
         *
         *  @param id One of the specified ids for the API call.
         *
         *  @return The value behind the id.
         */
        std::string getParameter(std::string const& id);

        /**
         *  Shorthand for getParameter.
         *  @see getParameter
         */
        std::string param(std::string const& id);

        /**
         *  Returns the request type. Which is get, put, post, ...
         */
        std::string getType();

        /**
         *  Parses the body as JSON and returns the fresh object.
         *
         *  @return The parsed object.
         */
        template <typename T>
        T getJson()
        {
            T t;
            connection_->readJson(t);
            return t;
        }

        /**
         *  Parses the body as JSON and stores it in the parameter.
         *
         *  @param obj A reference to an object to store the results in.
         */
        template <typename T>
        void getJson(T& obj)
        {
            connection_->readJson(obj);
        }

        /**
         *  Returns the body as a string.
         *
         *  @return Returns the body as a string.
         */
        std::string getString();

        /**
         *  Writes the body into a stream.
         *
         *  @param stream The stream to put the body into.
         *
         *  @return The body as a stream.
         */
        std::ostream& getStream(std::ostream& stream);

        /**
         *  Gets the remote address.
         *
         *  @return Remote peer ip address.
         */
        std::string getRemoteAddress() const;

        /**
         *  Shorthand for getRemoteAddress.
         *  @see getRemoteAddress.
         */
        std::string ip() const;

        /**
         *  Gets the request url sent by remote.
         *
         *  @return The unmodified remote url.
         */
        std::string getUrl() const;

        /**
         *  Returns the path part of the url.
         *  "/users?sort=desc" => "/users"
         *
         *  @return Returns the path part of the url.
         */
        std::string getPath() const;

        /**
         *  Returns whether or not the client connected over a secure https connection.
         *  As https is not supported yet, it will always return false.
         *
         *  @return false.
         */
        bool isSecure() const;

        /**
         *  Returns the query parameters.
         *  "/users?sort=desc" => "sort = desc"
         *
         *  Only supports key=value pairs, the key must be unique
         *
         *  @return An assoicative container for the key value pairs.
         */
        std::unordered_map <std::string, std::string> getQuery() const;

        /**
         *  Gets a field from the request header.
         *
         *  @param key The header entry key.
         *
         *  @return The corresponding value to the key. Will return an empty string if it was not specified.
         */
        std::string getHeaderField(std::string const& key);

    private:
        // cannot be created by user.
        Request(std::shared_ptr <RestConnection>& connection,
                std::unordered_map <std::string, std::string> parameters,
                Url url);

    private:
        std::shared_ptr <RestConnection> connection_;
        std::unordered_map <std::string, std::string> parameters_;
        Url url_;
    };
} // namespace Rest
