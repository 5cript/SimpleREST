#include "exceptions.hpp"

namespace RestApi
{
//#######################################################################################################
    RestException::RestException(std::string message)
        : message_(std::move(message))
    {

    }
//-------------------------------------------------------------------------------------------------------
    const char* RestException::what() const noexcept
    {
        return message_.c_str();
    }
//-------------------------------------------------------------------------------------------------------
    InvalidRequest::InvalidRequest(std::string message)
        : RestException(std::move(message))
    {

    }
//#######################################################################################################
} // namespace RestApi



