#ifndef CONNECTION_HPP
#define CONNECTION_HPP 

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <utility>
#include <unordered_map>

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	explicit Connection(boost::asio::io_service& io_s);	
	boost::asio::ip::tcp::socket& socket(); 
	void wait_for_query();
private:
	void get_rows(const float latitude, const float longitude);
	void write();
	boost::asio::ip::tcp::socket socket_;
	boost::asio::streambuf buf_;
};
#endif

