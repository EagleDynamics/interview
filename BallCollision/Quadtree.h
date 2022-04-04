#pragma once

#include "Ball.h"

#include <list>
#include <array>
#include <memory>


namespace Collision
{
	/// <summary>
	/// Represents quadtree block
	/// </summary>
	struct Rectangle
	{
		float x;
		float y;
		float width;
		float height;
	};

	/// <summary>
	/// Quadtree that stores blocks of balls.
	/// TODO: change it so it could store other collidable stuff
	/// </summary>
	class Quadtree
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="level">Node level</param>
		/// <param name="bounds">Block bounds</param>
		Quadtree(int level, Rectangle bounds)
			: m_level(level)
			, m_bounds(bounds)
		{
		}

		/// <summary>
		/// Clears the tree
		/// </summary>
		void Clear();

		/// <summary>
		/// Insert the object into the quadtree. If the node
		/// exceeds the capacity, it will split and add all
		/// objects to their corresponding nodes.
		/// </summary>
		/// <param name="ball">Ball to insert</param>
		void Insert(const Ball& ball);

		/// <summary>
		/// Typedef for list of balls.
		/// </summary>
		using BallsList = std::list<std::shared_ptr<Ball>>;

		/// <summary>
		/// Return all objects that could collide with the given object.
		/// </summary>
		/// <param name="returnObjects"></param>
		/// <param name="ball"></param>
		void Retrieve(BallsList& returnObjects, const Ball& ball);

	private:
		/// <summary>
		/// Splits block into 4 smaller ones.
		/// </summary>
		void split();

		/// <summary>
		/// Determine which node the object belongs to.
		/// -1 means object cannot completely fit withing a child node and is part
		/// of the parent node.
		/// </summary>
		/// <param name="ball">Ball to check</param>
		/// <returns>Index of the node</returns>
		int getIndex(const Ball& ball);

	private:
		const int MAX_OBJECTS{ 10 };
		const int MAX_LEVELS{ 5 };

		int m_level;
		BallsList m_objects;
		Rectangle m_bounds;
		std::array<std::unique_ptr<Quadtree>, 4> m_nodes;
	};
}