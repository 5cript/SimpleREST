#ifndef RestFUL_HPP_INCLUDED
#define RestFUL_HPP_INCLUDED

#include "server.hpp"
#include "connection.hpp"
#include "request.hpp"
#include "response.hpp"

#include <functional>

namespace Rest {
    /**
     *  The Interface Provider makes declaring a Restful interface
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
         *  @param callback The function called when a client sends a request on the url.
         *
         */
        InterfaceProvider& get(std::string const& url, std::function <void(Request&, Response&)> callback);

        /**
         *  Registers a new put request handler.
         *
         *  @param url The url to listen on. The syntax of is quite complex and documented elsewhere.
         *  @param callback The function called when a client sends a request on the url.
         *
         */
        InterfaceProvider& put(std::string const& url, std::function <void(Request&, Response&)> callback);

        /**
         *  Registers a new post request handler.
         *
         *  @param url The url to listen on. The syntax of is quite complex and documented elsewhere.
         *  @param callback The function called when a client sends a request on the url.
         *
         */
        InterfaceProvider& post(std::string const& url, std::function <void(Request&, Response&)> callback);

        /**
         *  Registers a new delete request handler.
         *  This functions is special, because the name is not standard.
         *  We cannot use the delete keyword and do not want to use an underscore.
         *
         *  @param url The url to listen on. The syntax of is quite complex and documented elsewhere.
         *  @param callback The function called when a client sends a request on the url.
         *
         */
        InterfaceProvider& remove(std::string const& url, std::function <void(Request&, Response&)> callback);

        /**
         *  Registers a new head request handler.
         *
         *  @param url The url to listen on. The syntax of is quite complex and documented elsewhere.
         *  @param callback The function called when a client sends a request on the url.
         *
         */
        InterfaceProvider& head(std::string const& url, std::function <void(Request&, Response&)> callback);

        /**
         *  Registers a new patch request handler.
         *
         *  @param url The url to listen on. The syntax of is quite complex and documented elsewhere.
         *  @param callback The function called when a client sends a request on the url.
         *
         */
        InterfaceProvider& patch(std::string const& url, std::function <void(Request&, Response&)> callback);

        // Needs special handling: trace, options
        // Not supported: connect

    private:
        void connectionHandler(std::shared_ptr <RestConnection> connection);
        void errorHandler(std::shared_ptr <RestConnection> connection, InvalidRequest const& erroneousRequest);

    private:
        RestServer server_;
    };
}

#endif // RestFUL_HPP_INCLUDED
