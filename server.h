#ifndef SERVER_H
#define SERVER_H
#include "connection.h"

#include <string>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class server
{
public:
    server(boost::asio::io_service& io_service, tcp::endpoint endpoint, std::string redirect_address, unsigned short redirect_port, unsigned long delay_time);
    
private:
    void start_accept(boost::asio::io_service& io_service);
    
    std::string redirect_address;
    unsigned short redirect_port;
    tcp::acceptor server_acceptor;
    tcp::socket incoming_socket;
    
    unsigned long delay_time;
};

#endif // SERVER_H
