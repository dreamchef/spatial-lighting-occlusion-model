//define SQUARE
//#define TRI
#define FLAT
//#define MODEL

#include "headers/draw.h"

struct Vertex {
	vec4f p;
	vec4f n;
	vec4f c;
};

struct ScanLine { // define a scanline
	vec4f	start;
	vec4f	end;
	vec4f	current;

	float	deltaX;
	float	deltaZ;

	vec4f currentColor;
	vec4f startColor;
	vec4f deltaColor;

	vec4f startNormal;
	vec4f currentNormal;
	vec4f deltaNormal;

}; // End Struct

std::vector<mat4f> matStack;
std::vector<mat4f> matNormalStack;

buf2d zBuf;
int imgWidth;
int imgHeight;

float thresh = 0.1;

std::vector<vec4f> rotatedLightDirs;


mat4f normMult(mat4f left, mat4f right)
{
	mat4f result;

	result = left * right;

	float K0, K1, K2;
	K0 = 1 / sqrt(pow(result[0][0], 2) + pow(result[0][1], 2) + pow(result[0][2], 2));
	K1 = 1 / sqrt(pow(result[1][0], 2) + pow(result[1][1], 2) + pow(result[1][2], 2));
	K2 = 1 / sqrt(pow(result[2][0], 2) + pow(result[2][1], 2) + pow(result[2][2], 2));


	result[0][0] = K0 * result[0][0]; result[0][1] = K0 * result[0][1]; result[0][2] = K0 * result[0][2]; result[3][0] = result[3][0];
	result[1][0] = K1 * result[1][0]; result[1][1] = K1 * result[1][1]; result[1][2] = K1 * result[1][2]; result[3][1] = result[3][1];
	result[2][0] = K2 * result[2][0]; result[2][1] = K2 * result[2][1]; result[2][2] = K2 * result[2][2]; result[3][2] = result[3][2];
	result[3][0] = 0.0; result[3][1] = 0.0; result[3][2] = 0.0; result[3][3] = 1.0;

	return result;
}

void swap(Vertex& left, Vertex& right)
{
	Vertex temp;
	temp = left;
	left = right;
	right = temp;
}

bool clip(Vertex& one, Vertex& two, Vertex& three)
{
	if ((one.p.x < 0 && two.p.x < 0 && three.p.x < 0) ||
		(one.p.x > imgWidth && two.p.x > imgWidth && three.p.x > imgWidth) ||
		(one.p.y < 0 && two.p.y < 0 && three.p.y < 0) ||
		(one.p.y > imgHeight && two.p.y > imgHeight && three.p.y > imgHeight))
		return true;
	return false;
}

bool sortTriangles(Vertex& one, Vertex& two, Vertex& three)
{
	bool longIsLeft = true;
	float deltaX;
	float deltaY;
	float slope;
	float yIntercept;

	// Check for a horizontal line -- morph all the cases to ether:
	//  1---2  or   1
	//    3       2---3

	// Case 1:
	//    3          3
	//  1---2  and 2---1
	if (abs (one.p.y - two.p.y) < thresh && one.p.y > three.p.y)
	{
		longIsLeft = false;
		swap(one, three);
		if (three.p.x < two.p.x)
		{
			swap(two, three);
		}// End if
	}
	// Case 2:
	//  1---2  and 2---1
	//    3          3
	else if (abs(one.p.y - two.p.y) < thresh && one.p.y < three.p.y)
	{
		if (one.p.x > two.p.x)
		{
			swap(one, two);
		}// End if
	}
	// Case 3:
	//    1          1
	//  3---2  and 2---3
	else if (abs(two.p.y - three.p.y) < thresh && one.p.y < three.p.y)
	{
		longIsLeft = false;
		if (three.p.x < two.p.x)
		{
			swap(two, three);
		}// End if
	}
	// Case 4:
	//  3---2  and 2---3
	//    1          1
	else if (abs(two.p.y - three.p.y) < thresh && one.p.y > three.p.y)
	{
		if (three.p.x < two.p.x)
		{
			swap(one, three);
		}
		else {
			swap(two, three);

			swap(one, three);
		}// End if	
	}
	// Case 5:
	//  3---1  and 1---3
	//    2          2	
	else if (abs(one.p.y - three.p.y) < thresh && one.p.y < two.p.y)
	{
		swap(two, three);
		if (one.p.x > two.p.x)
		{
			swap(one, two);
		}// End if
	}
	// Case 6:
	//    2          2
	//  3---1  and 1---3
	else if (abs(one.p.y - three.p.y) < thresh && one.p.y > two.p.y)
	{
		longIsLeft = false;
		swap(one, two);
		if (three.p.x < two.p.x)
		{
			swap(two, three);
		}// End if

		 // Start handling the general triangle cases -- catch verticle lines and set delta's to 0.
	}
	// Sort along Y-axis -- This ensures that there are only 2 cases of the general triangle:
	//  1		      1
	//     2   or   2
	//   3             3
	else
	{
		if (one.p.y > two.p.y)
		{
			swap(one, two);

		}// End If
		if (one.p.y > three.p.y)
		{
			swap(one, three);
		}// End If
		if (two.p.y > three.p.y)
		{
			swap(two, three);
		}// End If


		 // find where the horizontal line from 2 intersects 1-3
		 //   uses equation of a line y = mx + b
		deltaX = three.p.x - one.p.x;
		deltaY = (three.p.y - one.p.y);
		slope = deltaY / deltaX;
		yIntercept = three.p.y - slope * three.p.x;

		// Set whether or not the "long" edge is on the left 
		if (deltaX != 0.00)
		{
			float temp = (two.p.y - yIntercept) / slope;
			longIsLeft = temp < two.p.x;
		}
		// catch the horizontal line case
		else {
			longIsLeft = one.p.x < two.p.x;
		}// End If
	}// End If


	return longIsLeft;
}

mat4f setupRHRCameraMat(Scene& scene, int width, int height, mat4f& normalMat)
{
	mat4f Xsp, Xpc, Xcw, result;

	normalMat = normalMat.Identity();
	result = result.Identity();

	// set up transfrom Xsp (projection to screen)

	Xsp.SetAll(0.0);
	Xsp[0][0] = width / 2;	Xsp[0][1] = 0.0;		 Xsp[0][2] = 0.0;		Xsp[0][3] = width / 2;
	Xsp[1][0] = 0.0;		Xsp[1][1] = height / 2;  Xsp[1][2] = 0.0;		Xsp[1][3] = height / 2;
	Xsp[2][0] = 0.0;		Xsp[2][1] = 0.0;		 Xsp[2][2] = 1.0;		Xsp[2][3] = 0.0;
	Xsp[3][0] = 0.0;		Xsp[3][1] = 0.0;		 Xsp[3][2] = 0.0;		Xsp[3][3] = 1.0;

	// set up transform Xpc (camera to projection)_
	float aspect = width / height;
	float N = 0.01f;
	float F = 100.0f;
	float d = tan(scene.cameraFOV / 2.0f * (PI / 180.0f));
	float AR = aspect * d;


	Xpc.SetAll(0.0);
	Xpc[0][0] = 1.0 / AR;	Xpc[0][1] = 0.0;     Xpc[0][2] = 0.0;			     Xpc[0][3] = 0.0;
	Xpc[1][0] = 0.0;	    Xpc[1][1] = 1.0 / d; Xpc[1][2] = 0.0;			     Xpc[1][3] = 0.0;
	Xpc[2][0] = 0.0;	    Xpc[2][1] = 0.0;	 Xpc[2][2] = (N + F) / (F - N);  Xpc[2][3] = (2.0 * N * F) / (F - N);
	Xpc[3][0] = 0.0;	    Xpc[3][1] = 0.0;	 Xpc[3][2] = -1.0;		         Xpc[3][3] = 0.0;

	result = Xsp * Xpc;

	normalMat = normMult(Xpc, normalMat);

	// set up world to camera
	vec4f  z, y, x, worldUp, eye;

	worldUp = scene.cameraUp;
	worldUp.Normalize();

	eye = scene.cameraLocation;
	z = scene.cameraLocation - scene.cameraLookAt;
	z.Normalize();

	x = worldUp ^ z;
	x.Normalize();

	y = z ^ x; // worldUp -worldUp.dot(n) * n;
	y.Normalize();

	Xcw.SetAll(0.0);
	Xcw[0][0] = x[0]; Xcw[0][1] = x[1]; Xcw[0][2] = x[2]; Xcw[0][3] = -x.dot(scene.cameraLocation);
	Xcw[1][0] = y[0]; Xcw[1][1] = y[1]; Xcw[1][2] = y[2]; Xcw[1][3] = -y.dot(scene.cameraLocation);
	Xcw[2][0] = z[0]; Xcw[2][1] = z[1]; Xcw[2][2] = z[2]; Xcw[2][3] = -z.dot(scene.cameraLocation);
	Xcw[3][0] = 0.0;  Xcw[3][1] = 0.0;  Xcw[3][2] = 0.0;  Xcw[3][3] = 1.0;

	result = result * Xcw;
	normalMat = normMult(normalMat, Xcw);

	return result;
}

ScanLine setupScanLineY(Vertex from, Vertex to)
{
	ScanLine result;
	// Scanline from one to three (the long edge)
	result.current = from.p;
	result.start = from.p;
	result.end = to.p;

	result.startColor = from.c;
	result.currentColor = from.c;

	result.startNormal = from.n;
	result.currentNormal = from.n;

	// Catch the horizontal line cases
	if (to.p.y - from.p.y != 0.0)
	{
		result.deltaX = (to.p.x - from.p.x) / (to.p.y - from.p.y);
		result.deltaZ = (to.p.z - from.p.z) / (to.p.y - from.p.y);
		result.deltaColor = (to.c - from.c) / (to.p.y - from.p.y);
		result.deltaNormal = (to.n - from.n) / (to.p.y - from.p.y);
	}
	else {
		result.deltaX = 0.0;
		result.deltaZ = 0.0;
		result.deltaColor = 0.0;
		result.deltaNormal = 0.0;
	}// End If

	return result;
}

ScanLine setupScanLineX(ScanLine left, ScanLine right)
{
	ScanLine result;

	result.current = left.current;
	result.start = left.current;
	result.end = right.current;
	result.currentColor = left.currentColor;
	result.currentNormal = left.currentNormal;

	// Catch the cases where there is no change in Z
	if (right.current.z == left.current.z)
	{
		result.deltaZ = 0.0;
	}
	else {
		result.deltaZ = (right.current.z - left.current.z) / (right.current.x - left.current.x);
	}// End if

	result.deltaColor = (right.currentColor - left.currentColor) / (right.current.x - left.current.x);
	result.deltaNormal = (right.currentNormal - left.currentNormal) / (right.current.x - left.current.x);

	return result;
}

void fillScanLineFlat(ScanLine leftRight, vec4f color, HDC& img)
{

	// Scan along the x pixel line, include the last one if it falls on the line.
	while (leftRight.current.x <= (int)leftRight.end.x)
	{

		int x, y;
		//float r, g, b, a;
		float z;

		x = static_cast<int> (leftRight.current.x);
		y = static_cast<int> (leftRight.current.y);

		vec4f tempColor = leftRight.currentColor;


		if (x > 0 && x < imgWidth && y > 0 && y < imgHeight)
		{
			z = zBuf[y][x];

			// If the current triangle is closer then set the right color
			//if (z > leftRight.current.z) // LHR			
			if (z < leftRight.current.z) // RHR			
			{

				tempColor *= 255;

				tempColor.x = (tempColor.x < 0 ? 0 : (tempColor.x > MAX_COLOR_VALUE ? MAX_COLOR_VALUE : tempColor.x));
				tempColor.y = (tempColor.y < 0 ? 0 : (tempColor.y > MAX_COLOR_VALUE ? MAX_COLOR_VALUE : tempColor.y));
				tempColor.z = (tempColor.z < 0 ? 0 : (tempColor.z > MAX_COLOR_VALUE ? MAX_COLOR_VALUE : tempColor.z));

				int r = (int)tempColor.x;
				int g = (int)tempColor.y;
				int b = (int)tempColor.z;

				SetPixelV(img, x, imgWidth - y, RGB(r, g, b));
				zBuf[y][x] = leftRight.current.z;
			}// End If
		}

		// Advance to the next pixel and interpolate Z
		leftRight.current.x += 1.0;
		leftRight.current.z += leftRight.deltaZ;
		leftRight.currentColor += leftRight.deltaColor;
		leftRight.currentNormal += leftRight.deltaNormal;
		leftRight.currentNormal.Normalize();

	}// End Loop
}

void draw(int width, int height, HDC& img, Scene scene)
{
	mat4f transMat, normTransMat;
	transMat = setupRHRCameraMat(scene, width, height, normTransMat);

	for (int i = 0; i < scene.lightDirections.size(); i++)
	{
		vec4f temp = normTransMat * scene.lightDirections[i];
		rotatedLightDirs.push_back(temp);
	}

	zBuf.init(width, height, -1e25f);  //RHR
	imgHeight = height;
	imgWidth = width;

	Vertex vOne, vTwo, vThree;
	int count = 0;
	for (int i = 0; i < scene.model.face.size(); i++)
	{
		vOne.p = scene.model.vertex[scene.model.face[i][0]];
		vTwo.p = scene.model.vertex[scene.model.face[i][1]];
		vThree.p = scene.model.vertex[scene.model.face[i][2]];

		vOne.p = transMat * vOne.p;
		vOne.p /= vOne.p.w;

		vTwo.p = transMat * vTwo.p;
		vTwo.p /= vTwo.p.w;

		vThree.p = transMat * vThree.p;
		vThree.p /= vThree.p.w;

		vOne.c = 0.75;
		vTwo.c = 0.75;
		vThree.c = 0.75;

		vOne.c = scene.model.faceColor[scene.model.face[i][0]];
		vOne.c.w = 1.0;

		vTwo.c = scene.model.faceColor[scene.model.face[i][1]];
		vTwo.c.w = 1.0;

		vThree.c = scene.model.faceColor[scene.model.face[i][2]];
		vThree.c.w = 1.0;

		bool longIsLeft = sortTriangles(vOne, vTwo, vThree);

		ScanLine oneTwo, oneThree, twoThree;

		// Scanline from one to three (the long edge)
		oneThree = setupScanLineY(vOne, vThree);

		// Scanline from one to two 
		oneTwo = setupScanLineY(vOne, vTwo);

		// Scanline from two to three 
		twoThree = setupScanLineY(vTwo, vThree);

		// Advance 1-3 and 1-2 to first pixel inside the triangle
		//deltaY = ceil(one.p.y) - one.p.y;

		// Interpolate the x and z values make sure we only do right and bottom values.
		//if (deltaY > 0.0)
		//{
		//	oneThree.current.x += oneThree.deltaX * deltaY;
		//	oneThree.current.y += deltaY;
		//	oneThree.current.z += oneThree.deltaZ * deltaY;
		//	oneThree.currentColor += oneThree.deltaColor * deltaY;
		//	oneThree.currentNormal += oneThree.deltaNormal * deltaY;
		//	oneThree.currentNormal.Normalize();

		//	oneTwo.current.x += oneTwo.deltaX * deltaY;
		//	oneTwo.current.y += deltaY;
		//	oneTwo.current.z += oneTwo.deltaZ * deltaY;
		//	oneTwo.currentColor += oneTwo.deltaColor * deltaY;
		//	oneTwo.currentNormal += oneTwo.deltaNormal * deltaY;
		//	oneTwo.currentNormal.Normalize();
		//}
		//else {
		//	oneThree.current.x += oneThree.deltaX;
		//	oneThree.current.y += 1.0;
		//	oneThree.current.z += oneThree.deltaZ;
		//	oneThree.currentColor += oneThree.deltaColor;
		//	oneThree.currentNormal += oneThree.deltaNormal;
		//	oneThree.currentNormal.Normalize();

		//	oneTwo.current.x += oneTwo.deltaX;
		//	oneTwo.current.y += 1.0;
		//	oneTwo.current.z += oneTwo.deltaZ;s
		//	oneTwo.currentColor += oneTwo.deltaColor;
		//	oneTwo.currentNormal += oneTwo.deltaNormal;
		//	oneTwo.currentNormal.Normalize();
		//}

		//Set pixels for the 1-2 edge -- including bottom and right edges
		while (oneTwo.current.y <= (int) oneTwo.end.y)
		{
			// Set up the left-right scanline
			ScanLine leftRight;

			// The long side is on the left
			if (longIsLeft)
			{
				leftRight = setupScanLineX(oneThree, oneTwo);
			}
			// The long is on the right
			else {
				leftRight = setupScanLineX(oneTwo, oneThree);
			}// End If

			fillScanLineFlat(leftRight, vOne.c, img);

			// Advance to the next scanline and interpolate x and z
			oneThree.current.x += oneThree.deltaX;
			oneThree.current.y += 1.0;
			oneThree.current.z += oneThree.deltaZ;
			oneThree.currentColor += oneThree.deltaColor;
			oneThree.currentNormal += oneThree.deltaNormal;
			oneThree.currentNormal.Normalize();

			oneTwo.current.x += oneTwo.deltaX;
			oneTwo.current.y += 1.0;
			oneTwo.current.z += oneTwo.deltaZ;
			oneTwo.currentColor += oneTwo.deltaColor;
			oneTwo.currentNormal += oneTwo.deltaNormal;
			oneTwo.currentNormal.Normalize();


		}// End Loop

		/*deltaY = ceil(vTwo.p.y) - vTwo.p.y;
		twoThree.current.x += twoThree.deltaX * deltaY;
		twoThree.current.y += deltaY;
		twoThree.current.z += twoThree.deltaZ * deltaY;
		twoThree.currentColor += twoThree.deltaColor * deltaY;
		twoThree.currentNormal += twoThree.deltaNormal * deltaY;
		twoThree.currentNormal.Normalize();*/


		//Set pixels for the 2-3 edge -- including bottom and right edges
		while (twoThree.current.y <= (int) twoThree.end.y)
		{
			// Set up the left-right scanline
			ScanLine leftRight;
			// The long side is on the left
			if (longIsLeft)
			{
				leftRight = setupScanLineX(oneThree, twoThree);
			}
			// The long side is on the right
			else {
				leftRight = setupScanLineX(twoThree, oneThree);
			}// End If


			fillScanLineFlat(leftRight, vOne.c, img);

			// Advance to the next scanline and interpolate x and z
			oneThree.current.x += oneThree.deltaX;
			oneThree.current.y += 1.0;
			oneThree.current.z += oneThree.deltaZ;
			oneThree.currentColor += oneThree.deltaColor;
			oneThree.currentNormal += oneThree.deltaNormal;
			oneThree.currentNormal.Normalize();

			twoThree.current.x += twoThree.deltaX;
			twoThree.current.y += 1.0;
			twoThree.current.z += twoThree.deltaZ;
			twoThree.currentColor += twoThree.deltaColor;
			twoThree.currentNormal += twoThree.deltaNormal;
			twoThree.currentNormal.Normalize();

		}// End Loop
	}
	zBuf.Release();
}
