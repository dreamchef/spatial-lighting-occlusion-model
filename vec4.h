#ifndef __VEC4_H__
#define __VEC4_H__


#include <math.h>


//****************************************************************
//*	 template<T> vec4
//****************************************************************
//* Generic 4-vector over any field.								
//****************************************************************

template<typename T>
struct vec4
{
	T x, y, z, w;

	// constructors
	vec4()																									{ }
	vec4(const T x, const T y, const T z, const T w)	: x(x), y(y), z(z), w(w)							{ }
	vec4(const T*e)										: x(e[0]), y(e[1]), z(e[2]), w(e[3])				{ }

	template<typename S>
	vec4(const vec4<S> &v)								: x((T) v.x), y((T) v.y), z((T) v.z), w((T) v.w)	{ }

	// type cast operators
	operator const T * () const											{ return &x; }
	operator T * ()														{ return &x; }

	T& operator [] ( int i)												{ return (&x)[i]; }

	// assignment operators (all element-wise operations)
	vec4<T>& operator  = (const vec4<T>& v)								{ x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
	vec4<T>& operator  = (const T v)									{ x = v; y = v; z = v; w = v; return *this; }
	vec4<T>& operator += (const vec4<T>& v)								{ x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	vec4<T>& operator -= (const vec4<T>& v)								{ x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	vec4<T>& operator *= (const T d)									{ x *= d; y *= d; z *= d; w *= d; return *this; }
	vec4<T>& operator *= (const vec4<T>& v)								{ x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
	vec4<T>& operator /= (const T d)									{ x /= d; y /= d; z /= d; w /= d; return *this; }
	vec4<T>& operator /= (const vec4<T>& v)								{ x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

	// standard vector operators
	vec4<T> operator - () const											{ return vec4<T>(-x, -y, -z, -w); }
	vec4<T> operator + (const vec4<T>& a) const							{ return vec4<T>(x + a.x, y + a.y, z + a.z, w + a.w); }
	vec4<T> operator - (const vec4<T>& a) const							{ return vec4<T>(x - a.x, y - a.y, z - a.z, w - a.w); }
	vec4<T> Ptmult(const vec4<T>& a) const								{ return vec4<T>(x * a.x, y * a.y, z * a.z, w * a.w); }

	vec4<T> operator * (const T d) const								{ return vec4<T>(x*d, y*d, z*d, w*d); }
	vec4<T> operator / (const T d) const								{ return vec4<T>(x/d, y/d, z/d, w/d); }

	// vector dot product (cross product ill-defined in 4 dimensions)
	T operator * (const vec4<T>& a)	const								{ return (x * a.x + y * a.y + z * a.z + w * a.w); }
	T dot(const vec4<T> a) const										{ return (x * a.x + y * a.y + z * a.z); }
	vec4<T> operator ^ (const vec4<T>& a) const							{ return vec4<T>(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x, w); }

	// comparison operators
	int operator == (const vec4<T>& a) const							{ return (x == a.x) && (y == a.y) && (z == a.z) && (w == a.w); }
	int operator != (const vec4<T>& a) const							{ return !(*this == a); }

	// special functions
	void SetAll(T nt)													{ x = nt;  y = nt;  z = nt;  w = nt; }
	void Set(T nx, T ny, T nz, T nw)									{ x = nx;  y = ny;  z = nz;  w = nw; }
	void Set(T nx, T ny, T nz)											{ x = nx;  y = ny;  z = nz;  w = 1; }
	void SetV(T nx, T ny, T nz)											{ x = nx;  y = ny;  z = nz;  w = 0; }
	T Length2() const													{ return x*x + y*y + z*z + w*w; }
	T Length() const													{ return (T)(sqrt(x*x+y*y+z*z+w*w)); }
	T Norm2() const														{ return x*x + y*y + z*z + w*w; }
	T Norm() const														{ return (T)(sqrt(x*x+y*y+z*z+w*w)); }
	void Normalize(void)												{ *this /= (T) Norm(); }
	vec4<T> GetNormalized(void)											{ return *this / ((T) Norm()); }

	// max/min functions
	T Min()																{ T m = (x < w ? x : w); return (m < y ? (m < z ? m : z) : (y < z ? y : z)); }
	T Max()																{ T m = (x > w ? x : w); return (m > y ? (m > z ? m : z) : (y > z ? y : z)); }
	vec4<T> Min(const vec4<T>& a)										{ return vec4<T>(x < a.x ? x : a.x, y < a.y ? y : a.y, z < a.z ? z : a.z, w < a.w ? w : a.w); }
	vec4<T> Max(const vec4<T>& a)										{ return vec4<T>(x > a.x ? x : a.x, y > a.y ? y : a.y, z > a.z ? z : a.z, w > a.w ? w : a.w); }

	// transformation functions
	void Zero()															{ x = 0; y = 0; z = 0; w = 0; }




	void Swap(vec4<T>& a)												{ vec4<T> tmp(a); a = *this; *this = tmp; }
};

template<typename T>
vec4<T> operator * (const T d, const vec4<T>& a)						{ return a*d; }

template<typename T>
vec4<T> Normalize(const vec4<T> &v)										{ return v / (T) v.Norm(); }

template<typename T>
T Dist2(const vec4<T>& a, const vec4<T>& b)								{ vec4<T> c = a - b; return (c.x*c.x + c.y*c.y + c.z*c.z + c.w*c.w); }


// typedefs
typedef vec4<double> vec4d;
typedef vec4<float> vec4f;
typedef vec4<int> vec4i;

// handy compare functions for std::sort
template<typename T, int D> bool Vec3LessThan(vec4<T> const & a, vec4<T> const & b)		{ return a[D] < b[D]; }

#endif
