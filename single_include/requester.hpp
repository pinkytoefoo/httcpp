#pragma once

#include <initializer_list>
#include <memory>
#include <stdint.h>
#include <expected>
#include <format>

#include <curl/curl.h>

struct response
{
    std::string text;
    int http_code{-1}; // -1 meaning no request has been sent
    CURLcode curl_code;
    bool ok;
};

struct curl_deleter
{
    void operator()(CURL* curl) const
    {
        if (curl) curl_easy_cleanup(curl);
    }
};

using curl_ptr = std::unique_ptr<CURL, curl_deleter>;

class curl_ex : public std::runtime_error {
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


class requester
{
public:
    requester()
        : curl_{curl_easy_init()}, headers_{nullptr}
    {
        if (!curl_.get())
            throw curl_ex("curl_easy_init() failed!");

        curl_easy_setopt(curl_.get(), CURLOPT_WRITEFUNCTION,
        +[](void* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
            size_t total = size * nmemb;
            auto* res = static_cast<response*>(userdata);
            res->text.append(static_cast<char*>(ptr), total);
            return total;
        });
    }

    ~requester()
    {
        reset_headers();
        curl_easy_cleanup(curl_.get());
    }
    
    // http methods
    response get(std::string_view url) {
        response res;
        // set response as write target
        curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, &res);
        // set url
        curl_easy_setopt(curl_.get(), CURLOPT_URL, url.data());
        
        CURLcode curl_status = curl_easy_perform(curl_.get());

        int http_code_;
        if(curl_status == CURLE_OK)
            curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &http_code_);
        else
            throw curl_ex(curl_status, "curl_easy_perform() failed: {}", curl_easy_strerror(curl_status));

        res.http_code = http_code_;
        res.ok = (http_code_ >= 200 && http_code_ < 300);
        return res;
    }
    
    // headers
    void set_header(std::string_view header) {
        headers_ = curl_slist_append(headers_, header.data());
        curl_easy_setopt(curl_.get(), CURLOPT_HTTPHEADER, headers_);
    }

    void set_headers(std::initializer_list<std::string> headers) {
        for (const auto& header : headers)
        {
            headers_ = curl_slist_append(headers_, header.c_str());
        }
        curl_easy_setopt(curl_.get(), CURLOPT_HTTPHEADER, headers_);
    }
    
    void reset_headers() {
        curl_slist_free_all(headers_);
    }

private:
    curl_ptr curl_;
    struct curl_slist* headers_;
};
