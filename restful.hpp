#ifndef RESTFUL_HPP_INCLUDED
#define RESTFUL_HPP_INCLUDED

#include "server.hpp"
#include "connection.hpp"

#include <functional>

namespace REST {
    /**
     *  The Interface Provider makes declaring a RESTful interface
     *  a breeze. It provides methods to register request types on certain
     *  urls.
     */
    class InterfaceProvider
    {
    public:
        // no copy
        InterfaceProvider& operator=(InterfaceProvider const&) = delete;
        InterfaceProvider(InterfaceProvider const&) = delete;

        /**
         *  Registers a new get request handler.
         *
         *  @param url The url to listen on. The syntax of is quite complex and documented elsewhere.
         *  @param callback
         *
         */
        InterfaceProvider& get(std::string const& url, std::function <> callback);

        /**
         *  Registers a new put request handler.
         */
        InterfaceProvider& put(std::string const& url, std::function <void(std::shared_ptr <RestConnection>)> callback);
        InterfaceProvider& post(std::string const& url, std::function <void(std::shared_ptr <RestConnection>)> callback);
        InterfaceProvider& delete(std::string const& url, std::function <void(std::shared_ptr <RestConnection>)> callback);
        InterfaceProvider& head(std::string const& url, std::function <void(std::shared_ptr <RestConnection>)> callback);
        InterfaceProvider& get(std::string const& url, std::function <void(std::shared_ptr <RestConnection>)> callback);
        InterfaceProvider& patch(std::string const& url, std::function <void(std::shared_ptr <RestConnection>)> callback);

        // Needs special handling: trace, options
        // Not supported: connect

    private:
        void connectionHandler(std::shared_ptr <RestConnection> connection);
        void errorHandler(std::shared_ptr <RestConnection> connection, InvalidRequest const& erroneousRequest);

    private:
        RestServer server_;
    };
}

#endif // RESTFUL_HPP_INCLUDED
