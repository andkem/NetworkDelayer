#ifndef CONNECTION_H
#define CONNECTION_H
#include <memory>
#include <vector>
#include <tuple>

#include <boost/asio.hpp>

#define UPDATE_T_DIFF 1

using boost::asio::ip::tcp;

class connection : 
public std::enable_shared_from_this<connection>
{
public:
    connection(tcp::socket incoming_socket, boost::asio::io_service& io_service);
    ~connection();
    
    void start(std::string redirect_address, unsigned short redirect_port, unsigned long delay_time);
    
private:    
    void handle_inc_data_from_forward_host(const boost::system::error_code& error, std::size_t length);
    void handle_inc_data_from_inc_client(const boost::system::error_code& error, std::size_t length);
    void rx_tx_data(const boost::system::error_code& error, std::size_t length, char* buffer, tcp::socket& rx_socket, tcp::socket& tx_socket);
    
    void cleanup_on_error(const boost::system::error_code& error);
    
    tcp::socket incoming_socket;
    tcp::socket forwarding_socket;
    tcp::resolver resolver;
    
    boost::asio::io_service& io_service;
    
    enum { max_length = 1500 };
    
    char data_from_forward_host_buff[max_length];
    char data_from_inc_client_buff[max_length];
    
    unsigned long delay_time;
};

#endif