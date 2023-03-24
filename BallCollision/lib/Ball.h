#ifndef INTERVIEW_BALLCOLLISION_BALL_H_
#define INTERVIEW_BALLCOLLISION_BALL_H_

#include <cassert>
#include <random>
#include "SFML/Graphics.hpp"
#include "constants.h"

using Vector2d = sf::Vector2<double>;

const double PI = 3.1415926535;

class Ball {
 private:
  Vector2d center_{0, 0};
  Vector2d direction_{0, 0};
  double radius_ = 0;
  double velocity_ = 0;
  bool is_collided_ = false;

 private:
  void normalize_dir();
  void set_rand_dir(std::mt19937 &gen);
  void set_rand_radius(std::mt19937 &gen);
  void set_rand_speed(std::mt19937 &gen);
  void set_rand_place(std::mt19937 &gen);

  double get_tetta() const;
  void update_velocity(double v_1, double v_2,
                       double m_1, double m_2,
                       double tetta_1, double tetta_2,
                       double phi);

 public:
  double get_radius() const { return radius_; }
  Vector2d get_center() const { return center_; }
  double get_velocity() const { return velocity_; }
//  Vector2d get_dir() const { return direction_; }
  void set_rand_properties(std::mt19937 &gen);
  void set_properties(Vector2d center,
                      Vector2d direction,
                      double radius,
                      double velocity,
                      bool is_collided);
  void set_center(const Vector2d& center);
  void set_collided(bool state);
  void draw(sf::RenderWindow &window) const;

  void move(double deltaTime) {
    center_.x += direction_.x * velocity_ * deltaTime;
    center_.y += direction_.y * velocity_ * deltaTime;
    if (center_.x + radius_ >= WINDOW_X) {
      center_.x = WINDOW_X - radius_;
      direction_.x *= -1;
    }
    if (center_.x - radius_ <= 0) {
      center_.x = radius_;
      direction_.x *= -1;
    }
    if (center_.y + radius_ >= WINDOW_Y) {
      center_.y = WINDOW_Y - radius_;
      direction_.y *= -1;
    }
    if (center_.y - radius_ <= 0) {
      center_.y = radius_;
      direction_.y *= -1;
    }
  }

  bool intersects(const Ball &other) const;

 public:
  bool operator==(const Ball &other) const;
  bool operator!=(const Ball &other) const;

  friend void collide(Ball &ball_1, Ball &ball_2);
};

void collide(Ball &ball_1, Ball &ball_2);

#endif //INTERVIEW_BALLCOLLISION_BALL_H_
