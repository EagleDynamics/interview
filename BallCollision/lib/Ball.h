#ifndef INTERVIEW_BALLCOLLISION_BALL_H_
#define INTERVIEW_BALLCOLLISION_BALL_H_

#include <cassert>
#include <cmath>
#include <random>
#include "SFML/Graphics.hpp"
#include "Constants.h"

float get_length(const sf::Vector2f &vec);

class Ball {
 private:
  sf::Vector2f center_{0, 0};
  sf::Vector2f direction_{0, 0};
  float radius_ = 0;
  float velocity_ = 0;
  bool is_collided_ = false;

 private:
  void normalize_dir();
  void set_rand_dir(std::mt19937 &gen);
  void set_rand_radius(std::mt19937 &gen);
  void set_rand_speed(std::mt19937 &gen);
  void set_rand_place(std::mt19937 &gen);

 public:
  float get_radius() const { return radius_; }
  sf::Vector2f get_center() const { return center_; }
//  sf::Vector2f get_dir() const { return direction_; }
  void set_rand_properties(std::mt19937 &gen);
  void set_collided(bool state);
  void draw(sf::RenderWindow &window) const;

  void move(float deltaTime) {
    center_.x += direction_.x * velocity_ * deltaTime;
    center_.y += direction_.y * velocity_ * deltaTime;
  }

  bool intersects(const Ball &other) const;

 public:
  bool operator==(const Ball& other) const;
  bool operator!=(const Ball& other) const;
};

#endif //INTERVIEW_BALLCOLLISION_BALL_H_
