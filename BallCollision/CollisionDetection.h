#pragma once

#include "Ball.h"
#include "Quadtree.h"

#include <vector>
#include <list>


namespace Collision
{
	inline float DoCirclesOverlap(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2);
	};

	/// <summary>
	/// Searches for balls that could collide with each other and works out related math.
	/// </summary>
	/// <param name="balls">Collection of balls</param>
	/// <param name="quad">Reusable quadtree</param>
	void CollideBalls(std::vector<Ball>& balls, Quadtree& quad);

	/// <summary>
	/// Searches for balls that could collide with borders and works out related math.
	/// </summary>
	/// <param name="balls">Collection of balls</param>
	/// <param name="WINDOW_X">Window width</param>
	/// <param name="WINDOW_Y">Window height</param>
	void CollideBallsWithBorders(std::vector<Ball>& balls, int WINDOW_X, int WINDOW_Y);
}