#method 
By cases
1. Several cases for horizontal lines (no need to split) over [[thresh]].  Still keeps track of slightly longer side in [[longIsLeft]]
2. No horizontal line
	1. Splits into two general cases depending on long side
	2. splits into two triangles

uses [[swap]] to reduce all horizontal line cases to two (standardization)