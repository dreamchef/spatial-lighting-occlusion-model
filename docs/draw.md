#method

Create transformation matrices and normal transformation matrices

Initialize transformation matrix to 'to camera'

Calculate transformations for light direction normals and push them onto the stack for rotated light directions

loop through the triangles in the scene
1. grab each [[Vertex]] in the face;
2. apply world->camera->projection->screen transformation using [[setupRHRCameraMat]];
3. determine whether long side is on the left (true) or right, using [[sortTriangles]] and store in [[longIsLeft]];
4. create a [[Scanline]] for each side;
5. for each scanline in the first sub-triangle
	1. scanline bounds depend on [[longIsLeft]]
	2. uses [[setUpScanLineY]] and [[setupScanLineX]]
	3. then uses [[fillScanLineFlat]]
6. for each scanline in the second sub-triangle
	1. scanline bounds depend on [[longIsLeft]]
	2. uses [[setUpScanLineY]] and [[setupScanLineX]]
	3. then uses [[fillScanLineFlat]]

release z-buffer #question