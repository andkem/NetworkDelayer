#include <iostream>
#include <string>
#include <stdexcept>

#include <boost/program_options.hpp>

#include "server.h"

namespace po = boost::program_options;
using namespace std;

void parse_command_line(int argc, char **argv, po::variables_map& vm)
{
    po::options_description desc("Command line");
    desc.add_options()
    ("help", "Print usage information.")
    ("lp", po::value<unsigned short>(), "The port on which to listen.")
    ("ra", po::value<string>(), "Address to redirect to.")
    ("rp", po::value<unsigned short>(), "Port to redirect to.")
    ("us", po::value<unsigned long>(), "The amount of µs to delay each byte of data.");

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    
    if (vm.count("help") || !vm.count("lp") || !vm.count("ra") 
        || !vm.count("rp") || !vm.count("us"))
    {
        cout << desc << "\n";
        throw invalid_argument("Missing parameters!");
    }
    
    server srv();
}

int main(int argc, char **argv)
{
    po::variables_map vm;
    
    try
    {
        parse_command_line(argc, argv, vm);
    }
    catch (const exception& e)
    {
        cout << "Error: " << e.what() << "\n";
        return 1;  
    }
    
    try
    {
        boost::asio::io_service io_service;
        server server(io_service, tcp::endpoint(tcp::v4(), vm["lp"].as<unsigned short>()), vm["ra"].as<string>(), vm["rp"].as<unsigned short>(), vm["us"].as<unsigned long>());
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
