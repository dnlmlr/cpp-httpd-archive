#include "http_header.hpp"

const std::string HttpHeader::Authorization = "Authorization";

const std::string HttpHeader::Connection = "Connection";
const std::string HttpHeader::KeepAlive = "Keep-Alive";

const std::string HttpHeader::Accept = "Accept";
const std::string HttpHeader::AcceptCharset = "Accept-Charset";
const std::string HttpHeader::AcceptEncoding = "Accept-Encoding";

const std::string HttpHeader::Cookie = "Cookie";
const std::string HttpHeader::SetCookie = "Set-Cookie";

const std::string HttpHeader::ContentDisposition = "Content-Disposition";

const std::string HttpHeader::ContentLength = "Content-Length";
const std::string HttpHeader::ContentType = "Content-Type";
const std::string HttpHeader::ContentEncoding = "Content-Encoding";
const std::string HttpHeader::ContentLanguage = "Content-Language";
const std::string HttpHeader::ContentLocation = "Content-Location";

const std::string HttpHeader::Location = "Location";

const std::string HttpHeader::Host = "Host";
const std::string HttpHeader::Referer = "Referer";
const std::string HttpHeader::UserAgent = "User-Agent";

const std::string HttpHeader::Allow = "Allow";
const std::string HttpHeader::Server = "Server";

const std::string HttpHeader::AcceptRanges = "Accept-Ranges";
const std::string HttpHeader::Range = "Range";
const std::string HttpHeader::ContentRange = "Content-Range";

const std::string HttpHeader::TransferEncoding = "Transfer-Encoding";

const std::string HttpHeader::Date = "Date";



HttpHeader::HttpHeader()
    : _isSet{false}
{ }

HttpHeader::HttpHeader(const std::string & key, const std::string & value)
    : _isSet{true}, key{key}, value{value}
{ }

const bool HttpHeader::isSet() const
{
    return _isSet;
}

const std::string & HttpHeader::getKey() const
{
    return key;
}

const std::string & HttpHeader::getValue() const
{
    return value;
}


void HttpHeader::setKey(const std::string & _key)
{
    key = _key;

    // Convert key to lowercase to ensure case insensitivity
    for (auto &c : key) c = tolower(c);
}

void HttpHeader::setValue(const std::string & _value)
{
    value = _value;
}


///////////////////////
// Class HttpHeaders //
///////////////////////


const HttpHeader HttpHeaders::emptyHeader{};

const std::string HttpHeaders::emptyString{};


const HttpHeader & HttpHeaders::getHeader(const std::string & key) const
{
    std::string key_lower = key;
    for (auto &c : key_lower) c = std::tolower(c);

    auto h = _headers.find(key_lower);
    if (h == _headers.end())
    {
        return HttpHeaders::emptyHeader;
    }
    
    return (*h).second;

}

const std::string & HttpHeaders::getValueOrEmpty(const std::string & key) const
{
    const HttpHeader & h = getHeader(key);
    if (!h._isSet) return HttpHeaders::emptyString;

    return h.getValue();
}

const std::unordered_map<std::string, HttpHeader> & HttpHeaders::getRawHeaders() const
{
    return _headers;
}

const bool HttpHeaders::headerExists(const std::string & key) const
{
    std::string key_lower = key;

    for (auto &c : key_lower) c = std::tolower(c);

    auto h = _headers.find(key_lower);

    if (h == _headers.end())
    {
        return false;
    }

    return true;
}


void HttpHeaders::setHeader(const HttpHeader & header)
{
    _headers[header.key] = header;
}

void HttpHeaders::setHeader(const std::string & key, const std::string & value)
{
    std::string key_lower = key;

    for (auto &c : key_lower) c = std::tolower(c);

    _headers[key_lower].key = key;
    _headers[key_lower].value = value;
}

void HttpHeaders::unsetHeader(const std::string & key)
{
    std::string key_lower = key;

    for (auto &c : key_lower) c = std::tolower(c);

    auto h = _headers.find(key_lower);

    if (h != _headers.end())
    {
        _headers.erase(h);
    }

}
