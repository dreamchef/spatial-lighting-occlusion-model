#pragma once
#ifndef DRAW_H
#define DRAW_H

#include <windows.h>
#include <algorithm>
#include "scene.h"
#include "definitions.h"
#include "mat4.h"
#include <math.h>

// 2D Array Struct definition to hold float values.
#ifndef BUF2D
#define BUF2D
struct buf2d { // note:  look at z value to determine color

	int sizex;  // number of columns
	int sizey;  // number of rows

	float *buf; // data buffer

	/////////////////
	// Operator overload: [] - indexes the data buffer
	// Parameters       : int y   Row number
	//
	// Return Value		: Returns a pointer to the first value in row y
	// Usage            : variable[y][x] will return the value at (x,y)
	/////////////////
	float *operator [] (int y) 
	{
		return &buf[y * sizex]; 
	}


	/////////////////
	// Function   : Release() - Clears and deallocates a buf2d
	/////////////////
	void Release()
	{
		if (buf)
		{
			delete[] buf;
			buf = 0;
			sizex = sizey = 0;
		}
	}
	
	
	/////////////////
	// Function   : init() - Initializes a buf2D, allocates memory and sets starting value
	// Parameters : int nsizex		number of columns
	//              int nsizey		number of rows
	//              float value		starting value for every array location
	//
	// Usage:  variable[y][x] will return the value at (x,y)
	/////////////////
	void init(
		int nsizex,
		int nsizey,
		float value)
	{
		buf = 0;
		sizex = sizey = 0;

		if (nsizex && nsizey)
		{
			if ((nsizex == sizex) && (nsizey == sizey))
				return;

			Release();

			buf = new float[nsizex * nsizey];

			if (buf)
			{
				sizex = nsizex;
				sizey = nsizey;
			}

			for (int i = 0; i < sizex*sizey; i++)
			{
				buf[i] = value;
			}
		}
	}
};
#endif


/////////////////
// Function   : draw() - Render the scene
// Parameters : int width		number of columns
//              int height		number of rows
//              HDC &img		windows image buffer
//				Scene scene		information about the scene (see scene.h)
//
/////////////////
void draw(
	int width,
	int height,
	HDC &img,
	Scene scene);

#endif // DRAW_H 