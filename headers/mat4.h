#ifndef __MAT4_H__
#define __MAT4_H__


#include <math.h>

//****************************************************************
//*	 template<T> mat4
//****************************************************************
//* Generic 4x4 matrix over any field.
//****************************************************************

template<typename T> struct vec4;

template<typename T>
struct mat4
{
	vec4<T> r0, r1, r2, r3;

	// Constructors
	mat4()																													{ }
	mat4(const vec4<T> &v0, const vec4<T> &v1, const vec4<T> &v2, const vec4<T> &v3): r0(v0), r1(v1), r2(v2), r3(v3)		{ }
	mat4(const T*e)																	: r0(e), r1(e+4), r2(e+8), r3(e+12)		{ }	// [NOTE] LAPack uses column-major data, which this is NOT
	template<typename S>
	mat4(const mat4<S> &m)															: r0(m.r0), r1(m.r1), r2(m.r2), r3(m.r3){ }
	mat4(const T a00, const T a01, const T a02, const T a03, const T a10, const T a11, const T a12, const T a13, const T a20, const T a21, const T a22, const T a23, const T a30, const T a31, const T a32, const T a33)
																					: r0(a00, a01, a02, a03), r1(a10, a11, a12, a13), r2(a20, a21, a22, a23), r3(a30, a31, a32, a33)	{ }

	vec4<T>& operator [] ( int i)														{ return (&r0)[i]; }
	const vec4<T>& operator [] ( int i) const											{ return (&r0)[i]; }

	T &operator() (int i, int j)														{ return (*this)[i][j]; }
	T  operator() (int i, int j) const													{ return (*this)[i][j]; }

	T* data()																			{ return r0; }	// [NOTE] LAPack uses column-major data, which this is NOT
	const T* data() const																{ return r0; }	// [NOTE] LAPack uses column-major data, which this is NOT

	// Assignment operators
	mat4<T>& operator  = (const mat4<T>& m)												{ r0 = m.r0; r1 = m.r1; r2 = m.r2; r3 = m.r3; return *this; }
	mat4<T>& operator += (const mat4<T>& m)												{ r0 += m.r0; r1 += m.r1; r2 += m.r2; r3 += m.r3; return *this; }
	mat4<T>& operator -= (const mat4<T>& m)												{ r0 -= m.r0; r1 -= m.r1; r2 -= m.r2; r3 -= m.r3; return *this; }

	mat4<T>& operator *= (const T d)													{ r0 *= d; r1 *= d; r2 *= d; r3 *= d; return *this; }
	mat4<T>& operator /= (const T d)													{ r0 /= d; r1 /= d; r2 /= d; r3 /= d; return *this; }

	// row/col accessors
	vec4<T> row(int r) const															{ return vec4<T>((&r0)[r]); }
	vec4<T> col(int c) const															{ return vec4<T>(r0[c], r1[c], r2[c], r3[c]); }

	// standard matrix operators
	mat4<T> operator - ()																{ return mat4<T>(-r0, -r1, -r2, -r3); }
	mat4<T> operator + (const mat4<T>& a)												{ return mat4<T>(r0 + a.r0, r1 + a.r1, r2 + a.r2, r3 + a.r3); }
	mat4<T> operator - (const mat4<T>& a)												{ return mat4<T>(r0 - a.r0, r1 - a.r1, r2 - a.r2, r3 - a.r3); }
	mat4<T> Ptmult(const mat4<T>& a)													{ return mat4<T>(r0.Ptmult(a.r0), r1.Ptmult(a.r1), r2.Ptmult(a.r2), r3.Ptmult(a.r3)); }
	mat4<T> operator * (const T d)														{ return mat4<T>(r0 * d, r1 * d, r2 * d, r3 * d); }
	mat4<T> operator / (const T d)														{ return mat4<T>(r0 / d, r1 / d, r2 / d, r3 / d); }

	// matrix multiply
	mat4<T> operator * (const mat4<T>& a)												{ return mat4<T>(r0 * a.col(0), r0 * a.col(1), r0 * a.col(2), r0 * a.col(3), r1 * a.col(0), r1 * a.col(1), r1 * a.col(2), r1 * a.col(3), r2 * a.col(0), r2 * a.col(1), r2 * a.col(2), r2 * a.col(3), r3 * a.col(0), r3 * a.col(1), r3 * a.col(2), r3 * a.col(3)); }

	int operator == (const mat4<T>& a)													{ return (r0 == a.r0) && (r1 == a.r1) && (r2 == a.r2) && (r3 == a.r3); }
	int operator != (const mat4<T>& a)													{ return (r0 != a.r0) || (r1 != a.r1) || (r2 != a.r2) || (r3 != a.r3); }

	void Swap(mat4<T>& a)																{ mat4<T> tmp(a); a = *this; *this = tmp; }

	// special functions
	void SetAll(T nt)																	{ r0.SetAll(nt); r1.SetAll(nt); r2.SetAll(nt); r3.SetAll(nt); }
	void Set(const vec4<T>& v0, const vec4<T>& v1, const vec4<T>& v2, const vec4<T>& v3){ r0 = v0; r1 = v1; r2 = v2; r3 = v3;}
	
	// transformation functions
	T Determinant()	const																{ return (T)0; }
	mat4<T> Transpose()		const														{ return mat4<T>(r0.x, r1.x, r2.x, r3.x, r0.y, r1.y, r2.y, r3.y, r0.z, r1.z, r2.z, r3.z, r0.w, r1.w, r2.w, r3.w); }
	mat4<T> Inverse()		const
	{
		// [FIXME] this does not invert a general 4x4 matrix, instead it inverts an affine transform
		mat3<T> rotationMatrix ;
		vec3<T> transVect ;

		for (int i = 0; i < 9; i++)
		{
			rotationMatrix[i/3][i%3] = (*this)[i/3][i%3] ;
		}

		transVect.x = r0[3] ;
		transVect.y = r1[3] ;
		transVect.z = r2[3] ;

		mat3<T> rotationInverse = rotationMatrix.Inverse() ;
		vec3<T> translationNeg = transVect * -1.0 ;

		translationNeg = rotationInverse * translationNeg ;

		mat4<T> m
			( rotationInverse[0][0], rotationInverse[0][1], rotationInverse[0][2], translationNeg[0],
			rotationInverse[1][0], rotationInverse[1][1], rotationInverse[1][2], translationNeg[1],
			rotationInverse[2][0], rotationInverse[2][1], rotationInverse[2][2], translationNeg[2],
			0.0,                   0.0,                   0.0,               1.0) ;

		return m ;		
	}

	vec4<T> operator * (const vec4<T>& v);						// linear transform

	void PrintFloat();											// print matrix i case T = float.. debug

	// statics
	static mat4<T> Identity();									// identity 2D
	

};


template<typename T>
mat4<T> operator * (const T d, const mat4<T>& a)										{ return a*d; }

template<typename T>
vec4<T> mat4<T>::operator * (const vec4<T>& v)
{
	vec4<T> av;
	av.x = r0*v;
	av.y = r1*v;
	av.z = r2*v;
	av.w = r3*v;
	return av;
}

template<typename T>
mat4<T> mat4<T>::Identity() {
	mat4<T> a;
	a.r0 = vec4<T>((T)1.0,0,0,0);
	a.r1 = vec4<T>(0,(T)1.0,0,0);
	a.r2 = vec4<T>(0,0,(T)1.0,0);
	a.r3 = vec4<T>(0,0,0,(T)1.0);
	return a;
}

template<typename T>
void mat4<T>::PrintFloat() {
	printf("\n");
	printf("[%.2f\t%.2f\t%.2f\t%.2f]\n", r0.x,  r0.y,  r0.z,  r0.w );
	printf("[%.2f\t%.2f\t%.2f\t%.2f]\n", r1.x,  r1.y,  r1.z,  r1.w );
	printf("[%.2f\t%.2f\t%.2f\t%.2f]\n", r2.x,  r2.y,  r2.z,  r2.w );
	printf("[%.2f\t%.2f\t%.2f\t%.2f]\n", r3.x,  r3.y,  r3.z,  r3.w );
}


// typedefs
typedef mat4<double> mat4d;
typedef mat4<float> mat4f;
typedef mat4<int> mat4i;


#endif
