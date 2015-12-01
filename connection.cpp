#include "connection.h"

#include <sstream>
#include <iostream>

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
            // Start the proxy -> client data handling.
            incoming_socket.async_read_some(boost::asio::buffer(data_from_inc_client_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
            {
                rx_tx_data(error, length, data_from_inc_client_buff, incoming_socket, forwarding_socket);
            });
            
            // Start the proxy -> server data handling.
            forwarding_socket.async_read_some(boost::asio::buffer(data_from_forward_host_buff, max_length), [this, self](const boost::system::error_code& error, std::size_t length)
            {
                rx_tx_data(error, length, data_from_forward_host_buff, forwarding_socket, incoming_socket);
            });
        }
        else
            cleanup_on_error(error);
    });
}

void connection::rx_tx_data(const boost::system::error_code& error, std::size_t length, char* buffer, tcp::socket& rx_socket, tcp::socket& tx_socket)
{
    auto self(shared_from_this());
    
    if (!error)
    {
        std::shared_ptr<boost::asio::deadline_timer> sleep_timer(new boost::asio::deadline_timer(io_service));
        sleep_timer->expires_from_now(boost::posix_time::microseconds(delay_time));
        
        // Copy the data from the buffer to keep it in memory until the send timer expires.
        char* buffer_cpy = copy_data(buffer, length);
        
        // Schedule the data for transmission after a delay of delay_time microseconds. This is done asynchronously to make sure
        // data is continously received while we wait to send the already received data.
        sleep_timer->async_wait([this, self, sleep_timer, buffer_cpy, length, &rx_socket, &tx_socket](const boost::system::error_code& error)
        {
            tx_socket.async_write_some(boost::asio::buffer(buffer_cpy, length), [this, self, buffer_cpy, &rx_socket, &tx_socket](const boost::system::error_code& error, std::size_t length)
            {
                // Manually clear the buffered data from memory since it has been sent at this point.
                delete[] buffer_cpy;
                
                if (error)
                    cleanup_on_error(error);
            });
        });
        
        // Read more data asynchronously and make the recursive callback to this function when data has been received.
        rx_socket.async_read_some(boost::asio::buffer(buffer, max_length), [this, self, buffer, &rx_socket, &tx_socket](const boost::system::error_code& error, std::size_t length)
        {
            rx_tx_data(error, length, buffer, rx_socket, tx_socket);
        });
    }
    else
        cleanup_on_error(error);
}

char* connection::copy_data(const char* data, std::size_t length)
{
    char* tmp = new char[length];
    
    memcpy(tmp, data, length);
    
    return tmp;
}

void connection::cleanup_on_error(const boost::system::error_code& error)
{
    if (incoming_socket.is_open())
        incoming_socket.close();

    if (forwarding_socket.is_open())
        forwarding_socket.close();
        
    std::cout << "Error: " << error.message() << "\n";
}


