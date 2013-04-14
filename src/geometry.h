#pragma once


#include <list>
using namespace std;

#include "linear_algebra.h"
using namespace linear_algebra;


namespace geometry {
	
	
	// ND straight line between two points
	template <int N,class R>
	struct line {
		typedef vec<N,R> vec_type;
		line () {}
		line (const vec_type& a, const vec_type& b) : a(a), b(b) {}
		vec_type a, b;
	};
	
	// 2D circle: position, radius
	template <class R>
	struct circle {
		typedef vec<2,R> vec_type;
		circle () {}
		circle (const vec_type& p, const R& r) : p(p), r(r) {}
		vec_type p;
		R r;
	};
	
	// 2D axis-aligned rectangle, radii from center to perpendicular edges
	template <class R>
	struct rect_aa {
		typedef vec<2,R> vec_type;
		rect_aa () {}
		rect_aa (const vec_type& p, const R& rx, const R& ry) : p(p), rx(rx), ry(ry) {}
		vec_type p;
		R rx, ry;
	};
	
	// 2D rotatable rectangle, radii from center to perpendicular edges
	template <class R>
	struct rect {
		typedef vec<2,R> vec_type;
		rect () {}
		rect (const vec_type& p, const R& rx, const R& ry, const float& theta) : p(p), rx(rx), ry(ry), theta(theta) {}
		vec_type p;
		R rx, ry;
		float theta;
	};
	
	
	template <class R>
	bool touching (const circle<R>& a, const circle<R>& b) {
		return (a.p-b.p).mag() < a.r+b.r;
	}
	
	template <class R>
	bool touching (const rect_aa<R>& a, const rect_aa<R>& b) {
		return (a.p.x+a.rx)>(b.p.x-b.rx) && (a.p.x-a.rx)<(b.p.x+b.rx)
			&& (a.p.y+a.ry)>(b.p.y-b.ry) && (a.p.y-a.ry)<(b.p.y+b.ry);
	}
	
/*	template <class R>
	bool touching (const rect<R>& a, const rect<R>& b) {
		return (a.p.x+a.rx)>(b.p.x-b.rx) && (a.p.x-a.rx)<(b.p.x+b.rx)
			&& (a.p.y+a.ry)>(b.p.y-b.ry) && (a.p.y-a.ry)<(b.p.y+b.ry);
	}
*/	
}
