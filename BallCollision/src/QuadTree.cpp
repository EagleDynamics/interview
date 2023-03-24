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

QuadTree::QuadTree(const std::vector<Ball> &balls,
                   const AABB &boundary) : balls_(balls) {
  root_ = std::make_unique<Node>(boundary);
  uint32_t balls_count = balls.size();
  for (uint32_t i = 0; i < balls_count; ++i) {
    root_->insert(this, i);
  }
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

bool QuadTree::Node::insert(QuadTree *qt_ptr, uint32_t idx) {
  if (boundary.contains(qt_ptr->balls_[idx])) {
    if (children[0] == nullptr) {
      if (balls_id.size() < qt_ptr->MAX_AABB_BALLS_COUNT) {
        balls_id.push_back(idx);
        return true;
      }
      subdivide();

    }
    bool is_inserted = false;
    for (uint32_t i = 0; i < 4; ++i) {
      is_inserted |= children[i]->insert(qt_ptr, idx);
    }
    return is_inserted;
  }
}

std::vector<uint32_t> QuadTree::get_close_balls(const Ball &ball) {
  auto search_range = AABB{ball.get_center().x,
                           ball.get_center().y,
                           ball.get_radius() + 1};
  return query(search_range);
}

std::vector<uint32_t> QuadTree::query(const AABB &range) {
  std::vector<uint32_t> close_balls;
  root_->query(this, range, close_balls);
  return close_balls;
}

void QuadTree::Node::query(QuadTree *qt_ptr,
                           const AABB &range,
                           std::vector<uint32_t> &close_balls) {
  if (!boundary.intersects(range)) {
    return;
  }
  for (uint32_t id : balls_id) {
    if (range.contains(qt_ptr->balls_[id])) {
      close_balls.push_back(id);
    }
  }
  if (children[0] != nullptr) {
    for (uint32_t i = 0; i < 4; ++i) {
      children[i]->query(qt_ptr, range, close_balls);
    }
  }
}

