#ifndef _HTTP_REQUEST_HPP
#define _HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "http_header.hpp"

class HttpRequest
{
private:

    std::string _ip;
    int _port;

    std::string _method;
    std::string _uri;
    std::string _httpver;

    HttpHeaders _headers;

    std::vector<std::string> _regexMatches;

public:

    const std::string & ip() const;

    int port() const;

    const std::string & method() const;

    const std::string & uri() const;

    const std::string & httpver() const;

    const HttpHeaders & headers() const;

    const std::vector<std::string> & regexMatches() const;

    friend class HttpServer;
    friend bool parse_headers_into(const std::string &head_text, HttpRequest &req);
    friend bool parse_requestline_into(const std::string &head_text, HttpRequest &req);
};

#endif // _HTTP_REQUEST_HPP