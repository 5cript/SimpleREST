#include "restful.hpp"

#include <cassert>
#include <algorithm>

namespace Rest
{
//#######################################################################################################
    InterfaceProvider::InterfaceProvider(uint32_t port)
        : server_(
            std::bind(connectionHandler, this, std::placeholders::_1),
            std::bind(errorHandler, this, std::placeholders::_1, std::placeholders::_2),
            port
        )
    {

    }
//-------------------------------------------------------------------------------------------------------
    InterfaceProvider& InterfaceProvider::get(std::string const& url, std::function <void(Request, Response)> callback)
    {
        registerRequest("GET", url, callback);
        return *this;
    }
//-------------------------------------------------------------------------------------------------------
    InterfaceProvider& InterfaceProvider::put(std::string const& url, std::function <void(Request, Response)> callback)
    {
        registerRequest("PUT", url, callback);
        return *this;
    }
//-------------------------------------------------------------------------------------------------------
    InterfaceProvider& InterfaceProvider::post(std::string const& url, std::function <void(Request, Response)> callback)
    {
        registerRequest("POST", url, callback);
        return *this;
    }
//-------------------------------------------------------------------------------------------------------
    InterfaceProvider& InterfaceProvider::remove(std::string const& url, std::function <void(Request, Response)> callback)
    {
        registerRequest("DELETE", url, callback);
        return *this;
    }
//-------------------------------------------------------------------------------------------------------
    InterfaceProvider& InterfaceProvider::head(std::string const& url, std::function <void(Request, Response)> callback)
    {
        registerRequest("HEAD", url, callback);
        return *this;
    }
//-------------------------------------------------------------------------------------------------------
    InterfaceProvider& InterfaceProvider::patch(std::string const& url, std::function <void(Request, Response)> callback)
    {
        registerRequest("PATCH", url, callback);
        return *this;
    }
//-------------------------------------------------------------------------------------------------------
    void InterfaceProvider::start()
    {
        server_.start();
    }
//-------------------------------------------------------------------------------------------------------
    void InterfaceProvider::stop()
    {
        server_.stop();
    }
//-------------------------------------------------------------------------------------------------------
    void InterfaceProvider::connectionHandler(std::shared_ptr <RestConnection> connection)
    {
        auto url = ReducedUrlParser::parse(connection->getRequestHeader().url);
        auto type = connection->getRequestHeader().requestType;
        auto requestList = requests_.find(type);

        // is there any request matching the request type?
        if (requestList == std::end(requests_))
        {
            if (type != "GET" && type != "POST" && type != "PUT" && type != "DELETE" && type != "HEAD" && type != "PATCH")
            {
                Response response (connection);
                response.sendStatus(501);
                return;
            }
            else
            {
                Response response (connection);
                response.sendStatus(404);
                return;
            }
        }

        // is there a registered request, that matches the url?
        auto request = std::find_if(std::begin(requestList->second), std::end(requestList->second), [&, this](BuiltRequest const& request) {
            return matching(url, request.url);
        });

        if (request == std::end(requestList->second))
        {
            Response response (connection);
            response.sendStatus(404);
            return;
        }

        auto params = extractParameters(url, request->url);

        request->callback(
            Request {connection, params},
            Response {connection}
        );
    }
//-------------------------------------------------------------------------------------------------------
    void InterfaceProvider::errorHandler(std::shared_ptr <RestConnection> connection, InvalidRequest const& erroneousRequest)
    {
        Response response (connection);
        response.sendStatus(400);
    }
//-------------------------------------------------------------------------------------------------------
    bool InterfaceProvider::matching(Url received, Url registered)
    {
        auto registeredParts = registered.parsePath();
        auto receivedParts = received.parsePath(true);

        if (registeredParts.size() != receivedParts.size())
            return false;

        for (std::size_t i = 0; i != registeredParts.size(); ++i)
        {
            if (registeredParts[i]->getType() == PathType::STRING)
            {
                // check if path string parts are equal
                if (registeredParts[i]->getValue() != receivedParts[i]->getValue())
                    return false;
            }
        }
        return true;
    }
//-------------------------------------------------------------------------------------------------------
    std::unordered_map <std::string, std::string> InterfaceProvider::extractParameters(Url received, Url registered)
    {
        auto registeredParts = registered.parsePath();
        auto receivedParts = received.parsePath(true);

        assert (registeredParts.size() == receivedParts.size());

        std::unordered_map <std::string, std::string> map;
        for (std::size_t i = 0; i != registeredParts.size(); ++i)
        {
            if (registeredParts[i]->getType() == PathType::PARAMETER)
                map[static_cast <PathParameter*> (registeredParts[i].get())->getId()] = receivedParts[i]->getValue();
        }
        return map;
    }
//-------------------------------------------------------------------------------------------------------
    void InterfaceProvider::registerRequest(std::string const& type, std::string const& url, std::function <void(Request, Response)> callback)
    {
        BuiltRequest req {
            ReducedUrlParser::parse(url),
            callback
        };
        requests_[type].push_back(req);
    }
//#######################################################################################################
}
