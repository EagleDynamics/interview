#ifndef INTERVIEW_BALLCOLLISION_QUADTREE_H_
#define INTERVIEW_BALLCOLLISION_QUADTREE_H_

#include <unordered_set>
#include "Ball.h"

class HeapQuadTree;

class BaseQuadTree {
 protected:
  struct AABB {
    sf::Vector2f center_{0, 0};
    float half_of_length_ = 0;

    AABB() = default;
    AABB(float x, float y, float half_of_length) :
        center_(x, y), half_of_length_(half_of_length) {}

    bool contains(const Ball &ball) const;
    bool intersects(const AABB &other) const;
  };

  struct BaseNode {
    AABB boundary;
    std::vector<uint32_t> balls_id;

    BaseNode() = default;
    BaseNode(const AABB &boundary) :
        boundary(boundary) {}

    virtual void subdivide() = 0;
    virtual bool insert(BaseQuadTree *qt_ptr, uint32_t idx) = 0;
    virtual void query(BaseQuadTree *qt_ptr,
                       const AABB &range,
                       std::vector<uint32_t> &close_balls_id) = 0;
  };

  virtual std::vector<uint32_t> query(const AABB &range);

 public:
  BaseQuadTree(const std::vector<Ball> &balls);
  std::vector<uint32_t> get_close_balls(const Ball &ball);

 protected:
  std::unique_ptr<BaseNode> root_{nullptr};
  std::vector<Ball> balls_;
  const uint32_t MAX_AABB_BALLS_COUNT = 3;


  friend class HeapQuadTree;
};

class HeapQuadTree : public BaseQuadTree {
 private:
  struct Node : BaseNode {
    std::array<std::unique_ptr<Node>, 4> children;

    void subdivide() override;
    bool insert(BaseQuadTree *qt_ptr, uint32_t idx) override;
    void query(BaseQuadTree *qt_ptr,
               const AABB &range,
               std::vector<uint32_t> &close_balls_id) override;

    Node(const AABB &boundary) :
        BaseNode(boundary) {}
  };

 public:
  HeapQuadTree(const std::vector<Ball> &balls,
               const AABB &boundary);
};

class StackQuadTree : public BaseQuadTree {
 private:
  struct Node : BaseNode {
    std::array<uint32_t, 4> children;

    void subdivide() override;
    bool insert(BaseQuadTree *qt_ptr, uint32_t idx) override;
    void query(BaseQuadTree *qt_ptr,
               const AABB &range,
               std::vector<uint32_t> &close_balls_id) override;

    Node(const AABB &boundary) :
        BaseNode(boundary) {}
  };

 private:
  std::vector<Node> nodes_;

 public:
  StackQuadTree(const std::vector<Ball> &balls,
               const AABB &boundary);
};

#endif //INTERVIEW_BALLCOLLISION_QUADTREE_H_
