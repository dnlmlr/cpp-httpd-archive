#ifndef _HTTP_HEADER_HPP
#define _HTTP_HEADER_HPP

#include <string>
#include <unordered_map>

class HttpHeader
{
private:

    bool _isSet;

    std::string key;
    std::string value;

public:

    HttpHeader();

    HttpHeader(const std::string & key, const std::string & value);

    const bool isSet() const;

    const std::string & getKey() const;

    const std::string & getValue() const;


    void setKey(const std::string & key);

    void setValue(const std::string & value);


    friend class HttpHeaders;


    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers

    static const std::string Authorization;

    static const std::string Connection;
    static const std::string KeepAlive;

    static const std::string Accept;
    static const std::string AcceptCharset;
    static const std::string AcceptEncoding;

    static const std::string Cookie;
    static const std::string SetCookie;

    static const std::string ContentDisposition;

    static const std::string ContentLength;
    static const std::string ContentType;
    static const std::string ContentEncoding;
    static const std::string ContentLanguage;
    static const std::string ContentLocation;

    static const std::string Location;

    static const std::string Host;
    static const std::string Referer;
    static const std::string UserAgent;

    static const std::string Allow;
    static const std::string Server;

    static const std::string AcceptRanges;
    static const std::string Range;
    static const std::string ContentRange;

    static const std::string TransferEncoding;

    static const std::string Date;

};

class HttpHeaders
{
private:
    /**
     * @brief Statically allocated empty string to be used for empty values without
     * the need for new allocations.
     */
    const static std::string emptyString;

    /**
     * @brief Statically allocated empty header to be used for empty values without
     * the need for new allocations.
     */
    const static HttpHeader emptyHeader;

    std::unordered_map<std::string, HttpHeader> _headers;

public:

    const HttpHeader & getHeader(const std::string & key) const;

    const bool headerExists(const std::string & key) const;

    const std::string & getValueOrEmpty(const std::string & key) const;

    const std::unordered_map<std::string, HttpHeader> & getRawHeaders() const;


    void setHeader(const HttpHeader & header);

    void setHeader(const std::string & key, const std::string & value);

    void unsetHeader(const std::string & key);

};

#endif // _HTTP_HEADER_HPP