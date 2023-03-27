#ifndef INTERVIEW_figureCOLLISION_QUADTREE_H_
#define INTERVIEW_figureCOLLISION_QUADTREE_H_

#include "constants.h"
#include "SFML/Graphics.hpp"

using Vector2d = sf::Vector2<double>;

template<typename T>
class heap_quad_tree;

template<typename T>
class stack_quad_tree;

template<typename T>
class base_quad_tree {
 protected:
  struct AABB {
    Vector2d center_{0, 0};
    double half_of_length_ = 0;

    AABB() : center_{(double) std::max(WINDOW_X, WINDOW_Y) / 2,
                     (double) std::max(WINDOW_X, WINDOW_Y) / 2},
             half_of_length_((double) std::max(WINDOW_X, WINDOW_Y) / 2) {}
    AABB(double x, double y, double half_of_length) :
        center_(x, y), half_of_length_(half_of_length) {}
    AABB(const sf::RenderWindow &window);

    bool contains(const T &figure) const;
    bool intersects(const AABB &other) const;
  };

  struct BaseNode {
    AABB boundary;
    std::vector<uint32_t> figures_id;

    BaseNode() = default;
    BaseNode(const AABB &boundary) :
        boundary(boundary) {}

    virtual bool insert(base_quad_tree *qt_ptr, uint32_t idx) = 0;
    virtual void query(const base_quad_tree *qt_ptr,
                       const AABB &range,
                       std::vector<uint32_t> &close_figures_id) const = 0;
  };

  virtual std::vector<uint32_t> query(const AABB &range) const;

 public:
  base_quad_tree() = default;
  base_quad_tree(const std::vector<T> &figures);
  std::vector<uint32_t> get_close_figures(const T &figure) const;
  bool is_collided(const T &figure) const;
  bool insert(const T &figure);
  virtual void update(const std::vector<T> &figures) = 0;

 protected:
  std::unique_ptr<BaseNode> root_{nullptr};
  std::vector<T> figures_;
  const uint32_t MAX_AABB_FIGURES_COUNT = 3;

  friend class heap_quad_tree<T>;
  friend class stack_quad_tree<T>;
};

template<typename T>
class heap_quad_tree : public base_quad_tree<T> {
 private:
  struct Node : base_quad_tree<T>::BaseNode {
    std::array<std::unique_ptr<Node>, 4> children;

    void subdivide();
    bool insert(base_quad_tree<T> *qt_ptr, uint32_t idx) override;
    void query(const base_quad_tree<T> *qt_ptr,
               const typename base_quad_tree<T>::AABB &range,
               std::vector<uint32_t> &close_figures_id) const override;

    Node() = default;
    Node(const typename base_quad_tree<T>::AABB &boundary) :
        base_quad_tree<T>::BaseNode(boundary) {}
  };

 public:
  heap_quad_tree();
  heap_quad_tree(const typename base_quad_tree<T>::AABB &boundary);
  heap_quad_tree(const sf::RenderWindow &window);
  heap_quad_tree(const std::vector<T> &figures,
                 const typename base_quad_tree<T>::AABB &boundary);
  void update(const std::vector<T> &figures) override;
};

template<typename T>
class stack_quad_tree : public base_quad_tree<T> {
 private:
  struct Node : base_quad_tree<T>::BaseNode {
    std::array<uint32_t, 4> children = {0, 0, 0, 0};

    void subdivide(stack_quad_tree *qt_ptr);
    bool insert(base_quad_tree<T> *qt_ptr, uint32_t idx) override;
    void query(const base_quad_tree<T> *qt_ptr,
               const typename base_quad_tree<T>::AABB &range,
               std::vector<uint32_t> &close_figures_id) const override;

    Node() = default;
    Node(const typename base_quad_tree<T>::AABB &boundary) :
        base_quad_tree<T>::BaseNode(boundary) {}
  };

 private:
  std::vector<Node> nodes_;

 public:
  stack_quad_tree();
  stack_quad_tree(const typename base_quad_tree<T>::AABB &boundary);
  stack_quad_tree(const sf::RenderWindow &window);
  stack_quad_tree(const std::vector<T> &figures,
                  const typename base_quad_tree<T>::AABB &boundary);
  void update(const std::vector<T> &figures) override;
};

#endif //INTERVIEW_figureCOLLISION_QUADTREE_H_
