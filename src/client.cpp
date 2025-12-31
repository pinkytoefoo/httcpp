#include <stdexcept>
#include <memory>

#include <curl/curl.h>

#include "httcpp/client.h"
#include "httcpp/response.h"

client::client()
    : curl_{curl_easy_init()}, headers_{nullptr}
{
    if (!curl_.get())
        throw curl_ex("httcpp: curl_easy_init() failed!");

    curl_easy_setopt(curl_.get(), CURLOPT_WRITEFUNCTION,
    +[](void* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
        size_t total = size * nmemb;
        auto* res = static_cast<response*>(userdata);
        res->text.append(static_cast<char*>(ptr), total);
        return total;
    });
}

response client::get(std::string_view url)
{
    response res;
    // attach headers
    curl_easy_setopt(curl_.get(), CURLOPT_HTTPHEADER, headers_);
    // set response as write target
    curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, &res);
    // set url
    curl_easy_setopt(curl_.get(), CURLOPT_URL, url.data());
    
    CURLcode curl_status = curl_easy_perform(curl_.get());

    int http_code_;
    if(curl_status == CURLE_OK)
        curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &http_code_);
    else
        throw curl_ex(curl_status, "httcpp: curl_easy_perform() failed: {}", curl_easy_strerror(curl_status));

    res.http_code = http_code_;
    res.ok = (http_code_ >= 200 && http_code_ < 300);
    return res;
}

response client::get(std::string_view url, std::initializer_list<std::string> headers)
{
    set_headers(std::forward<std::initializer_list<std::string>>(headers));

    response res;
    // attach headers
    curl_easy_setopt(curl_.get(), CURLOPT_HTTPHEADER, headers_);
    // set response as write target
    curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, &res);
    // set url
    curl_easy_setopt(curl_.get(), CURLOPT_URL, url.data());
    
    CURLcode curl_status = curl_easy_perform(curl_.get());

    int http_code_;
    if(curl_status == CURLE_OK)
        curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &http_code_);
    else
        throw curl_ex(curl_status, "httcpp: curl_easy_perform() failed: {}", curl_easy_strerror(curl_status));

    res.http_code = http_code_;
    res.ok = (http_code_ >= 200 && http_code_ < 300);
    return res;
}

response client::post(std::string_view url, std::string_view data)
{
    response res;
    curl_easy_setopt(curl_.get(), CURLOPT_HTTPHEADER, headers_);
    curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, &res);
    curl_easy_setopt(curl_.get(), CURLOPT_POSTFIELDS, data.data());
    curl_easy_setopt(curl_.get(), CURLOPT_URL, url.data());

    CURLcode curl_status = curl_easy_perform(curl_.get());

    int http_code_;
    if(curl_status == CURLE_OK)
        curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &http_code_);
    else
        throw curl_ex(curl_status, "httcpp: curl_easy_perform() failed: {}", curl_easy_strerror(curl_status));

    res.http_code = http_code_;
    res.ok = (http_code_ >= 200 && http_code_ < 300);
    return res;

}

void client::set_header(std::string_view header)
{
    headers_ = curl_slist_append(headers_, header.data());
}

void client::set_headers(std::initializer_list<std::string> headers)
{
    for (const auto& header : headers)
    {
        headers_ = curl_slist_append(headers_, header.c_str());
    }
}

void client::reset_headers()
{
    curl_slist_free_all(headers_);
}

client::~client()
{
    reset_headers();
    curl_easy_cleanup(curl_.get());
}
