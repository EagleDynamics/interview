#ifndef MAIN_BALLCOLLISION_LIB_SHAPE_H_
#define MAIN_BALLCOLLISION_LIB_SHAPE_H_

#include <random>
#include "constants.h"
#include "SFML/Graphics.hpp"

using Vector2d = sf::Vector2<double>;

class Shape {
 protected:
  Vector2d direction_{0, 0};
  double velocity_ = 0;
  bool is_collided_ = false;

 protected:
  void normalize_dir();
  void set_rand_dir(std::mt19937 &gen);
  void set_rand_velocity(std::mt19937 &gen);

 public:
  virtual void set_rand_properties(std::mt19937 &gen);
  void set_properties(const Vector2d &direction,
                      double velocity,
                      bool is_collided);
  double get_velocity() const;
  bool is_collided() const;

  virtual Vector2d get_leftmost() const = 0;
  virtual Vector2d get_rightmost() const = 0;
  virtual Vector2d get_topmost() const = 0;
  virtual Vector2d get_bottommost() const = 0;


 public:
  virtual void move(double deltaTime) = 0;
  virtual bool intersects(const Shape &other) const;
};

#endif //MAIN_BALLCOLLISION_LIB_SHAPE_H_
