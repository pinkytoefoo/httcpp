#pragma once

#include <string>

#include <curl/curl.h>

// TODO: add response `headers` object
struct response
{
    std::string text;
    int http_code{-1}; // -1 meaning no request has been sent
    CURLcode curl_code;
    bool ok;

    // TODO: implement a pretty print without nlohmann's json
    // assumes response is of type json
    //std::string pretty_json(std::string_view str, int size);
    void ensure_ok() const;
    operator bool() const { return ok; };
};