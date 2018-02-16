#include "server.hpp"

#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <unordered_map>
#include <pthread.h>


Server::Server(
	boost::asio::io_service& io_s, 
	const std::string& ip, 
	std::size_t port) 
	: acceptor_(io_s, boost::asio::ip::tcp::endpoint(
		boost::asio::ip::address_v4::from_string(ip), port))
	, threadcount_(std::thread::hardware_concurrency())
	, threadpool_(threadcount_)
{
	listen();
	launch_threadpool(io_s);
}

Server::~Server(){ for(auto& th: threadpool_) if(th.joinable()) th.join(); }

void Server::listen()
{
	auto new_connection = std::make_shared<Connection>(acceptor_.get_io_service());
	
	acceptor_.async_accept(new_connection->socket(), 
		[this, new_connection](const boost::system::error_code& ec)
	{
		if (!ec)  { new_connection->wait_for_query(); }
		listen();
	});
}

void Server::launch_threadpool(boost::asio::io_service& io_s)
{
	for(std::size_t i = 0; i < threadcount_; ++i) 
	{
		threadpool_[i] = std::thread([&](){io_s.run();});
		set_thread_affinity(threadpool_[i], i);
	}
}

void Server::set_thread_affinity(std::thread& th, std::size_t i)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(i, &cpuset);
	pthread_setaffinity_np(th.native_handle(), sizeof(cpu_set_t), &cpuset);
}

