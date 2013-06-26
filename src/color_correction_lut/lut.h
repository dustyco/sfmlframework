

#include <cmath>
#include <stdint.h>
#include "linear_algebra.h"
using namespace hmath;
typedef vec<2,float>   Vec2f;
typedef vec<3,float>   Vec3f;
typedef vec<2,uint8_t> Vec2u8;
typedef vec<3,uint8_t> Vec3u8;


template <class Functor>
void makeLut (Functor& f, Vec3f* lut, int size)
{
	float period = 1.0 / (size-1);
	for (int z=0; z!=size; ++z)
	for (int y=0; y!=size; ++y)
	for (int x=0; x!=size; ++x)
	{
		*(lut++) = f(Vec3f(period*x, period*y, period*z));
	}
}

template <class T>
T min3 (const T& a, const T& b, const T& c) {
	return (b<=c)?((a<=b)?a:b):((a<=c)?a:c);
}
template <class T>
T max3 (const T& a, const T& b, const T& c) {
	return (b>=c)?((a>=b)?a:b):((a>=c)?a:c);
}
/*
Vec3f rgbToHsv (Vec3f rgb)
{
	Vec3f hsv;
	
	float rgb_min = min3(rgb.x, rgb.y, rgb.z);
	float rgb_max = max3(rgb.x, rgb.y, rgb.z);
	
	hsv.z = rgb_max;
	if (hsv.z == 0) {
		// Color is black
		hsv.x = hsv.y = 0;
		return hsv;
	}
	// Normalize value to 1
	rgb.x /= hsv.z;
	rgb.y /= hsv.z;
	rgb.z /= hsv.z;
	rgb_min = min3(rgb.x, rgb.y, rgb.z);
	rgb_max = max3(rgb.x, rgb.y, rgb.z);
	hsv.y = rgb_max - rgb_min;
	if (hsv.y == 0) {
		hsv.x = 0;
		return hsv;
	}
	
	// Normalize saturation to 1
	rgb.r = (rgb.r - rgb_min)/(rgb_max - rgb_min);
	rgb.g = (rgb.g - rgb_min)/(rgb_max - rgb_min);
	rgb.b = (rgb.b - rgb_min)/(rgb_max - rgb_min);
	rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
	rgb_max = MAX3(rgb.r, rgb.g, rgb.b);
	// Compute hue
	if (rgb_max == rgb.r) {
		hsv.hue = 0.0 + 60.0*(rgb.g - rgb.b);
		if (hsv.hue < 0.0) {
			hsv.hue += 360.0;
		}
	} else if (rgb_max == rgb.g) {
		hsv.hue = 120.0 + 60.0*(rgb.b - rgb.r);
	} else {
		// rgb_max == rgb.b
		hsv.hue = 240.0 + 60.0*(rgb.r - rgb.g);
	}
}
*/


// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
Vec3f rgbToHsv (Vec3f rgb)
{
	Vec3f hsv;
	float min, max, delta;
	min = min3( rgb.x, rgb.y, rgb.z );
	max = max3( rgb.x, rgb.y, rgb.z );
	hsv.z = max;				// v
	delta = max - min;
	if( max != 0 )
		hsv.y = delta / max;		// s
	else {
		// rgb.x = rgb.y = rgb.z = 0		// s = 0, v is undefined
		hsv.y = 0;
		hsv.x = -1;
		return hsv;
	}
	if( rgb.x == max )
		hsv.x = ( rgb.y - rgb.z ) / delta;		// between yellow & magenta
	else if( rgb.y == max )
		hsv.x = 2 + ( rgb.z - rgb.x ) / delta;	// between cyan & yellow
	else
		hsv.x = 4 + ( rgb.x - rgb.y ) / delta;	// between magenta & cyan
	hsv.x *= 60;				// degrees
	if( hsv.x < 0 )
		hsv.x += 360;
	return hsv;
}
Vec3f hsvToRgb (Vec3f hsv)
{
	Vec3f rgb;
	int i;
	float f, p, q, t;
	if( hsv.y == 0 ) {
		// achromatic (grey)
		rgb.x = rgb.y = rgb.z = hsv.z;
		return rgb;
	}
	hsv.x /= 60;			// sector 0 to 5
	i = floor( hsv.x );
	f = hsv.x - i;			// factorial part of hsv.x
	p = hsv.z * ( 1 - hsv.y );
	q = hsv.z * ( 1 - hsv.y * f );
	t = hsv.z * ( 1 - hsv.y * ( 1 - f ) );
	switch( i ) {
		case 0:
			rgb.x = hsv.z;
			rgb.y = t;
			rgb.z = p;
			break;
		case 1:
			rgb.x = q;
			rgb.y = hsv.z;
			rgb.z = p;
			break;
		case 2:
			rgb.x = p;
			rgb.y = hsv.z;
			rgb.z = t;
			break;
		case 3:
			rgb.x = p;
			rgb.y = q;
			rgb.z = hsv.z;
			break;
		case 4:
			rgb.x = t;
			rgb.y = p;
			rgb.z = hsv.z;
			break;
		default:		// case 5:
			rgb.x = hsv.z;
			rgb.y = p;
			rgb.z = q;
			break;
	}
	return rgb;
}




struct ColorTransform
{
	Vec3f operator () (const Vec3f& in)
	{
		
//		return Vec3f(sin(in.x*20), sin(in.y*3), sin(in.z*10));
//		return hsvToRgb(rgbToHsv(in) * Vec3f(1, 2, 1) + Vec3f(360, 0, 0));
		Vec3f hsv = rgbToHsv(in);
		hsv.x = fmod(hsv.x+0, 360);
		hsv.y = pow(hsv.y, 1.1);
		hsv.y *= 1.2;
//		hsv.y = 1;
//		hsv.z = 1;
		return hsvToRgb(hsv) * Vec3f(0, 0, 1);
	}
};


