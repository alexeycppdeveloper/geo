#include "connection.hpp"
#include "hash_parser.hpp"
#include "geo_parser.hpp"

#include <boost/asio.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>


template<typename Container>
std::pair<float, float> 
get_client_point(Container& cont)
{
	std::pair<float, float> point;
	std::string line( (std::istreambuf_iterator<char>(&cont)), std::istreambuf_iterator<char>() );
	if(!boost::spirit::qi::phrase_parse(
			line.cbegin(), 
		    line.cend(),
			boost::spirit::qi::float_ >> ','>>
			boost::spirit::qi::float_, 
			boost::spirit::qi::space,
			point))
	{throw std::runtime_error("invalid query");}
	return point;
}


Connection::Connection(boost::asio::io_service& io_s): socket_(io_s)
{}
	
boost::asio::ip::tcp::socket& 
Connection::socket() 
{ 
	return socket_; 
}

void
Connection::wait_for_query()
{
	auto sharedthis = shared_from_this();
	boost::asio::async_read_until(socket_, buf_, '\n',
	[sharedthis, this](const boost::system::error_code& ec, std::size_t sz)
	{
		if(!ec || ec == boost::asio::error::eof) 
		sharedthis->write();
	});
}


void 
Connection::write()
{
	try
	{
		const std::pair<float, float>& client_point = get_client_point(buf_);
		const auto& geohash = geo_parser::makeinfo().gethash();
		int max_radius = geo_parser::makeinfo().getmaxradius();
		
		std::string towns = hash_parser::get_towns(
			client_point.first, client_point.second, geohash, max_radius);
		
		if(towns.empty()) 
		{		
			boost::asio::write(socket_, boost::asio::buffer("not found\n"));
		}
		else
		{
			towns += "\n";
			boost::asio::write(socket_, boost::asio::buffer(towns));
		}
	}
	catch(...)
	{
	}
}

