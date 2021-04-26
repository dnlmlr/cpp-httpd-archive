#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <regex>
#include <chrono>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


#include "threadpool.hpp"
#include "httpd.hpp"


bool parse_requestline_into(const std::string &head_text, HttpRequest &req)
{
    // Find the end of the request line
    auto offset_httpver_end = head_text.find("\r\n");

    if (offset_httpver_end == std::string::npos)
        return false;

    // Find the end of the first section (METHOD) and the beginning of the next (URI)
    auto offset_method_end = head_text.find(" ", 0);

    if (offset_method_end == std::string::npos || offset_method_end > offset_httpver_end)
        return false;

    std::string method = head_text.substr(0, offset_method_end);

    // Find the space after the URI and before the HTTPVER
    auto offset_uri_end = head_text.find(" ", offset_method_end+1);

    if (offset_uri_end == std::string::npos || offset_uri_end > offset_httpver_end)
        return false;

    std::string uri = head_text.substr(offset_method_end+1, offset_uri_end-offset_method_end -1);

    std::string httpver = head_text.substr(offset_uri_end+1, offset_httpver_end-offset_uri_end -1);

    req._method = method;
    req._uri = uri;
    req._httpver = httpver;

    return true;
}


bool parse_headers_into(const std::string &head_text, HttpRequest &req)
{
    // The first call gets the end of the request line
    auto offset = head_text.find("\r\n");

    // Loop as long as there are more lines to parse
    while (offset < head_text.size()-1 -2)
    {
        // Skip the 2 characters from \r\n
        offset += 2;

        // Search the next ": " divider, starting from the current line beginning
        auto offset_colon = head_text.find(": ", offset);

        if (offset_colon == std::string::npos)
            return false;

        auto header_key = head_text.substr(offset, offset_colon - offset);

        offset = head_text.find("\r\n", offset_colon);

        if (offset == std::string::npos)
            return false;

        auto header_val = head_text.substr(offset_colon+2, offset - offset_colon - 2);

        req._headers.setHeader(header_key, header_val);

    }

    return true;

}


void HttpServer::handleConnection(int sockfd, const std::string &ip, uint16_t port)
{
    // HttpRequest object will be filled with the parsed request parameters
    HttpRequest req;
    req._ip = ip;
    req._port = port;

    // Temporary read buffer that is used as an immediate target for receiving data
    std::vector<uint8_t> buff(tcp_read_buffer_size);

    // Body buffer that will be filled with accidentally read body data
    std::vector<uint8_t> body_buff;

    // String buffer that will be filled with the head data (request line + headers)
    std::string head_str_buff;

    // Byte offset at which the head ends (the location of the head-end \r\n\r\n )
    // The body content begins at offset_head_end + 4
    size_t offset_head_end = std::string::npos;

    // The number of bytes read with the last read command
    ssize_t bytes_read;

    // Loop and read from socket while the head is not finished and there is still data available
    while (offset_head_end == std::string::npos && (bytes_read = read(sockfd, buff.data(), tcp_read_buffer_size)) > 0)
    {
    
        // Append the newly read data to the head string buffer
        head_str_buff.append((char*)buff.data(), bytes_read);

        // Look for the head end. The loop will end if it is found
        offset_head_end = head_str_buff.find("\r\n\r\n");

    }

    // Cut off the head-end, as well as body data that was possibly read
    // One line end (\r\n) is left in for easier header parsing
    head_str_buff.resize(offset_head_end + 2);

    // Try to parse the request line (METHOD URI HTTPVER)
    if (!parse_requestline_into(head_str_buff, req))
    {
        char resp[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(sockfd, resp, sizeof(resp), 0);
        return;
    }

    // Try to parse the headers
    if (!parse_headers_into(head_str_buff, req))
    {
        char resp[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(sockfd, resp, sizeof(resp), 0);
        return;
    }


    // Insert potential body data into the body buffer. Body data might accidently be read 
    // when the head-end (\r\n\r\n) is somewhere in the middle of the read buffer.
    // This does not read the full body data, instead only data that was already read is 
    // inserted
    if (offset_head_end < head_str_buff.size()-1)
    {
        body_buff.insert(body_buff.end(), buff.begin() + offset_head_end + 4, buff.begin() + bytes_read);
    }
    
    
    // Prepare HttpResponse 
    HttpResponse res(sockfd);

    // Try to match routes available for the server using the dedicated matching type 
    // for each available route.
    bool finalHandled = false;
    for (const auto &route : routes)
    {
        bool match_found = false;

        std::smatch matches;

        switch (route.matchType)
        {
        case HttpRoute::MatchType::MatchAny:
            match_found = true;
        break;
        
        case HttpRoute::MatchType::Regex:
            if (std::regex_search(req._uri, matches, route.route_matcher))
            {
                for (auto m : matches)
                {
                    req._regexMatches.push_back(m);
                }

                match_found = true;
            }
        break;

        case HttpRoute::MatchType::Literal:
            if (route.route == req._uri)
            {
                match_found = true;
            }
        break;

        case HttpRoute::MatchType::StartsWith:
            if (req._uri.compare(0, route.route.size(), route.route) == 0)
            {
                match_found = true;
            }
        break;
        }

        if (match_found) 
        {
            auto routeType = route.handler_fn(req, res);
            if (routeType == HttpRouteHandling::End)
            {
                finalHandled = true;
                break;
            }
        }

    }


    // If none of the routes match, use the default handler. This will cause a 
    // 404 Not found status code by default
    if (!finalHandled) defaultHandler(req, res);

}


HttpRouteHandling HttpServer::defaultHandlerFunction(const HttpRequest &req, HttpResponse &res)
{
    res.sendDefault404();
    return HttpRouteHandling::End;
}


HttpServer::HttpServer(uint16_t port, std::string ip)
    : port{port}, ip{ip}
{
    // Try to convert the given ipv4 string into the C lower level ipv4 struct 
    if (inet_aton(ip.c_str(), &ip_inaddr) == 0)
    {
        throw HttpException(HttpException::InvalidIP);
    }
}


void HttpServer::serveForever()
{
    // The number of connections that the server can keep on wait (not yet accepted) before
    // refusing further connection requests 
    const int waiting_connections = pendingConnections;

    // Socket File Descriptor for listening
    int sockfd_listen;

    // Socket Address for listening
    sockaddr_in local_saddr = {0};
    local_saddr.sin_family = AF_INET;
    local_saddr.sin_addr.s_addr = ip_inaddr.s_addr;
    local_saddr.sin_port = htons(port);

    try
    {
        // Create a tcp network socket
        sockfd_listen = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_listen < 0)
        {
            throw(HttpException(
                HttpException::SocketOpen, 
                "Opening socket failed"
            ));
        }

        // Bind the socket to the listening address and port
        if (bind(sockfd_listen, (sockaddr*) &local_saddr, sizeof(local_saddr)))
        {
            throw(HttpException(
                HttpException::SocketBind, 
                "Binding socket failed"
            ));
        }

        // Listen to new connections
        listen(sockfd_listen, waiting_connections);

        // Create and run the threadpool
        Threadpool tp(numberOfThreads, queuedConnections);

        // Accept-Handle-Repeat loop
        // This loops forever and handles new requests
        while (true)
        {
            // Remote socket address
            sockaddr_in remote_saddr = {0};
            // Length of the remote socket address structure
            socklen_t remote_slen = sizeof(remote_saddr);


            // Accept a new tcp connection
            int remote_sockfd = accept(sockfd_listen, (sockaddr*) &remote_saddr, &remote_slen);
            if (remote_sockfd < 0)
            {
                throw(HttpException(
                    HttpException::TcpAccept, 
                    "Accepting connection failed"
                ));
            }

            //std::cout << "Connection opened\n";

            // Pass the tcp connection socket to the http handling function
            // handleConnection(remote_sockfd);

            tp.addTask([this, remote_sockfd, remote_saddr]() {
                try
                {
                    // inet_ntoa returns the string in a statically allocated buffer
                    // that will be overwritten on subsequent calls. So no free needed
                    std::string remote_ip(inet_ntoa(remote_saddr.sin_addr));

                    uint16_t remote_port = ntohs(remote_saddr.sin_port);

                    handleConnection(remote_sockfd, remote_ip, remote_port);
                    close(remote_sockfd);
                
                }
                catch (const HttpException& e)
                {
                    close(remote_sockfd);
                    std::cerr << e.what() + '\n';
                }
            });

            //std::cout << "Connection closing\n";

            // Close the accepted tcp connection
            // close(remote_sockfd);

        }


    }
    catch (const HttpException& e)
    {
        close(sockfd_listen);
        throw e;
    }
}