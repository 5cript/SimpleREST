#include "io_service_provider.hpp"

namespace RestApi {

    IOServiceProvider::IOServiceProvider()
        : ioService()
    {
    }

    boost::asio::io_service& IOServiceProvider::getIOService()
    {
        return ioService;
    }

}
