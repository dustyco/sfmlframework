

/*
	Linear Algebra
	See README for usage info
	http://github.com/dustyco/linear_algebra
	
	Changes:
		2013-03-07, 0.1
			Switched vector-scalar division overloads to use std::numeric_limits<R>::is_iec559 instead
			Added vector bracket operator
*/


#pragma once

#include <iostream>
#include <limits>
#include <cmath>


namespace linear_algebra {
	
	// For static type comparison
	template <class T, class U>
	struct is_same { static const bool value = false; };
	template <class T>
	struct is_same<T, T> { static const bool value = true; };
	
	// Totally generic
	template <int N, class R>
	struct vec {
		static vec origin ();
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
		R a[N];
	};
	
	// 2D specialization
	template <class R>
	struct vec<2,R> {
		vec () {};
		vec (R x, R y) : x(x), y(y) {}
		
		static vec origin () { return vec(0,0); }
		static vec x_unit () { return vec(1,0); }
		static vec y_unit () { return vec(0,1); }
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
		union {
			R a[2];
			struct {
				R x;
				R y;
			};
		};
	};
	
	// 3D specialization
	template <class R>
	struct vec<3,R> {
		vec () {};
		vec (R x, R y, R z) : x(x), y(y), z(z) {}
		
		static vec origin () { return vec(0,0,0); }
		static vec x_unit () { return vec(1,0,0); }
		static vec y_unit () { return vec(0,1,0); }
		static vec z_unit () { return vec(0,0,1); }
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
		union {
			R a[3];
			struct {
				R x;
				R y;
				R z;
			};
		};
	};
	
	// Constant /////////////////////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> vec<N,R>::origin () {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = 0;
		return v;
	}
	
	// Vector addition ////////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator +  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,float> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] + rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator -  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,float> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] - rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator += (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] += rhs[i];
		return lhs;
	}
	template <int N, class R>
	vec<N,R> operator -= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] -= rhs[i];
		return lhs;
	}
	
	// Vector-vector multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] * rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator /  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] / rhs[i];
		return v;
	}
	template <int N, class R>
	vec<N,R> operator *= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] *= rhs[i];
	}
	template <int N, class R>
	vec<N,R> operator /= (vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) lhs[i] /= rhs[i];
	}
	
	// Vector-scalar multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const vec<N,R>& lhs, const R& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] * rhs;
		return v;
	}
	template <int N, class R>
	vec<N,R> operator /  (const vec<N,R>& lhs, const R& rhs) {
		vec<N,R> v;
		if (std::numeric_limits<R>::is_iec559 && N>1) {
			// Cache reciprocol to avoid multiple divisions
			R irhs = R(1)/rhs;
			for (int i=0; i<N; i++) v[i] = lhs[i]*irhs;
		} else {
			// Otherwise divide normally (integers or custom types)
			for (int i=0; i<N; i++) v[i] = lhs[i] / rhs;
		}
		return v;
	}
	template <int N, class R>
	vec<N,R> operator *= (vec<N,R>& lhs, const R& rhs) {
		for (int i=0; i<N; i++) lhs[i] *= rhs;
		return lhs;
	}
	template <int N, class R>
	vec<N,R> operator /= (vec<N,R>& lhs, const R& rhs) {
		if (std::numeric_limits<R>::is_iec559 && N>1) {
			// Cache reciprocol to avoid multiple divisions
			R irhs = R(1)/rhs;
			for (int i=0; i<N; i++) lhs[i] = lhs[i]*irhs;
		} else {
			// Otherwise divide normally (integers or custom types)
			for (int i=0; i<N; i++) lhs[i] /= rhs;
		}
		return lhs;
	}
	
	// Comparisons //////////////////////////////////////////////////////////////////////////////
	template <int N, class R> bool operator <  (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) <  magsquared(rhs); }
	template <int N, class R> bool operator <= (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) <= magsquared(rhs); }
	template <int N, class R> bool operator >  (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) >  magsquared(rhs); }
	template <int N, class R> bool operator >= (const vec<N,R>& lhs, const vec<N,R>& rhs) { return magsquared(lhs) >= magsquared(rhs); }
	template <int N, class R> bool operator == (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) if (lhs[i]!=rhs[i]) return false;
		return true;
	}
	template <int N, class R> bool operator != (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		for (int i=0; i<N; i++) if (lhs[i]!=rhs[i]) return true;
		return false;
	}
	
	// Stream insertion /////////////////////////////////////////////////////////////////////////
	template <int N, class R>
	std::ostream& operator<< (std::ostream& os, const vec<N,R>& v) {
		os << '(' << v[0];
		for (int i=1; i<N; i++)
			os << ", " << v[i];
		os << ')';
		return os;
	}
	
	// Utility functions /////////////////////////////////////////////////////////////////////////
	template <int N, class R> R        magsquared (const vec<N,R>& v) {
		R sum = 0;
		for (int i=0; i<N; i++) sum += v[i]*v[i];
		return sum;
	}
	template <int N, class R> R        mag        (const vec<N,R>& v) { return std::sqrt(magsquared(v)); }
	template <int N, class R> vec<N,R> unit       (const vec<N,R>& v) { return v / mag(v); }
	template <int N, class R> void     normalize  (vec<N,R>& v)       { v /= mag(v); }
	template <int N, class R> R        dot        (const vec<N,R>& v1, const vec<N,R>& v2) {
		R sum = 0;
		for (int i=0; i<N; i++) sum += v1[i]*v2[i];
		return sum;
	}
	template <int N, class R> float    angle      (const vec<N,R>& v1, const vec<N,R>& v2) {
		return std::acos(dot(unit(v1), unit(v2)));
	}
	// 3D only
	template <class R>        vec<3,R> cross      (const vec<3,R>& v1, const vec<3,R>& v2) {
		return vec<3,R>(
			v1.y*v2.z - v1.z*v2.y,
			v1.z*v2.x - v1.x*v2.z,
			v1.x*v2.y - v1.y*v2.x
		);
	}
	
}





namespace linear_algebra {
	
	// Totally generic
	template <int N, class R>
	struct mat {
		static mat identity ();
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
		R a[N][N];
	};
	
	// 2D specialization
	template <class R>
	struct mat<2,R> {
		mat () {}
		mat (R theta);
		
		R&       operator[] (unsigned int i)       { return a[i]; }
		const R& operator[] (unsigned int i) const { return a[i]; }
	
		R a[2][2];
	};
	
	// Construct 2D rotation matrix
	template <class R>
	mat<2,R>::mat (R theta) {
		R cos_theta = cos(theta);
		R sin_theta = sin(theta);
		a[0][0] = cos_theta; a[0][0] = -sin_theta;
		a[0][0] = sin_theta; a[0][0] =  cos_theta;
	}

/*	// Vector-vector multiplication /////////////////////////////////////////////////////////////////
	template <int N, class R>
	vec<N,R> operator *  (const vec<N,R>& lhs, const vec<N,R>& rhs) {
		vec<N,R> v;
		for (int i=0; i<N; i++) v[i] = lhs[i] * rhs[i];
		return v;
	}
*/
	// Stream insertion /////////////////////////////////////////////////////////////////////////
	/*
		[ 1, 0 ]
		[ 0, 1 ]
	*/
	template <int N, class R>
	std::ostream& operator<< (std::ostream& os, const mat<N,R>& m) {
		os << '(' << m[0][0];
		for (int i=1; i<N; i++)
			os << ", " << m[i][i];
		os << ')';
		return os;
	}
}
