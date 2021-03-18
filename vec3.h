#ifndef __VEC3_H__
#define __VEC3_H__


#include <math.h>


//****************************************************************
//*	 template<T> vec3
//****************************************************************
//* Generic 3-vector over any field.								
//****************************************************************

template<typename T>
struct vec3
{
	T x, y, z;

	// constructors
	vec3()																									{ }
	vec3(const T x, const T y, const T z)				: x(x), y(y), z(z)									{ }
	vec3(const T*e)										: x(e[0]), y(e[1]), z(e[2])							{ }

	template<typename S>
	vec3(const vec3<S> &v)								: x((T) v.x), y((T) v.y), z((T) v.z)				{ }
	
	// type cast operators
	operator const T * () const											{ return &x; }
	operator T * ()														{ return &x; }

	T& operator [] ( int i)												{ return (&x)[i]; }

	// assignment operators (all element-wise operations)
	vec3<T>& operator  = (const vec3<T>& v)								{ x = v.x; y = v.y; z = v.z; return *this; }
	vec3<T>& operator  = (const T d)									{ x = y = z = d; return *this; }
	vec3<T>& operator += (const vec3<T>& v)								{ x += v.x; y += v.y; z += v.z; return *this; }
	vec3<T>& operator -= (const vec3<T>& v)								{ x -= v.x; y -= v.y; z -= v.z; return *this; }
	vec3<T>& operator *= (const T d)									{ x *= d; y *= d; z *= d; return *this; }
	vec3<T>& operator *= (const vec3<T>& v)								{ x *= v.x; y *= v.y; z *= v.z; return *this; }
	vec3<T>& operator /= (const T d)									{ x /= d; y /= d; z /= d; return *this; }
	vec3<T>& operator /= (const vec3<T>& v)								{ x /= v.x; y /= v.y; z /= v.z; return *this; }

	// standard vector operators
	vec3<T> operator - () const											{ return vec3<T>(-x, -y, -z); }
	vec3<T> operator + (const vec3<T>& a) const							{ return vec3<T>(x + a.x, y + a.y, z + a.z); }
	vec3<T> operator - (const vec3<T>& a) const							{ return vec3<T>(x - a.x, y - a.y, z - a.z); }
	vec3<T> Ptmult(const vec3<T>& a) const								{ return vec3<T>(x * a.x, y * a.y, z * a.z); }

	vec3<T> operator * (const T d) const								{ return vec3<T>(x*d, y*d, z*d); }
	vec3<T> operator / (const T d) const								{ return vec3<T>(x/d, y/d, z/d); }

	// vector dot product and cross product
	T operator * (const vec3<T>& a)	const								{ return (x * a.x + y * a.y + z * a.z); }
	vec3<T> operator ^ (const vec3<T>& a)	const						{ return vec3<T>(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x); }

	// comparison operators
	int operator == (const vec3<T>& a) const							{ return (x == a.x) && (y == a.y) && (z == a.z); }
	int operator != (const vec3<T>& a) const							{ return !(*this == a); }

	// special functions
	void SetAll(T nt)													{ x = nt;  y = nt;  z = nt; }
	void Set(T nx, T ny, T nz)											{ x = nx;  y = ny;  z = nz; }
	T Length2() const													{ return x*x + y*y + z*z; }
	T Length() const													{ return (T)(sqrt(x*x+y*y+z*z)); }
	T Norm2() const														{ return x*x + y*y + z*z; }
	T Norm() const														{ return (T)(sqrt(x*x+y*y+z*z)); }
	void Normalize(void)												{ *this /= (T) Norm(); }
	vec3<T> GetNormalized(void) const									{ return *this / ((T) Norm()); }

	// max/min functions
	T Min()																{ return (x < y ? (x < z ? x : z) : (y < z ? y : z)); }
	T Max()																{ return (x > y ? (x > z ? x : z) : (y > z ? y : z)); }
	vec3<T> Min(const vec3<T>& a)										{ return vec3<T>(x < a.x ? x : a.x, y < a.y ? y : a.y, z < a.z ? z : a.z); }
	vec3<T> Max(const vec3<T>& a)										{ return vec3<T>(x > a.x ? x : a.x, y > a.y ? y : a.y, z > a.z ? z : a.z); }

	// transformation functions
	void Zero()															{ x = 0; y = 0; z = 0; }
	vec3<T> RotateX(double rad) const									{ return vec3<T>(x, y * cos(rad) - z * sin(rad), z * cos(rad) + y * sin(rad)); }	// [NOTE] right-handed
	vec3<T> RotateY(double rad) const									{ return vec3<T>(x * cos(rad) + z * sin(rad), y, z * cos(rad) - x * sin(rad)); }	// [NOTE] right-handed
	vec3<T> RotateZ(double rad) const									{ return vec3<T>(x * cos(rad) - y * sin(rad), y * cos(rad) + x * sin(rad), z); }	// [NOTE] right-handed

	void Swap(vec3<T>& a)												{ vec3<T> tmp(a); a = *this; *this = tmp; }
	vec3<T> Dot(const vec3<T>& a, const vec3<T>& b)						{ return a*b;}
	vec3<T> Cross(const vec3<T>& a, const vec3<T>& b)					{ return a^b;}

	void PrintFloat() const												{ printf("[%.3f, %.3f, %.3f]", x, y, z); }

};
	
template<typename T>
vec3<T> operator * (const T d, const vec3<T>& a)						{ return a*d; }

template<typename T>
vec3<T> Normalize(const vec3<T> &v)										{ return v / (T) v.Norm(); }

template<typename T>
T Dist2(const vec3<T>& a, const vec3<T>& b)								{ vec3<T> c = a - b; return (c.x*c.x + c.y*c.y + c.z*c.z); }
template<typename T>
T Dist(const vec3<T>& a, const vec3<T>& b)								{ vec3<T> c = a - b; return (T)sqrt(c.x*c.x + c.y*c.y + c.z*c.z); }


// typedefs
typedef vec3<double> vec3d;
typedef vec3<float> vec3f;
typedef vec3<int> vec3i;

// handy compare functions for std::sort
template<typename T, int D> bool Vec3LessThan(vec3<T> const & a, vec3<T> const & b)		{ return a[D] < b[D]; }

#endif
