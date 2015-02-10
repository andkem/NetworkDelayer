#include "connection.h"

#include <sstream>

#include <boost/bind.hpp>

connection::connection(tcp::socket incoming_socket, boost::asio::io_service& io_service) :
incoming_socket(std::move(incoming_socket)), forwarding_socket(io_service), resolver(io_service),
io_service(io_service)
{
}

connection::~connection()
{
    if (incoming_socket.is_open())
        incoming_socket.close();
    
    if (forwarding_socket.is_open())
        forwarding_socket.close();
}

void connection::start(std::string redirect_address, short unsigned int redirect_port, long unsigned int delay_time_in)
{
    auto self(shared_from_this());
    
    std::stringstream port_stream;
    port_stream << redirect_port;
    
    delay_time = delay_time_in;
    
    tcp::resolver::query query(redirect_address, port_stream.str());
    
    boost::asio::async_connect(forwarding_socket, resolver.resolve(query), [this, self](const boost::system::error_code& error, tcp::resolver::iterator itr)
    {
        if (!error)
        {
            incoming_socket.async_read_some(boost::asio::buffer(data_from_inc_client_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
            {
                handle_inc_data_from_inc_client(error, length);
            });
            
            forwarding_socket.async_read_some(boost::asio::buffer(data_from_forward_host_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
            {
                handle_inc_data_from_forward_host(error, length);
            });
        }
        else
            cleanup_on_error(error);
    });
}

void connection::handle_inc_data_from_forward_host(const boost::system::error_code& error, std::size_t length)
{
    auto self(shared_from_this());
    
    if (!error)
    {        
        incoming_socket.async_write_some(boost::asio::buffer(data_from_forward_host_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
        {
            if (!error)
            {
                boost::asio::deadline_timer sleep_timer(io_service);
                sleep_timer.expires_from_now(boost::posix_time::milliseconds(delay_time));
                
                sleep_timer.async_wait([this, self](const boost::system::error_code& error)
                {
                    forwarding_socket.async_read_some(boost::asio::buffer(data_from_forward_host_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
                    {
                            handle_inc_data_from_forward_host(error, length);
                    });
                });
            }
            else
                cleanup_on_error(error);
        });
    }
    else
        cleanup_on_error(error);
}

void connection::handle_inc_data_from_inc_client(const boost::system::error_code& error, std::size_t length)
{
    auto self(shared_from_this());
    
    if (!error)
    {
        forwarding_socket.async_write_some(boost::asio::buffer(data_from_inc_client_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
        {
            if (!error)
            {
                boost::asio::deadline_timer sleep_timer(io_service);
                sleep_timer.expires_from_now(boost::posix_time::milliseconds(delay_time));
                
                sleep_timer.async_wait([this, self](const boost::system::error_code& error)
                {
                    incoming_socket.async_read_some(boost::asio::buffer(data_from_inc_client_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
                    {
                            handle_inc_data_from_inc_client(error, length);
                    });
                });
            }
            else
                cleanup_on_error(error);
        });
    }
    else
        cleanup_on_error(error);
}


void connection::cleanup_on_error(const boost::system::error_code& error)
{
    if (incoming_socket.is_open())
        incoming_socket.close();

    if (forwarding_socket.is_open())
        forwarding_socket.close();
        
    std::cout << "Error: " << error.message() << "\n";
}


