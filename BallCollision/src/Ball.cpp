#include "Ball.h"

float get_length(const sf::Vector2f &vec) {
  return (float) sqrt(vec.x * vec.x + vec.y * vec.y);
}

void Ball::normalize_dir() {
  float dir_length = get_length(direction_);
  assert(dir_length != 0);
  direction_ /= dir_length;
}

void Ball::set_rand_dir(std::mt19937 &gen) {
  while (direction_ == sf::Vector2f{0, 0}) {
    direction_ = {-DIR_SCALE + (float) (gen() % (2 * DIR_SCALE)),
                  -DIR_SCALE + (float) (gen() % (2 * DIR_SCALE))};
  }
  // vector normalization
  normalize_dir();
}

void Ball::set_rand_radius(std::mt19937 &gen) {
  radius_ = MIN_BALL_R + (float) (gen() % (int) (MAX_BALL_R - MIN_BALL_R));
}

void Ball::set_rand_speed(std::mt19937 &gen) {
  // TODO: update
  velocity_ = MIN_BALL_SPEED + (float) (gen() % (int) (MAX_BALL_SPEED));
//    velocity_ = 0;
}

void Ball::set_rand_place(std::mt19937 &gen) {
  center_ = {radius_ + (float) (gen() % (int) (WINDOW_X - radius_)),
             radius_ + (float) (gen() % (int) (WINDOW_Y - radius_))};
}

void Ball::set_rand_properties(std::mt19937 &gen) {
  // Direction vector must be a unit vector if we use
  // suggested move-method. Also this vector can't be a
  // null vector if ball has non-zero velocity_. So after we get
  // not null direction vector we need to normalize it.
  set_rand_dir(gen);
  set_rand_radius(gen);
  set_rand_speed(gen);
  set_rand_place(gen);
}

void Ball::set_collided(bool state) {
  is_collided_ = state;
}

void Ball::draw(sf::RenderWindow &window) const {
  sf::CircleShape gball;
  gball.setRadius(radius_);
  gball.setPosition(center_ - sf::Vector2f{radius_, radius_});
  if (is_collided_) {
    gball.setFillColor(sf::Color(150, 50, 250));
  }
  window.draw(gball);
}

bool Ball::intersects(const Ball &other) const {
  auto delta = center_ - other.center_;
  return get_length(delta) <= radius_ + other.radius_;
}

bool Ball::operator==(const Ball &other) const {
  return center_ == other.center_ &&
      direction_ == other.direction_ &&
      radius_ == other.radius_ &&
      velocity_ == other.velocity_;
}

bool Ball::operator!=(const Ball &other) const {
  return !operator==(other);
}