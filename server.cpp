#include "server.hpp"
#include "io_service_provider.hpp"
#include "connection.hpp"

#include <iostream>

using namespace boost::asio::ip;

namespace Rest
{
//#######################################################################################################
    /*
    void RestServer::start()
    {
        acceptor_.reset(new tcp::acceptor(IOServiceProvider::getInstance().getIOService(), endpoint_));

        listening_.store(true);

        //acceptNext();

        // is currently running, stop first
        if (acceptingThread_.joinable())
            stop();

        listening_.store(true);
        acceptingThread_ = std::thread([this]() {
            for (;listening_.load();)
            {
                idIncrement_.store(idIncrement_.load() + 1);
                std::shared_ptr <RestConnection> connection (new RestConnection(this, idIncrement_.load()));
                boost::system::error_code ec;
                boost::asio::ip::tcp::acceptor::endpoint_type remoteEndpoint;
                acceptor_->accept(*connection->getStream().rdbuf(), remoteEndpoint, ec);
                if (!ec)
                {
                    connection->setEndpoint(remoteEndpoint);
                    // LOCK_SCOPE
                    {
                        std::lock_guard <std::mutex> guard (memberLock_);
                        connections_.insert({connection->getId(), connection});
                    }
                    std::thread([connection, this](){
                        try {
                            connection->readHead();
                            handler_(connection);
                        } catch (InvalidRequest const& exc) {
                            errorHandler_(connection, exc);
                        }
                        connection->free();
                        //catch (...) {
                        // std::terminate - do not handle unexpected exceptions.
                        // we don't wanna catch our programming errors ;)
                        //}
                    }).detach();
                }
                else
                {
                    std::cout << "accept failed\n" << ec.message() << "\n";
                }
            }
        });
    }
    */
//#######################################################################################################
} // namespace Rest
