#include "shape.h"
#include "vector_operations.h"
#include <cassert>

double Shape::get_velocity() const { return velocity_; }
bool Shape::is_collided() const { return is_collided_; }

void Shape::set_collided(bool state) {
  is_collided_ = state;
}

void Shape::set_rand_properties(std::mt19937 &gen) {
  set_rand_dir(gen);
  set_rand_velocity(gen);
}

void Shape::set_properties(const Vector2d &direction,
                           double velocity,
                           bool is_collided) {
  direction_ = direction;
  velocity_ = velocity;
  is_collided_ = is_collided;
}

void Shape::normalize_dir() {
  double dir_length = get_length(direction_);
  assert(dir_length != 0);
  direction_ /= dir_length;
}

void Shape::set_rand_dir(std::mt19937 &gen) {
  while (direction_ == Vector2d{0, 0}) {
    direction_ = {-DIR_SCALE + (double) (gen() % (2 * DIR_SCALE)),
                  -DIR_SCALE + (double) (gen() % (2 * DIR_SCALE))};
  }
  // vector normalization
  normalize_dir();
}

void Shape::set_rand_velocity(std::mt19937 &gen) {
  velocity_ = MIN_BALL_SPEED + (double) (gen() % (int) (MAX_BALL_SPEED));
}
