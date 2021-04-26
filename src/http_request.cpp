#include "http_request.hpp"

const std::string & HttpRequest::ip() const
{
    return _ip;
}

int HttpRequest::port() const
{
    return _port;
}

const std::string & HttpRequest::method() const
{
    return _method;
}

const std::string & HttpRequest::uri() const
{
    return _uri;
}

const std::string & HttpRequest::httpver() const
{
    return _httpver;
}

const HttpHeaders & HttpRequest::headers() const
{
    return _headers;
}

const std::vector<std::string> & HttpRequest::regexMatches() const
{
    return _regexMatches;
}