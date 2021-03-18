#include "scene.h"

using namespace std;

/////////////////
// Function     : read() - reads a scene desciption file
// Parameters   : char const * filename		Name of the file to read
//
// Return Value : Returns true if scene is read correctly, false otherwise
/////////////////
bool Scene::read(char const * filename)
{
	// clear and intialize scene
	clear();

	FILE *fptr;
	char buff[255];
	char buff2[255];

	// open file
	fopen_s(&fptr, filename, "r");
	if (fptr == NULL)
		return false;

	float r, g, b, x, y, z;
	char code;
	vec4f lightLocation;
	vec4f lightColor;


	// initialize light values
	lightLocation = lightColor = 0.0; 

	// Read scene file one line at a time and record values -- note duplicate entries for some values will overwrite previously read information
	while (fgets(buff, 255, fptr) != NULL)
	{
		switch (buff[0])
		{
		case 'A':
			sscanf_s(buff, "%c %f %f %f", &code, (unsigned)sizeof(char), &r, &g, &b);
			ambientLight.Set(r, g, b);
			break;
		case 'L':
			sscanf_s(buff, "%c %f %f %f %f %f %f", &code, (unsigned)sizeof(char), &x, &y, &z, &r, &g, &b);
			lightLocation.Set(x, y, z, 0.0);
			lightLocation.Normalize();
			lightColor.Set(r, g, b);
			lightDirections.push_back(lightLocation);
			lightColors.push_back(lightColor);
			break;
		case 'c':
			sscanf_s(buff, "%c %f %f %f", &code, (unsigned)sizeof(char), &x, &y, &z);
			cameraLocation.Set(x, y, z, 1.0);
			break;
		case 'l':
			sscanf_s(buff, "%c %f %f %f", &code, (unsigned)sizeof(char), &x, &y, &z);
			cameraLookAt.Set(x, y, z, 1.0);
			break;
		case 'u':
			sscanf_s(buff, "%c %f %f %f", &code, (unsigned)sizeof(char), &x, &y, &z);
			cameraUp.Set(x, y, z, 0.0);
			break;
		case 'f':
			sscanf_s(buff, "%c %f", &code, (unsigned)sizeof(char), &cameraFOV);
			break;
		case 'm':
			sscanf_s(buff, "%c %s", &code, (unsigned)sizeof(char), buff2, (unsigned)_countof(buff2));
			if (!model.read(buff2))
				return false;
			break;
		case 'a':
			sscanf_s(buff, "%c %f %f %f", &code, (unsigned)sizeof(char), &x, &y, &z);
			ambientReflection.Set(x, y, z);
			break;
		case 'd':
			sscanf_s(buff, "%c %f %f %f", &code, (unsigned)sizeof(char), &x, &y, &z);
			diffuseReflection.Set(x, y, z);
			break;
		case 's':
			sscanf_s(buff, "%c %f %f %f", &code, (unsigned)sizeof(char), &x, &y, &z);
			specularReflection.Set(x, y, z);
			break;
		case 'r':
			sscanf_s(buff, "%c %f", &code, (unsigned)sizeof(char), &specularRoughness);
			break;
		default:
			break;
		}



	}
	fclose(fptr);
	return true;
}

/////////////////
// Function     : clear() - clears out a scene instance 
// Return Value : Returns true if scene is cleared, and false otherwise
/////////////////
bool Scene::clear()
{
	model.clear();
	lightDirections.clear();
	lightColors.clear();

	return true;
}