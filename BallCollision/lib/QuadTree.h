#ifndef INTERVIEW_BALLCOLLISION_QUADTREE_H_
#define INTERVIEW_BALLCOLLISION_QUADTREE_H_

#include <unordered_set>
#include "Ball.h"

class QuadTree {
 private:
  struct AABB {
    sf::Vector2f center_{0, 0};
    float half_of_length_ = 0;

    AABB() = default;
    AABB(float x, float y, float half_of_length) :
        center_(x, y), half_of_length_(half_of_length) {}

    bool contains(const Ball &ball) const;
    bool intersects(const AABB &other) const;
  };

  struct Node {
    std::array<std::unique_ptr<Node>, 4> children;
    AABB boundary;
    std::vector<uint32_t> balls_id;

    Node() = default;
    Node(const AABB &boundary) :
        boundary(boundary) {}

    void subdivide();
    bool insert(QuadTree *qt_ptr, uint32_t idx);
    void query(QuadTree *qt_ptr,
               const AABB &range,
               std::vector<uint32_t> &close_balls_id);
  };

  std::vector<uint32_t> query(const AABB &range);
 public:
  QuadTree(const std::vector<Ball> &balls,
           const AABB &boundary);
  std::vector<uint32_t> get_close_balls(const Ball &ball);

 private:
  std::unique_ptr<Node> root_{nullptr};
  std::vector<Ball> balls_;
  const uint32_t MAX_AABB_BALLS_COUNT = 3;
};

#endif //INTERVIEW_BALLCOLLISION_QUADTREE_H_
