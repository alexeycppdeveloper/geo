#ifndef HASH_PARSER_HPP
#define HASH_PARSER_HPP

#include "hash_value.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <algorithm>


namespace hash_parser{

using Hashvalues = std::vector<Hashvalue>;
using Hashmap = std::unordered_map<int, Hashvalues>; 

std::string 
collect_towns(
	const Hashvalues& values,
	int walked_so_far,
	int client_latitude)
{
	std::string towns{};
	for(const auto& value: values)
	{
		if( walked_so_far <= value.longitude_radius && 
			std::abs(value.latitude - client_latitude) <= value.latitude_radius)
		{
			towns += value.town;
			towns += ",";
		}
	}
	return towns;
}

std::string
get_towns(
	float latitude, 
	float longitude, 
	const Hashmap& geohash,
	std::int32_t max_radius)
{
	int key =   static_cast<int>(longitude * 10000);	
	int client_latitude = static_cast<int>(latitude * 10000);
	std::string towns{};
	
	for(int i = 0; i < max_radius; ++i)
	{
		auto it = geohash.find(key+i);
		if(it != geohash.end()) 
		{
			towns += collect_towns(it->second, i, client_latitude); 
		}
		if(i == 0 && it != geohash.end()) {continue;}
		
		it = geohash.find(key-i);	
		if(it != geohash.end())
		{
			towns += collect_towns(it->second, i, client_latitude); 
		}
	}
	return towns;
}

}
#endif

