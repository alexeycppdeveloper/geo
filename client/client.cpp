#include <boost/asio.hpp>
#include <iostream>
#include <string>

class Client
{
public:
	Client(
		   const std::string& raw_ip_address, 
		   unsigned short port_num) 
	: 
	  ep_(boost::asio::ip::address::from_string(raw_ip_address), port_num)
	, sock_(ios_)
	{
		sock_.open(ep_.protocol());
		sock_.connect(ep_);
	}
	
	
	void close()
	{
		sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		sock_.close();
	}

	void send_request(const std::string& latitude, const std::string& longitude)
	{
		std::string request = latitude + "," + longitude + "\n"; 
		do_send_request(request);
	}

	std::string get_response()
	{
		boost::asio::streambuf buf;
		boost::asio::read_until(sock_, buf, '\n');
		std::istream input(&buf);
		std::string response;
		std::getline(input, response);
		return response;
	}

private:
	void do_send_request(const std::string& request)
	{
		boost::asio::write(sock_, boost::asio::buffer(request));
	}

private:
	boost::asio::io_service ios_;
	boost::asio::ip::tcp::endpoint ep_;
	boost::asio::ip::tcp::socket sock_;
};

int main(int argc, char* argv[])
{
	
	while(true){
	try
	{
		if(argc < 3)
		{
			std::cerr << "Usage: <latitude> <longitude>\n";
			return -1;
		}
		std::string latitude = argv[1];
		std::string longitude = argv[2];
		const std::string ip = argc < 4 ? "127.0.0.1" : argv[3];
		const unsigned short port = argc < 5 ? 2018 : std::atoi(argv[4]);
		
		Client client(ip, port);
		client.send_request(latitude, longitude);
		std::string response = client.get_response();
		std::cout << "Response received: " << response << '\n';
		client.close();
		return 0;
	}
    catch(...)
	{
	}
	}
}

