#ifndef _HTTP_SERVICE_HPP
#define _HTTP_SERVICE_HPP

#include <string>
#include <fstream>

#include "http_route.hpp"



HttpRoute serveFile(
    const std::string route, const std::string & filePath, const std::string & contentType, 
    HttpRoute::MatchType matchType = HttpRoute::MatchType::Literal, 
    const std::vector<HttpHeader> setHeaders = std::vector<HttpHeader>{}
)
{
    return HttpRoute (
        route,
        [filePath, contentType, setHeaders] (const HttpRequest &req, HttpResponse res) {

            std::ifstream inputFile(filePath, std::ios::binary);

            if (inputFile.fail())
            {
                res.sendDefault404();
                return HttpRouteHandling::End;
            }

            // This is not a good way to get the filesize
            auto fsize_beg = inputFile.tellg();
            inputFile.seekg(0, std::ios::end);
            auto fsize_end = inputFile.tellg();
            auto filesize = fsize_end-fsize_beg;
            inputFile.seekg(0, std::ios::beg);

            res.getHeadersWritable().setHeader("Content-Type", contentType);
            res.getHeadersWritable().setHeader("Content-Length", std::to_string(filesize));

            
            for (const auto &h : setHeaders)
            {
                res.getHeadersWritable().setHeader(h.getKey(), h.getValue());
            }

            res.sendHeader();

            uint8_t buffer[4096];
            ssize_t bytes_read = 0;

            while((bytes_read = inputFile.readsome((char*)buffer, 4096)) > 0)
            {
                res.sendBody(buffer, bytes_read);
            }

            return HttpRouteHandling::End;
        },
        matchType
    );
}

HttpRoute serveFileCached(
    const std::string route, const std::string & filePath, const std::string & contentType, 
    HttpRoute::MatchType matchType = HttpRoute::MatchType::Literal, 
    const std::vector<HttpHeader> setHeaders = std::vector<HttpHeader>{}
)
{
    std::ifstream inputFile(filePath, std::ios::binary);

    bool found = !inputFile.fail();

    std::vector<uint8_t> data;

    if (found)
    {
        uint8_t buffer[4096];
        ssize_t bytes_read = 0;

        while((bytes_read = inputFile.readsome((char*)buffer, 4096)) > 0)
        {
            data.insert(data.end(), std::begin(buffer), std::begin(buffer) + bytes_read);
        }

    }

    return HttpRoute (
        route,
        [filePath, contentType, setHeaders, data, found] (const HttpRequest &req, HttpResponse res) {

            if (!found)
            {
                res.sendDefault404();
                return HttpRouteHandling::End;
            }

            res.getHeadersWritable().setHeader("Content-Type", contentType);
            res.getHeadersWritable().setHeader("Content-Length", std::to_string(data.size()));

            for (const auto &h : setHeaders)
            {
                res.getHeadersWritable().setHeader(h.getKey(), h.getValue());
            }

            res.sendHeader();

            res.sendBody(data.data(), data.size());

            return HttpRouteHandling::End;
        },
        matchType
    );
}

#endif // _HTTP_SERVICE_HPP