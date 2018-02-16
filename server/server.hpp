#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <thread>
#include <unordered_map>

#include "connection.hpp"

class Server
{
public:
	explicit Server(boost::asio::io_service& io_s, const std::string& ip, std::size_t port); 
	~Server();
private:
	void listen();
	void launch_threadpool(boost::asio::io_service& io_s);
	void set_thread_affinity(std::thread& th, std::size_t i);
	boost::asio::ip::tcp::acceptor acceptor_;
	std::size_t threadcount_;
	std::vector<std::thread> threadpool_;
};
#endif

