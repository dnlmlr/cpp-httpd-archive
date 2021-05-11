#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <sstream>
#include <chrono>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "httpd.hpp"
#include "http_service.hpp"


HttpRouteHandling handle_echo_path(const HttpRequest &req, HttpResponse &res)
{
    std::string echo_text = req.regexMatches().at(1);

    std::string resp = "Echo:  " + echo_text;

    res.send((uint8_t*) resp.c_str(), resp.size());

    return HttpRouteHandling::End;
}

// Calculate the first N prime numbers where N is extracted from the request uri
HttpRouteHandling handle_prime(const HttpRequest &req, HttpResponse &res)
{
    std::string result;
    int number_of_primes_found = 0;

    int max = std::stoi(req.regexMatches().at(1));

    int num = 2;
    while(number_of_primes_found < max)
    {
        bool is_prime = true;
        for (int i = 2; i <= num/2; i++)
        {
            if (num % i == 0)
            {
                is_prime = false;
                break;
            }
        }
        if (is_prime)
        {
            result += std::to_string(num) + " ";
            number_of_primes_found++;
        }
        num++;
    }

    res.send((uint8_t*) result.c_str(), result.size());

    return HttpRouteHandling::End;
}

int main(int argc, char **argv)
{
    // Create the webserver on port 8081. By default listening on 0.0.0.0
    HttpServer srv(8081);

    // Log Middleware example
    srv.addRoute(HttpRoute(
        "",
        [](const HttpRequest &req, HttpResponse &res) {

            std::string log = "Request: " + req.ip() + " => " + req.uri() + "\n";
            log +=            "  Agent: " + req.headers().getHeader(HttpHeader::UserAgent).getValue() + "\n";

            std::cout << log;
            
            return HttpRouteHandling::Continue;
        },
        HttpRoute::MatchType::MatchAny
    ));

    // Example index route with hardcoded html response
    srv.addRoute(HttpRoute(
        "/(index.html)?", // match "/" or "/index.html"
        [](const HttpRequest &req, HttpResponse &res) {

            char body[] = R"EOT(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Example HTML Document</title>
</head>
<body>
    <h1><a href="/example.html">example.html</a></h1>
    <h1><a href="/cached/example.html">cached example.html</a></h1>
    <h1><a href="/prime/100">first 100 primes</a></h1>
    <h1><a href="/echo/Hello">echo hello</a></h1>
</body>
</html>
            )EOT";
            res.sendAll((uint8_t*)body, sizeof(body));

            return HttpRouteHandling::End;
        }
    ));

    // Static file example
    srv.addRoute(serveFile(
        "/example.html",
        "./static/example.html",
        "text/html; charset=utf-8"
    ));

    // Cached static file example. The file is loaded into memory at launch
    srv.addRoute(serveFileCached(
        "/cached/example.html",
        "./static/example.html",
        "text/html; charset=utf-8"
    ));

    // Example for a handler function that uses regex to extract a path segment
    srv.addRoute(HttpRoute(
        "/prime/(\\d+)",
        &handle_prime
    ));

    // Example for a handler function that uses regex to extract a path segment
    srv.addRoute(HttpRoute(
        "/echo/([a-zA-Z0-9_\\-.]+)/?", 
        &handle_echo_path
    ));

    try
    {

        // Start serving the content
        srv.serveForever();

    }
    catch(const HttpException& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
    
}
