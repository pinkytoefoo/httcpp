#pragma once

#include <memory>
#include <string>
#include <initializer_list>
#include <stdexcept>
#include <format>

#include <curl/curl.h>

#include "httcpp/response.h"

struct curl_deleter
{
    void operator()(CURL* curl) const
    {
        if (curl) curl_easy_cleanup(curl);
    }
};

using curl_ptr = std::unique_ptr<CURL, curl_deleter>;

// TODO: put into seperate file and implement (maybe)
// class headers_store
// {
// public:
//     headers_store();
//     ~headers_store();
// private:
// };

class client
{
public:
    client();
    ~client();
    
    response get(std::string_view url);
    response get(std::string_view url, std::initializer_list<std::string> headers);
    response post(std::string_view url, std::string_view data);
    
    void set_header(std::string_view header);
    void set_headers(std::initializer_list<std::string> headers);
    void reset_headers();
private:
    curl_ptr curl_;
    struct curl_slist* headers_;
};

class curl_ex : public std::runtime_error
{
public:
    template<typename... Args>
    explicit curl_ex(const std::format_string<Args...> message, Args&&... args)
        : std::runtime_error{std::format(message, std::forward<Args>(args)...)}
    {
    }

    template<typename... Args>
    explicit curl_ex(CURLcode code, const std::format_string<Args...> message, Args&&... args)
        : code_{code}, std::runtime_error{std::format(message, std::forward<Args>(args)...)}
    {
    }

    CURLcode get_code() const { return code_; }
private:
    CURLcode code_;
};
