#include <stdexcept>

#include "httcpp/response.h"

void response::ensure_ok() const
{
    if(!ok)
        throw std::runtime_error{"httcpp: request failed with http status code " + http_code};
}