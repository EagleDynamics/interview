#include "Quadtree.h"


namespace Collision
{
	void Quadtree::Clear()
	{
		m_objects.clear();

		for (auto& node : m_nodes)
		{
			if (node != nullptr)
			{
				node->Clear();
				node = nullptr;
			}
		}
	}

	void Quadtree::Insert(const Ball& ball)
	{
		if (m_nodes[0] != nullptr) {
			int index = getIndex(ball);

			if (index != -1) {
				m_nodes[index]->Insert(ball);

				return;
			}
		}

		m_objects.emplace_back(std::make_shared<Ball>(ball));

		if (m_objects.size() > MAX_OBJECTS && m_level < MAX_LEVELS)
		{
			if (m_nodes[0] == nullptr)
			{
				split();
			}

			int i = 0;
			while (i < m_objects.size())
			{
				auto it = m_objects.begin();
				std::advance(it, i);
				int index = getIndex(**it);
				if (index != -1)
				{
					m_nodes[index]->Insert(**it);
					m_objects.erase(it);
				}
				else
				{
					i++;
				}
			}
		}
	}

	void Quadtree::Retrieve(BallsList& returnObjects, const Ball& ball)
	{
		int index = getIndex(ball);
		if (index != -1 && m_nodes[0] != nullptr) {
			m_nodes[index]->Retrieve(returnObjects, ball);
		}

		for (auto& object : m_objects)
		{
			returnObjects.emplace_back(object);
		}
	}

	void Quadtree::split()
	{
		float subWidth = static_cast<int>(m_bounds.width / 2);
		float subHeight = static_cast<int>(m_bounds.height / 2);
		float x = m_bounds.x;
		float y = m_bounds.y;

		m_nodes[0] = std::make_unique<Quadtree>(m_level + 1, Rectangle{ x + subWidth,	y,				subWidth, subHeight });
		m_nodes[1] = std::make_unique<Quadtree>(m_level + 1, Rectangle{ x,				y,				subWidth, subHeight });
		m_nodes[2] = std::make_unique<Quadtree>(m_level + 1, Rectangle{ x,				y + subHeight,	subWidth, subHeight });
		m_nodes[3] = std::make_unique<Quadtree>(m_level + 1, Rectangle{ x + subWidth,	y + subHeight,	subWidth, subHeight });
	}

	int Quadtree::getIndex(const Ball& ball)
	{
		int index = -1;
		float verticalMidpoint = m_bounds.x + (m_bounds.width / 2);
		float horizontalMidpoint = m_bounds.y + (m_bounds.height / 2);

		// Object can completely fit within the top quadrants
		bool topQuadrant = (ball.p.y < horizontalMidpoint && ball.p.y + ball.r < horizontalMidpoint);
		// Object can completely fit within the bottom quadrants
		bool bottomQuadrant = (ball.p.y > horizontalMidpoint);

		// Object can completely fit within the left quadrants
		if (ball.p.x < verticalMidpoint && ball.p.x + ball.r < verticalMidpoint)
		{
			if (topQuadrant)
			{
				index = 1;
			}
			else if (bottomQuadrant)
			{
				index = 2;
			}
		}
		else if (ball.p.x > verticalMidpoint) // Object can completely fit within the right quadrants
		{
			if (topQuadrant)
			{
				index = 0;
			}
			else if (bottomQuadrant)
			{
				index = 3;
			}
		}

		return index;
	}
}