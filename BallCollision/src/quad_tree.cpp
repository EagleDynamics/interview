#include "quad_tree.h"
#include "ball.h"

template<typename T>
base_quad_tree<T>::AABB::AABB(const sf::RenderWindow &window) {
  auto size = window.getSize();
  center_ = {(double) std::max(size.x, size.y) / 2,
             (double) std::max(size.x, size.y) / 2};
  half_of_length_ = (double) std::max(size.x, size.y) / 2;
}

template<typename T>
bool base_quad_tree<T>::AABB::contains(const T &figure) const {
  if ((figure.get_rightmost().x >= center_.x - half_of_length_) &&
      (figure.get_leftmost().x <= center_.x + half_of_length_) &&
      (figure.get_topmost().y >= center_.y - half_of_length_) &&
      (figure.get_bottommost().y <= center_.y + half_of_length_)) {
    return true;
  } else {
    return false;
  }
}

template<typename T>
bool base_quad_tree<T>::AABB::intersects(const AABB &other) const {
  if ((std::abs(center_.x - other.center_.x) <
      half_of_length_ + other.half_of_length_) &&
      (std::abs(center_.y - other.center_.y) <
          half_of_length_ + other.half_of_length_)) {
    return true;
  } else {
    return false;
  }
}

template<typename T>
base_quad_tree<T>::base_quad_tree(const std::vector<T> &figures)
    : figures_(figures) {}

template<typename T>
std::vector<uint32_t> base_quad_tree<T>::get_close_figures(const T &figure) const {
  auto search_range = AABB{figure.get_center().x,
                           figure.get_center().y,
                           figure.get_radius() + 1};
  return query(search_range);
}

template<typename T>
bool base_quad_tree<T>::is_collided(const T &figure) const {
  auto close_figures_id = get_close_figures(figure);
  for (uint32_t id : close_figures_id) {
    auto other = figures_[id];
    if (figure.intersects(other)) {
      return true;
    }
  }
  return false;
}

template<typename T>
bool base_quad_tree<T>::insert(const T &figure) {
  figures_.push_back(figure);
  return root_->insert(this, figures_.size() - 1);
}

template<typename T>
std::vector<uint32_t> base_quad_tree<T>::query(const AABB &range) const {
  std::vector<uint32_t> close_figures;
  root_->query(this, range, close_figures);
  return close_figures;
}

// ---------HEAP QUAD TREE---------
template<typename T>
heap_quad_tree<T>::heap_quad_tree() {
  base_quad_tree<T>::root_ = std::make_unique<Node>();
}

template<typename T>
heap_quad_tree<T>::heap_quad_tree(const typename base_quad_tree<T>::AABB &boundary) {
  base_quad_tree<T>::root_ = std::make_unique<Node>(boundary);
}

template<typename T>
heap_quad_tree<T>::heap_quad_tree(const sf::RenderWindow &window) {
  base_quad_tree<T>::root_ = std::make_unique<Node>(window);
}

template<typename T>
heap_quad_tree<T>::heap_quad_tree(const std::vector<T> &figures,
                                  const typename base_quad_tree<T>::AABB &boundary)
    : base_quad_tree<T>(figures) {
  base_quad_tree<T>::root_ = std::make_unique<Node>(boundary);
  uint32_t figures_count = figures.size();
  for (uint32_t i = 0; i < figures_count; ++i) {
    base_quad_tree<T>::root_->insert(this, i);
  }
}

template<typename T>
void heap_quad_tree<T>::Node::subdivide() {
  double quarter_length = this->boundary.half_of_length_ / 2;
  children[0] = std::make_unique<Node>(
      typename heap_quad_tree<T>::AABB(
          this->boundary.center_.x - quarter_length,
          this->boundary.center_.y - quarter_length,
          quarter_length));
  children[1] = std::make_unique<Node>(
      typename base_quad_tree<T>::AABB(
          this->boundary.center_.x + quarter_length,
          this->boundary.center_.y - quarter_length,
          quarter_length));
  children[2] = std::make_unique<Node>(
      typename base_quad_tree<T>::AABB(
          this->boundary.center_.x - quarter_length,
          this->boundary.center_.y + quarter_length,
          quarter_length));
  children[3] = std::make_unique<Node>(
      typename base_quad_tree<T>::AABB(
          this->boundary.center_.x + quarter_length,
          this->boundary.center_.y + quarter_length,
          quarter_length));
}

template<typename T>
bool heap_quad_tree<T>::Node::insert(base_quad_tree<T> *qt_ptr, uint32_t idx) {
  if (this->boundary.contains(qt_ptr->figures_[idx])) {
    if (children[0] == nullptr) {
      if (this->figures_id.size() < qt_ptr->MAX_AABB_FIGURES_COUNT) {
        this->figures_id.push_back(idx);
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

template<typename T>
void heap_quad_tree<T>::Node::query(const base_quad_tree<T> *qt_ptr,
                                    const typename base_quad_tree<T>::AABB &range,
                                    std::vector<uint32_t> &close_figures) const {
  if (!this->boundary.intersects(range)) {
    return;
  }
  for (uint32_t id : this->figures_id) {
    if (range.contains(qt_ptr->figures_[id])) {
      close_figures.push_back(id);
    }
  }
  if (children[0] != nullptr) {
    for (uint32_t i = 0; i < 4; ++i) {
      children[i]->query(qt_ptr, range, close_figures);
    }
  }
}

template<typename T>
void heap_quad_tree<T>::update(const std::vector<T> &figures) {
  this->figures_ = figures;
  this->root_ = std::make_unique<Node>(this->root_->boundary);
  uint32_t figures_count = figures.size();
  for (uint32_t i = 0; i < figures_count; ++i) {
    this->root_->insert(this, i);
  }
}

// ---------STACK QUAD TREE---------
// Use it when you have enough the stack size.
// Fewer freezes expected.
template<typename T>
stack_quad_tree<T>::stack_quad_tree() {
  this->root_ = std::make_unique<Node>();
  nodes_.emplace_back();
}

template<typename T>
stack_quad_tree<T>::stack_quad_tree(const typename base_quad_tree<T>::AABB &boundary) {
  this->root_ = std::make_unique<Node>(boundary);
  nodes_.emplace_back(boundary);
}

template<typename T>
stack_quad_tree<T>::stack_quad_tree(const sf::RenderWindow &window) {
  base_quad_tree<T>::root_ = std::make_unique<Node>(window);
}

template<typename T>
stack_quad_tree<T>::stack_quad_tree(const std::vector<T> &figures,
                                    const typename base_quad_tree<T>::AABB &boundary)
    :
    base_quad_tree<T>(figures) {
  this->root_ = std::make_unique<Node>(boundary);
  nodes_.emplace_back(boundary);
  uint32_t figures_count = figures.size();
  for (uint32_t i = 0; i < figures_count; ++i) {
    this->root_->insert(this, i);
  }
}

template<typename T>
void stack_quad_tree<T>::Node::subdivide(stack_quad_tree *qt_ptr) {
  double quarter_length = this->boundary.half_of_length_ / 2;
  uint32_t n = qt_ptr->nodes_.size();
  qt_ptr->nodes_.push_back(AABB(this->boundary.center_.x - quarter_length,
                                this->boundary.center_.y - quarter_length,
                                quarter_length));
  children[0] = n;
  qt_ptr->nodes_.push_back(AABB(this->boundary.center_.x + quarter_length,
                                this->boundary.center_.y - quarter_length,
                                quarter_length));
  children[1] = n + 1;
  qt_ptr->nodes_.push_back(AABB(this->boundary.center_.x - quarter_length,
                                this->boundary.center_.y + quarter_length,
                                quarter_length));
  children[2] = n + 2;
  qt_ptr->nodes_.push_back(AABB(this->boundary.center_.x + quarter_length,
                                this->boundary.center_.y + quarter_length,
                                quarter_length));
  children[3] = n + 3;
}

template<typename T>
bool stack_quad_tree<T>::Node::insert(base_quad_tree<T> *qt_ptr, uint32_t idx) {
  if (this->boundary.contains(qt_ptr->figures_[idx])) {
    if (children[0] == 0) {
      if (this->figures_id.size() < qt_ptr->MAX_AABB_figures_COUNT) {
        this->figures_id.push_back(idx);
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

template<typename T>
void stack_quad_tree<T>::Node::query(const base_quad_tree<T> *qt_ptr,
                                     const typename base_quad_tree<T>::AABB &range,
                                     std::vector<uint32_t> &close_figures) const {
  if (!this->boundary.intersects(range)) {
    return;
  }
  for (uint32_t id : this->figures_id) {
    if (range.contains(qt_ptr->figures_[id])) {
      close_figures.push_back(id);
    }
  }
  if (children[0] != 0) {
    for (uint32_t i = 0; i < 4; ++i) {
      dynamic_cast<const stack_quad_tree *>(qt_ptr)->nodes_[children[i]].query(
          qt_ptr,
          range,
          close_figures);
    }
  }
}

template<typename T>
void stack_quad_tree<T>::update(const std::vector<T> &figures) {
  // TODO: We can optimize it using additional field to maintain
  // index of last used node. Hence, we won't have to clear vector
  // each iteration.
  nodes_.resize(0);
  nodes_.emplace_back(this->root_->boundary);
  this->figures_ = figures;
  this->root_ = std::make_unique<Node>(this->root_->boundary);
  uint32_t figures_count = figures.size();
  for (uint32_t i = 0; i < figures_count; ++i) {
    this->root_->insert(this, i);
  }
}

template
class base_quad_tree<Ball>;
template
class heap_quad_tree<Ball>;