#include "server.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	try
	{
		std::string ip = argc < 2 ? "127.0.0.1" : argv[1];
		std::size_t port = argc < 3 ? 2018 : std::atoi(argv[2]);
		boost::asio::io_service io_s;
		Server server(io_s, ip, port);
	}
	catch(std::exception& e) { std::cerr << e.what() << '\n'; }
}

