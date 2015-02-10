#include "server.h"

#include <iostream>

#include <boost/bind.hpp>

server::server(boost::asio::io_service& io_service, tcp::endpoint endpoint, std::string redirect_address, unsigned short redirect_port, unsigned long delay_time) : 
redirect_address(redirect_address), redirect_port(redirect_port), server_acceptor(io_service, endpoint), incoming_socket(io_service), delay_time(delay_time)
{
    start_accept(io_service);
}

void server::start_accept(boost::asio::io_service& io_service)
{  
    server_acceptor.async_accept(incoming_socket, [this, &io_service](const boost::system::error_code& error)
    {
        if (!error)
            std::make_shared<connection>(std::move(incoming_socket), io_service)->start(redirect_address, redirect_port, delay_time);

        start_accept(io_service);
    });
}
