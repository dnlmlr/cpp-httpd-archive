#include "http_route.hpp"

HttpRoute::HttpRoute(const std::string &route, HttpHandlerFn handler, 
    HttpRoute::MatchType matchType)
        : route{route}, route_matcher{"^" + route + "$"}, 
            handler_fn {handler}, matchType{matchType}
{ }

const std::string & HttpRoute::getRoute() const
{
    return route;
}

const HttpRoute::MatchType & HttpRoute::getMatchType() const
{
    return matchType;
}