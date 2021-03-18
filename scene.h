#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <stdio.h>
#include "vec4.h"
#include "PlyReader.h"

// Scene Struct definition
struct Scene
{

	PlyReader			model;					// Object in the scene
	vec4f				ambientReflection;		// Ambient reflection constant
	vec4f				diffuseReflection;		// Diffuse reflection constant
	vec4f				specularReflection;		// Specular reflection constant
	float				specularRoughness;		// Specular roughness or shininess constant for this material, which is larger for surfaces that are smoother and more mirror-like. When this constant is large the specular highlight is small.
	vec4f				ambientLight;			// Ambient light intensity
	std::vector<vec4f>	lightDirections;		// Vector of light directions
	std::vector<vec4f>	lightColors;			// Vector of light intensities -- used for both Specular and Diffuse light 
	vec4f				cameraLocation;			// Location of the camera
	vec4f				cameraLookAt;			// Point that the camera is looking at
	vec4f				cameraUp;				// Vector that defines "up" for the camera
	float				cameraFOV;				// Camera Field of View (how wide the camera is) -- assuming a square camera


	/////////////////
	// Function     : read() - reads a scene desciption file
	// Parameters   : char const * filename		Name of the file to read
	//
	// Return Value : Returns true if scene is read correctly, false otherwise
	/////////////////
	bool read(char const * filename);


	/////////////////
	// Function     : clear() - clears out a scene instance 
	// Return Value : Returns true if scene is cleared, and false otherwise
	/////////////////
	bool clear();

};


#endif
