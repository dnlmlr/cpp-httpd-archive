#include "http_response.hpp"
#include <unistd.h>
#include <sys/socket.h>

#include "http_err.hpp"

HttpResponse::HttpResponse(int sockfd)
    : sockfd{sockfd}
{
    status = 200;
    statusPhrase = "OK";
    httpver = "HTTP/1.1";
    headers.setHeader(HttpHeader::ContentType, "text/html; charset=utf-8");
}


void HttpResponse::setStatus(uint16_t statusCode, std::string _statusPhrase)
{
    status = statusCode;
    statusPhrase = _statusPhrase;
}

void HttpResponse::setHttpver(std::string _httpver)
{
    httpver = _httpver;
}

HttpHeaders & HttpResponse::getHeadersWritable()
{
    return headers;
}

void HttpResponse::rawWriteAll(int sockfd, const uint8_t *data, size_t dataLength)
{
    size_t bytes_written_total = 0;
    ssize_t bytes_written = 0;

    do
    {
        bytes_written = write(sockfd, data + bytes_written_total, dataLength-bytes_written_total);

        if (bytes_written < 0)
        {
            throw HttpException(HttpException::TcpSend);
        }

        bytes_written_total += bytes_written;
    } while (bytes_written_total != dataLength);
}

void HttpResponse::sendHeader()
{
    std::string head = httpver + " " + std::to_string(status) + " " + statusPhrase + "\r\n";

    for (auto h : headers.getRawHeaders())
    {
        head += h.first + ": " + h.second.getValue() + "\r\n";
    }

    head += "\r\n";

    rawWriteAll(sockfd, (uint8_t*)head.c_str(), head.size());
}

void HttpResponse::sendAll(const uint8_t *bodyData, size_t bodyLength)
{
    sendHeader();
    sendBody(bodyData, bodyLength);
}

void HttpResponse::send(const uint8_t *bodyData, size_t bodyLength)
{
    sendAll(bodyData, bodyLength);
}

void HttpResponse::sendBody(const uint8_t *bodyData, size_t bodyLength)
{
    rawWriteAll(sockfd, bodyData, bodyLength);
}


void HttpResponse::sendDefault404()
{
    status = 404;
    statusPhrase = "Not found";
    headers.setHeader(HttpHeader::ContentType, "text/html; charset=utf-8");

    char body[] = "404 Not found";

    sendAll((uint8_t*)body, sizeof(body));
}