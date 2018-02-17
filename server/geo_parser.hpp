#ifndef GEO_PARSER_HPP
#define GEO_PARSER_HPP

#include "geo.hpp"
#include "hash_value.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <vector>
#include <string>
#include <exception>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <tuple>

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
	const std::tuple<double, double, std::string>& tpl,
	std::unordered_map<int, Hashvalues>& geohash,
	double& max_longitude_radius)

{
	double latitude = std::get<0>(tpl);
	double longitude = std::get<1>(tpl);
	const std::string& town = std::get<2>(tpl);
	
	double current_longitude_radius = get_longitude_radius(latitude, longitude);
	max_longitude_radius = std::max(max_longitude_radius, current_longitude_radius);
	
	int latitude_value = static_cast<int>(latitude * 10000); 
	int longitude_radius = static_cast<int>(current_longitude_radius*10000); 
	int latitude_radius = static_cast<int>(get_latitude_radius(latitude, longitude)*10000);  

	int key = static_cast<int>(longitude*10000);
	Hashvalue value{latitude_value, latitude_radius, longitude_radius, town};
	
	insert(key,value, geohash);
}


std::tuple<double, double, std::string>
parsed_line(const std::string& line)
{	
	std::tuple<double, double, std::string> t;
	if(!boost::spirit::qi::parse(
		line.begin(), line.end(), 
		+boost::spirit::qi::omit[' '] >>
		boost::spirit::qi::double_ >> +boost::spirit::qi::omit[' '] >> 
		boost::spirit::qi::double_ >> +boost::spirit::qi::omit[' '] >> 
		+(boost::spirit::qi::char_), 
		t))
	{
		std::cerr << "error while parsing file\n";
		std::terminate();
	}
	return  t;
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
	std::ifstream infile{"geo.txt"};
	if(!infile) 
	{ 
		throw std::runtime_error("couldn't open geo file"); 
	}
	
	Hashmap geohash;
	double max_radius = -0.1;
	std::size_t row{1};
	for(std::string line; std::getline(infile, line);++row)
	{
		std::cout << std::setprecision(4) << row*100/54911.0 << "% loaded" <<'\n';	
		
		populate_hash(parsed_line(line), geohash, max_radius);
	}
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
