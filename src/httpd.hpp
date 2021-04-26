#ifndef _HTTPD_HPP
#define _HTTPD_HPP

#include <regex>
#include <string>
#include <unordered_map>
#include <functional>

#include "http_err.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_route.hpp"

#include "threadpool.hpp"

class HttpServer
{
private:

    ssize_t tcp_read_buffer_size = 4096;

    int pendingConnections = 5;
    int queuedConnections = 5;

    int numberOfThreads = Threadpool::AUTO_NO_WORKERS;

    std::string ip;

    in_addr ip_inaddr;

    uint16_t port;

    std::vector<HttpRoute> routes;

    static HttpRouteHandling defaultHandlerFunction(const HttpRequest &req, HttpResponse & res);

    HttpHandlerFn defaultHandler = &defaultHandlerFunction;

    void handleConnection(int sockfd, const std::string &ip, uint16_t port);

public:

    HttpServer(uint16_t port, std::string ip = "0.0.0.0");

    void addRoute(const HttpRoute &route)
    {
        routes.push_back(route);
    }

    void setDefaultHandler(const HttpHandlerFn &);

    void serveForever();

};

#endif // _HTTPD_HPP