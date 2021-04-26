#ifndef _HTTP_ERR_HPP
#define _HTTP_ERR_HPP

#include <string>
#include <exception>

class HttpException : virtual public std::exception
{
public:
    enum Type
    {
        Generic,
        SocketOpen,
        InvalidIP,
        SocketBind,
        TcpAccept,
        TcpSend
    };

protected:
    std::string message;
    Type error_type;

public:

    HttpException(const HttpException::Type &type, const std::string &message);

    HttpException(const HttpException::Type &type);

    const char * what()
        const noexcept override;
    
    const HttpException::Type & getType() const noexcept;

    static const char * typeToString(const HttpException::Type & type);
};


#endif // _HTTP_ERR_HPP