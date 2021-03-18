#include "PlyReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//include <iomanip>
//#include <algorithm>
#include <list>


using namespace std;

bool PlyReader::read(char const * filename)
{
	// only supports vertex properties (in order) x y z nx ny nz r g b a
	ifstream in(filename, ios::binary);
	if (!in) return false;
	clear();
	int numVertices;
	int numFaces;
	list<string> properties;

	while (true)
	{
		string test;
		if (!getline(in, test)) break;

		stringstream strin(test);
		string s;
		strin >> s;

		if (s == "end_header")
		{
			break;
		}
		else if (s == "element")
		{
			strin >> s;
			if (s == "vertex") {
				strin >> numVertices;
			}
			else if (s == "face")
			{
				strin >> numFaces;
			}
		}
	}

	// vertex loop
	for (int i = 0; i < numVertices; i++)
	{
		string test;
		if (!getline(in, test)) break;

		stringstream strin(test);
		vec4f v, n, c;

		strin >> v.x >> v.y >> v.z >> n.x >> n.y >> n.z >> c.x >> c.y >> c.z;	
		c /= 255.0;
		v.w = 1.0;
		n.w = 0.0;
		c.w = 1.0;

		vertex.push_back(v);
		normal.push_back(n);
		faceColor.push_back(c);
	}

	// face loop
	for (int i = 0; i < numFaces; i++)
	{
		string test;
		if (!getline(in, test)) break;

		stringstream strin(test);
		vector<int> f;
		int numVectorsOnThisFace;
		strin >> numVectorsOnThisFace;
		for (int i = 0; i < numVectorsOnThisFace; i++)
		{
			int temp;
			strin >> temp;
			f.push_back(temp);
		}
		face.push_back(f);
	}

	return true;
}

void PlyReader::clear()
{
	vertex.clear();
	uvw.clear();
	normal.clear();
	face.clear();
	faceColor.clear();
}