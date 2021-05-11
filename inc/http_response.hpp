#ifndef _HTTP_RESPONSE_HPP
#define _HTTP_RESPONSE_HPP

#include <string>
#include <unordered_map>

#include "http_header.hpp"


class HttpResponse
{
private:

    int sockfd;

    uint16_t status;

    std::string statusPhrase;

    std::string httpver;

    HttpHeaders headers;

    void rawWriteAll(int sockfd, const uint8_t *data, size_t dataLength);

public:

    HttpResponse(int sockfd);

    void setStatus(uint16_t statusCode, std::string statusPhrase = "");

    void setHttpver(std::string httpver);

    HttpHeaders & getHeadersWritable();

    void sendHeader();

    void sendAll(const uint8_t *bodyData, size_t bodyLength);

    void send(const uint8_t *bodyData, size_t bodyLength);

    void sendBody(const uint8_t *bodyData, size_t bodyLength);

    void sendDefault404();

    friend class HttpServer;

};

#endif // _HTTP_RESPONSE_HPP