#ifndef _HTTP_ROUTE_HPP
#define _HTTP_ROUTE_HPP

#include <functional>
#include <string>
#include <regex>

#include "http_request.hpp"
#include "http_response.hpp"

enum class HttpRouteHandling
{
    /**
     * @brief End the connection after the route was handled.
     */
    End,
    /**
     * @brief Continue trying to match the route with other handlers.
     */
    Continue
};

typedef std::function<HttpRouteHandling (const HttpRequest &req, HttpResponse &res)> HttpHandlerFn;

class HttpRoute
{
public:

    enum class MatchType
    {
        Regex,
        StartsWith,
        Literal,
        MatchAny
    };

private:

    std::string route;
    std::regex route_matcher;

    HttpRoute::MatchType matchType;

    HttpHandlerFn handler_fn;

public:
    HttpRoute(const std::string &route, HttpHandlerFn handler, 
        HttpRoute::MatchType matchType = HttpRoute::MatchType::Regex);

    const std::string & getRoute() const;

    const HttpRoute::MatchType & getMatchType() const;

    friend class HttpServer;

};

#endif // _HTTP_ROUTE_HPP