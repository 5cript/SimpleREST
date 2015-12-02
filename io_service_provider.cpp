#include "io_service_provider.hpp"

namespace REST {

    IOServiceProvider::IOServiceProvider()
        : ioService()
    {
    }

    boost::asio::io_service& IOServiceProvider::getIOService()
    {
        return ioService;
    }

}
