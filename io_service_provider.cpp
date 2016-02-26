#include "io_service_provider.hpp"

#include <iostream>

namespace Rest {

    IOServiceProvider::~IOServiceProvider()
    {
        running_.store(false);
        ioService.stop();

        if (worker_.joinable())
            worker_.join();
    }

    IOServiceProvider::IOServiceProvider()
        : ioService()
        , running_(true)
        , worker_()
    {
        worker_ = std::thread([this](){
            while (running_.load())
            {
                boost::system::error_code ec;
                ioService.run(ec);
                if (ec)
                {
                    std::cout << ec.message() << "\n";
                    break;
                }
                ioService.reset();
            }
        });
    }

    boost::asio::io_service& IOServiceProvider::getIOService()
    {
        return ioService;
    }

}
