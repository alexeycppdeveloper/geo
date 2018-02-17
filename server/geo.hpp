#ifndef GEO_HPP
#define GEO_HPP

#include <cmath>

namespace geo{

const long double pi = 3.1415926535897932384626433832795; 

inline long double deg2rad(long double deg)
{
    return (deg * pi / 180.0);
}

long double vincenty_distance(long double latitude1, long double longitude1, long double latitude2, long double longitude2)
{
	const long double earth_radius_m = 6372795.0;
    long double lat1 = deg2rad(latitude1);
    long double lon1 = deg2rad(longitude1);
    long double lat2 = deg2rad(latitude2);
    long double lon2 = deg2rad(longitude2);

    long double d_lon = std::abs(lon1 - lon2);

    long double a = std::pow(std::cos(lat2) * std::sin(d_lon), 2);

    long double b = std::cos(lat1) * std::sin(lat2);
    long double c = std::sin(lat1) * std::cos(lat2) * std::cos(d_lon);
    long double d = pow(b - c, 2);

    long double e = std::sqrt(a + d);

    long double f = std::sin(lat1) * std::sin(lat2);
    long double g = std::cos(lat1) * std::cos(lat2) * std::cos(d_lon);

    long double h = f + g;

    long double d_sigma = std::atan2(e, h);

    return earth_radius_m * d_sigma;
}


inline bool threshold_overpassed(const long double latitude1, const long double longitude1, const long double latitude2, const long double longitude2)
{
	const long double threshold = 50000.0;
	return vincenty_distance(latitude1, longitude1, latitude2, longitude2) > threshold;
}

}
#endif
