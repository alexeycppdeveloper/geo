#ifndef GEO_PARSER_HPP
#define GEO_PARSER_HPP

#include "geo.hpp"
#include "hash_value.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <vector>
#include <string>
#include <exception>
#include <unordered_map>
#include <algorithm>
#include <fstream>

#include <iostream>
#include <iomanip>


namespace geo_parser{


using Hashvalues = std::vector<Hashvalue>;
using Hashmap = std::unordered_map<int, Hashvalues>; 

void insert(int key, 
			const Hashvalue& value, 
			Hashmap& geohash)
{
	auto it = geohash.find(key);
	if(it == geohash.end()) 
	{	
		geohash.insert({key, std::vector<Hashvalue>{value}});
	}
	else
	{
		it->second.emplace_back(value);
	}
}

double get_latitude_radius(double latitude, double longitude)
{
	for(double lat = 0.0000; ; lat += 0.0001)
	{
		if(geo::threshold_overpassed(latitude, longitude, latitude+lat, longitude)) 
		{
			return lat - 0.0001;
		}	
	}
	throw std::runtime_error("coudn't find point's latitude radius");	
}

double get_longitude_radius(double latitude, double longitude)
{
	for(double lon = 0.0000; ; lon += 0.0001)
	{
		if(geo::threshold_overpassed(latitude, longitude, latitude, longitude+lon)) 
		{
			return lon - 0.0001;
		}
	}
	throw std::runtime_error("coudn't find point's longitude radius");	
}


void populate_hash(
	double latitude, 
	double longitude, 
	std::string town,
	std::unordered_map<int, Hashvalues>& geohash,
	double& max_longitude_radius)

{

	double current_longitude_radius = get_longitude_radius(longitude, latitude);
	
	max_longitude_radius = std::max(max_longitude_radius, current_longitude_radius);
	
	int latitude_value = static_cast<int>(latitude * 10000); 
	int longitude_radius = static_cast<int>(current_longitude_radius*10000); 
	int latitude_radius = static_cast<int>(get_latitude_radius(longitude, latitude)*10000);  

	int key = static_cast<int>(longitude*10000);
	Hashvalue value{latitude_value, latitude_radius, longitude_radius, town};
	
	insert(key,value, geohash);
}

bool parse(const std::string& line, double& latitude, double& longitude, std::string& town)
{	
		auto expr = boost::spirit::qi::double_ [([&latitude](double d){latitude = d;})]
		>> 
		boost::spirit::qi::double_ [([&longitude](double d){longitude = d;})] 
		>> 
		boost::spirit::qi::lexeme[*boost::spirit::qi::char_ - ' ']
		[([&town](std::vector<char> v){for(auto ch: v) town+=ch; })];
	
	return boost::spirit::qi::phrase_parse(line.begin(), line.end(), expr, boost::spirit::qi::space);
}

class Geoinfo
{

public:
	explicit Geoinfo(const Hashmap& hashmap, int max_radius)
	: geohash(hashmap)
	, max_radius(max_radius) 
	{
	}
	const Hashmap& gethash() const {return geohash;}
	int getmaxradius()       const {return max_radius;}
private:
	Hashmap geohash;
	int max_radius;
};

Geoinfo
make_geohash()
{
	Hashmap geohash;
	double max_radius = -0.1;
	
	std::ifstream infile{"geo.txt"};
	if(!infile) 
	{ 
		std::cerr << "couldn't open geo file\n"; 
		std::terminate();
	}
	std::size_t row{};
	for(std::string line; std::getline(infile, line);)
	{
		++row;
		std::cout << std::setprecision(4) << row*100/54795.0 << "% loaded" <<'\n';	
		
		double latitude{}, longitude{};
		std::string town{};
		if(parse(line, latitude, longitude, town))
		{
			populate_hash(latitude, longitude, town, geohash, max_radius);
		}
	}
	std::cout << "geohash loaded\n";
	return Geoinfo(geohash, static_cast<int>(max_radius*10000));
}


const Geoinfo&
makeinfo()
{
	static Geoinfo geoinfo = make_geohash();
	return geoinfo;
}

}
#endif
