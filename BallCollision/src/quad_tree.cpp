#include "quad_tree.h"

base_quad_tree::AABB::AABB(const sf::RenderWindow &window) {
  auto size = window.getSize();
  center_ = {(double) std::max(size.x, size.y) / 2,
             (double) std::max(size.x, size.y) / 2};
  half_of_length_ = (double) std::max(size.x, size.y) / 2;
}

bool base_quad_tree::AABB::contains(const Ball &ball) const {
  auto ball_center = ball.get_center();
  double radius = ball.get_radius();
  if ((ball_center.x + radius >= center_.x - half_of_length_) &&
      (ball_center.x - radius <= center_.x + half_of_length_) &&
      (ball_center.y + radius >= center_.y - half_of_length_) &&
      (ball_center.y - radius <= center_.y + half_of_length_)) {
    return true;
  } else {
    return false;
  }
}

bool base_quad_tree::AABB::intersects(const AABB &other) const {
  if ((std::abs(center_.x - other.center_.x) <
      half_of_length_ + other.half_of_length_) &&
      (std::abs(center_.y - other.center_.y) <
          half_of_length_ + other.half_of_length_)) {
    return true;
  } else {
    return false;
  }
}

base_quad_tree::base_quad_tree(const std::vector<Ball> &balls)
    : balls_(balls) {}

std::vector<uint32_t> base_quad_tree::get_close_balls(const Ball &ball) const {
  auto search_range = AABB{ball.get_center().x,
                           ball.get_center().y,
                           ball.get_radius() + 1};
  return query(search_range);
}

bool base_quad_tree::is_collided(const Ball &ball) const {
  auto close_balls_id = get_close_balls(ball);
  for (uint32_t id : close_balls_id) {
    auto other = balls_[id];
    if (ball.intersects(other)) {
      return true;
    }
  }
  return false;
}

bool base_quad_tree::insert(const Ball &ball) {
  balls_.push_back(ball);
  return root_->insert(this, balls_.size() - 1);
}

std::vector<uint32_t> base_quad_tree::query(const AABB &range) const {
  std::vector<uint32_t> close_balls;
  root_->query(this, range, close_balls);
  return close_balls;
}

// ---------HEAP QUAD TREE---------
heap_quad_tree::heap_quad_tree() {
  root_ = std::make_unique<Node>();
}

heap_quad_tree::heap_quad_tree(const AABB &boundary) {
  root_ = std::make_unique<Node>(boundary);
}

heap_quad_tree::heap_quad_tree(const sf::RenderWindow &window) {
  root_ = std::make_unique<Node>(window);
}

heap_quad_tree::heap_quad_tree(const std::vector<Ball> &balls,
                               const AABB &boundary) :
    base_quad_tree(balls) {
  root_ = std::make_unique<Node>(boundary);
  uint32_t balls_count = balls.size();
  for (uint32_t i = 0; i < balls_count; ++i) {
    root_->insert(this, i);
  }
}

void heap_quad_tree::Node::subdivide() {
  double quarter_length = boundary.half_of_length_ / 2;
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

bool heap_quad_tree::Node::insert(base_quad_tree *qt_ptr, uint32_t idx) {
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
  return false;
}

void heap_quad_tree::Node::query(const base_quad_tree *qt_ptr,
                                 const AABB &range,
                                 std::vector<uint32_t> &close_balls) const {
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

void heap_quad_tree::update(const std::vector<Ball> &balls) {
  balls_ = balls;
  root_ = std::make_unique<Node>(root_->boundary);
  uint32_t balls_count = balls.size();
  for (uint32_t i = 0; i < balls_count; ++i) {
    root_->insert(this, i);
  }
}

// ---------STACK QUAD TREE---------
// Use it when you have enough the stack size.
// Fewer freezes expected.
stack_quad_tree::stack_quad_tree() {
  root_ = std::make_unique<Node>();
  nodes_.emplace_back();
}

stack_quad_tree::stack_quad_tree(const AABB &boundary) {
  root_ = std::make_unique<Node>(boundary);
  nodes_.emplace_back(boundary);
}

stack_quad_tree::stack_quad_tree(const std::vector<Ball> &balls,
                                 const AABB &boundary) :
    base_quad_tree(balls) {
  root_ = std::make_unique<Node>(boundary);
  nodes_.emplace_back(boundary);
  uint32_t balls_count = balls.size();
  for (uint32_t i = 0; i < balls_count; ++i) {
    root_->insert(this, i);
  }
}

void stack_quad_tree::Node::subdivide(stack_quad_tree *qt_ptr) {
  double quarter_length = boundary.half_of_length_ / 2;
  uint32_t n = qt_ptr->nodes_.size();
  qt_ptr->nodes_.push_back(AABB{boundary.center_.x - quarter_length,
                                boundary.center_.y - quarter_length,
                                quarter_length});
  children[0] = n;
  qt_ptr->nodes_.push_back(AABB{boundary.center_.x + quarter_length,
                                boundary.center_.y - quarter_length,
                                quarter_length});
  children[1] = n + 1;
  qt_ptr->nodes_.push_back(AABB{boundary.center_.x - quarter_length,
                                boundary.center_.y + quarter_length,
                                quarter_length});
  children[2] = n + 2;
  qt_ptr->nodes_.push_back(AABB{boundary.center_.x + quarter_length,
                                boundary.center_.y + quarter_length,
                                quarter_length});
  children[3] = n + 3;
}

bool stack_quad_tree::Node::insert(base_quad_tree *qt_ptr, uint32_t idx) {

  if (boundary.contains(qt_ptr->balls_[idx])) {
    if (children[0] == 0) {
      if (balls_id.size() < qt_ptr->MAX_AABB_BALLS_COUNT) {
        balls_id.push_back(idx);
        return true;
      }
      subdivide(dynamic_cast<stack_quad_tree *>(qt_ptr));
    }
    bool is_inserted = false;
    for (uint32_t i = 0; i < 4; ++i) {
      is_inserted |=
          dynamic_cast<stack_quad_tree *>(qt_ptr)->nodes_[children[i]].insert(
              qt_ptr,
              idx);
    }
    return is_inserted;
  }
}

void stack_quad_tree::Node::query(const base_quad_tree *qt_ptr,
                                  const AABB &range,
                                  std::vector<uint32_t> &close_balls) const {
  if (!boundary.intersects(range)) {
    return;
  }
  for (uint32_t id : balls_id) {
    if (range.contains(qt_ptr->balls_[id])) {
      close_balls.push_back(id);
    }
  }
  if (children[0] != 0) {
    for (uint32_t i = 0; i < 4; ++i) {
      dynamic_cast<const stack_quad_tree *>(qt_ptr)->nodes_[children[i]].query(
          qt_ptr,
          range,
          close_balls);
    }
  }
}

void stack_quad_tree::update(const std::vector<Ball> &balls) {
  // TODO: We can optimize it using additional field to maintain
  // index of last used node. Hence, we won't have to clear vector
  // each iteration.
  nodes_.resize(0);
  nodes_.emplace_back(root_->boundary);
  balls_ = balls;
  root_ = std::make_unique<Node>(root_->boundary);
  uint32_t balls_count = balls.size();
  for (uint32_t i = 0; i < balls_count; ++i) {
    root_->insert(this, i);
  }
}
