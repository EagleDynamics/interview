#include "CollisionDetection.h"


namespace Collision
{
	void CollideBalls(std::vector<Ball>& balls, Quadtree& quad)
	{
		std::vector<std::pair<Ball*, Ball*>> vecCollidingPairs;
		
		quad.Clear();
		for (const auto& ball : balls)
		{
			quad.Insert(ball);
		}
		
		Quadtree::BallsList returnedBalls;
		// Find collisions
		for (auto& ball : balls)
		{
			returnedBalls.clear();
			quad.Retrieve(returnedBalls, ball);
		
			for (auto& target : returnedBalls)
			{
				if (!DoCirclesOverlap(ball.p.x, ball.p.y, ball.r, target->p.x, target->p.y, target->r)) continue;

				// Distance between ball centers
				float fDistance = sqrtf((ball.p.x - target->p.x) * (ball.p.x - target->p.x) + (ball.p.y - target->p.y) * (ball.p.y - target->p.y));

				if (fDistance == 0.f) continue; // either no collision and it's just the same ball or balls spawned in each other

				// Collision has occured
				vecCollidingPairs.emplace_back(&ball, target.get());

				// Calculate displacement required
				float fOverlap = 0.1f * (fDistance - ball.r - target->r);

				// Displace current ball away from collision
				ball.p.x -= fOverlap * (ball.p.x - target->p.x) / fDistance;
				ball.p.y -= fOverlap * (ball.p.y - target->p.y) / fDistance;
			}
		}
		
		// Now work out collisions
		Ball* b1;
		Ball* b2;
		for (auto& c : vecCollidingPairs)
		{
			b1 = c.first;
			b2 = c.second;
		
			// Distance between balls
			float fDistance = sqrtf((b1->p.x - b2->p.x) * (b1->p.x - b2->p.x) + (b1->p.y - b2->p.y) * (b1->p.y - b2->p.y));
		
			// Normal
			float nx = (b2->p.x - b1->p.x) / fDistance;
			float ny = (b2->p.y - b1->p.y) / fDistance;
		
			// Tangent
			float tx = -ny;
			float ty = nx;
		
			// Dot Product Tangent
			float dpTan1 = b1->dir.x * tx + b1->dir.y * ty;
			float dpTan2 = b2->dir.x * tx + b2->dir.y * ty;
		
			// Dot Product Normal
			float dpNorm1 = b1->dir.x * nx + b1->dir.y * ny;
			float dpNorm2 = b2->dir.x * nx + b2->dir.y * ny;
		
			// Conservation of momentum in 1D
			float m1 = (dpNorm1 * (b1->r - b2->r) + 2.0f * b2->r * dpNorm2) / (b1->r + b2->r);
			float m2 = (dpNorm2 * (b2->r - b1->r) + 2.0f * b1->r * dpNorm1) / (b1->r + b2->r);
		
			// Update ball velocities
			b1->dir.x = tx * dpTan1 + nx * m1;
			b1->dir.y = ty * dpTan1 + ny * m1;
			b2->dir.x = tx * dpTan2 + nx * m2;
			b2->dir.y = ty * dpTan2 + ny * m2;
		}
	}

	/* NOTE: Yes, I do have quadtree implemented and I could reuse it for border collision
	*	but I don't see any performance difference with such amount of balls on my PC.
	*	Therefore, I decided to leave the simplest solution, since it is still okay in terms of complexity ( O(n) )
	*	and easiest to maintain. */
	void CollideBallsWithBorders(std::vector<Ball>& balls, int WINDOW_X, int WINDOW_Y)
	{
		for (auto& ball : balls)
		{
			// Left border collision
			if ((ball.p.x - ball.r) <= 0)
			{
				ball.dir.x = -ball.dir.x;
			}

			// Right border collision
			if ((ball.p.x + ball.r) >= WINDOW_X)
			{
				ball.dir.x = -ball.dir.x;
			}

			// Bottom border collision
			if ((ball.p.y - ball.r) <= 0)
			{
				ball.dir.y = -ball.dir.y;
			}

			// Top border collision
			if ((ball.p.y + ball.r) >= WINDOW_Y)
			{
				ball.dir.y = -ball.dir.y;
			}
		}
	}
}