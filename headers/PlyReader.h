#ifndef _MINIMALOBJ_H
#define _MINIMALOBJ_H

#include <vector>
#include "vec4.h"

struct PlyReader
{
	// [NOTE] not optimized for speed
	std::vector<vec4f> vertex;
	std::vector<vec4f> uvw;
	std::vector<vec4f> normal;
	std::vector<std::vector<int> > face;
	std::vector<vec4f> faceColor;	// Optional
	bool read(char const * filename);
	void clear();
};

#endif
