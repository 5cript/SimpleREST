#pragma once

#include "forward.hpp"
#include "user_id.hpp"
#include "exceptions.hpp"

//#include "async_connection.hpp"
//#include "connection.hpp"

#include <boost/asio.hpp>

#include <functional>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

namespace Rest {

    /**
     *  A Restful web service server. Provide a callback function which is called when a http request is made.
     */
    template <typename ConnectionType>
    class RestServer
    {
        friend RestConnection;
        friend AsyncRestConnection;

    public:
        /**
         *  Constructs the RestServer, does not automatically bind.
         *
         *  @param handler A handler called when a request has been recieved.
         *         The connection is automatically closed when the function ends.
         *         You can use the connections stream to write a response.
         *
         *  @param errorHandler A handler called when a request is bad.
         *
         *  @param port The port to bind on.
         */
        RestServer(boost::asio::io_service* ioService,
                   std::function <void(std::shared_ptr <ConnectionType>)> handler,
                   std::function <void(std::shared_ptr <ConnectionType>, InvalidRequest const&)> errorHandler,
                   uint32_t port = 80)
            : endpoint_(boost::asio::ip::tcp::v4(), port)
            , acceptor_(nullptr)
            , handler_(handler)
            , errorHandler_(errorHandler)
            , nextConnection_()
            , nextRemoteEndpoint_()
            , listening_(false)
            , idIncrement_(0)
            , memberLock_()
            , ioService_(ioService)
            , connections_()
        {

        }

        /**
         *  Deconstructor. Automatically destroys all connections. Beware!
         *  Never keep any spare shared_ptrs to RestConnections.
         */
        ~RestServer() = default;

        /**
         *  Starts the server and binds to the port. May throw if the socket
         *  cannot be bound to any interface.
         */
        void start()
        {
            acceptor_.reset(new boost::asio::ip::tcp::acceptor(*ioService_, endpoint_));
            listening_.store(true);
            acceptNext();
        }

        /**
         *  Stops the server. Please make sure to finish off all clients or this might lead to std::terminate.
         */
        void stop()
        {
            listening_.store(false);
            acceptor_.reset();
        }

        /**
         *  Retrieves the io_service used for this server.
         *
         *  @return A pointer to a boost asio io_service (not owning).
         */
        boost::asio::io_service* getIoService()
        {
            return ioService_;
        }

    private:
        /**
         *  called by connection to deregister itself.
         */
        void deregisterClient(ConnectionType* connection)
        {
            std::lock_guard <std::mutex> guard (memberLock_);

            connections_.erase(connection->getId());
        }

        /**
         *  Used internally for async accepts
         */
        void acceptNext()
        {
            std::lock_guard <std::mutex> aLock (acceptLock_);
            idIncrement_.store(idIncrement_.load() + 1);
            nextConnection_.reset(new ConnectionType(this, idIncrement_.load()));
            acceptor_->async_accept(nextConnection_->getAcceptHandle(), nextRemoteEndpoint_,
                [this](boost::system::error_code const& error)
                {
                    acceptHandler(error);
                }
            );
        }

        /**
         *  Called when new client connects.
         */
        void acceptHandler(boost::system::error_code const& error)
        {
            if (!error)
            {
                {
                    std::lock_guard <std::mutex> aLock (acceptLock_);

                    nextConnection_->setEndpoint(nextRemoteEndpoint_);
                    // LOCK_SCOPE
                    {
                        std::lock_guard <std::mutex> guard (memberLock_);
                        connections_.insert({nextConnection_->getId(), nextConnection_});

                        try {
                            nextConnection_->readHead([this](std::shared_ptr <AsyncRestConnection> connection) {
                                handler_(connection);
                            });
                        } catch (InvalidRequest const& exc) {
                            errorHandler_(nextConnection_, exc);
                        }
                    }

                }

                if (listening_.load())
                    acceptNext();
            }
            else
            {
                std::cout << "accept failed\n" << error.message() << "\n";
            }
        }

    private:
        boost::asio::ip::tcp::endpoint endpoint_; // socket endpoint
        std::unique_ptr <boost::asio::ip::tcp::acceptor> acceptor_; // acceptor accepting connections

        std::function <void(std::shared_ptr <ConnectionType>)> handler_; // handler callback for connections.
        std::function <void(std::shared_ptr <ConnectionType>, InvalidRequest const&)> errorHandler_; // handler for invalid requests.

        std::thread acceptingThread_;
        std::shared_ptr <ConnectionType> nextConnection_;
        boost::asio::ip::tcp::acceptor::endpoint_type nextRemoteEndpoint_;
        std::atomic_bool listening_; // listening flag = server is bound?
        std::atomic <uint64_t> idIncrement_; // auto increment for ids. overflow is only an issue if the first connections still exists after 256**8 connections have gone through.
        std::mutex memberLock_; // thread protection
        std::mutex acceptLock_;

        boost::asio::io_service* ioService_;
        std::unordered_map <UserId, std::shared_ptr <ConnectionType>, UserIdHasher> connections_; // all currently connected peers.
    };

} // namespace Rest
