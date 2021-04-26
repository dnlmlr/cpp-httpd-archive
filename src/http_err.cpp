#include "http_err.hpp"

HttpException::HttpException(const HttpException::Type &type, const std::string &message)
{
    this->error_type = type;
    this->message = std::string(HttpException::typeToString(type)) + ": " + message;
}

HttpException::HttpException(const HttpException::Type &type)
    : HttpException(type, "?")
{ }

const char * HttpException::what() 
    const noexcept
{
    return message.c_str();
}


const HttpException::Type & HttpException::getType()
    const noexcept
{
    return error_type;
}

const char * HttpException::typeToString(const HttpException::Type & type)
{
    switch (type)
    {
    case Generic:
        return "HttpException::Generic";
    case SocketOpen:
        return "HttpException::SocketOpen";
    case InvalidIP:
        return "HttpException::InvalidIP";
    case SocketBind:
        return "HttpException::SocketBind";
    case TcpAccept:
        return "HttpException::TcpAccept";
    case TcpSend:
        return "HttpException::TcpSend";
    }

    return "HttpException::NoType";
}