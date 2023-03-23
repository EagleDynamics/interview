#include "QuadTree.h"

bool QuadTree::AABB::contains(const Ball &ball) const {
  auto ball_center = ball.get_center();
  float radius = ball.get_radius();
  if ((ball_center.x + radius >= center_.x - half_of_length_) &&
      (ball_center.x - radius <= center_.x + half_of_length_) &&
      (ball_center.y + radius >= center_.y - half_of_length_) &&
      (ball_center.y - radius <= center_.y + half_of_length_)) {
    return true;
  } else {
    return false;
  }
}

bool QuadTree::AABB::intersects(const AABB &other) const {
  if ((std::abs(center_.x - other.center_.x) <
      half_of_length_ + other.half_of_length_) &&
      (std::abs(center_.y - other.center_.y) <
          half_of_length_ + other.half_of_length_)) {
    return true;
  } else {
    return false;
  }
}

QuadTree::QuadTree(const AABB &boundary) {
  root_ = std::make_unique<Node>(boundary);
}

void QuadTree::Node::subdivide() {
  float quarter_length = boundary.half_of_length_ / 2;
  children[0] = std::make_unique<Node>(
      AABB{boundary.center_.x - quarter_length,
           boundary.center_.y - quarter_length,
           quarter_length});
  children[1] = std::make_unique<Node>(
      AABB{boundary.center_.x + quarter_length,
           boundary.center_.y - quarter_length,
           quarter_length});
  children[2] = std::make_unique<Node>(
      AABB{boundary.center_.x - quarter_length,
           boundary.center_.y + quarter_length,
           quarter_length});
  children[3] = std::make_unique<Node>(
      AABB{boundary.center_.x + quarter_length,
           boundary.center_.y + quarter_length,
           quarter_length});
}

bool QuadTree::insert(const Ball &ball) {
  return root_->insert(ball);
}

bool QuadTree::Node::insert(const Ball &ball) {
  if (boundary.contains(ball)) {
    if (children[0] == nullptr) {
      if (balls.size() < MAX_AABB_BALLS_COUNT) {
        balls.push_back(ball);
        return true;
      }
      subdivide();

    }
    bool is_inserted = false;
    for (uint32_t i = 0; i < 4; ++i) {
      is_inserted |= children[i]->insert(ball);
    }
    return is_inserted;
  }
}

std::vector<Ball> QuadTree::get_close_balls(const Ball &ball) {
  auto search_range = AABB{ball.get_center().x,
                           ball.get_center().y,
                           ball.get_radius() + 1};
  return query(search_range);
}

std::vector<Ball> QuadTree::query(const AABB &range) {
  std::vector<Ball> close_balls;
  root_->query(range, close_balls);
  return close_balls;
}

void QuadTree::Node::query(const AABB &range,
                           std::vector<Ball> &close_balls) {
  if (!boundary.intersects(range)) {
    return;
  }
  for (const auto &ball : balls) {
    if (range.contains(ball)) {
      close_balls.push_back(ball);
    }
  }
  if (children[0] != nullptr) {
    for (uint32_t i = 0; i < 4; ++i) {
      children[i]->query(range, close_balls);
    }
  }
}

