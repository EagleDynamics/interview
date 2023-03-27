#include "ball.h"
#include "vector_operations.h"
#include <cmath>

double Ball::get_radius() const { return radius_; }
Vector2d Ball::get_center() const { return center_; }

void Ball::set_rand_radius(std::mt19937 &gen) {
  radius_ = MIN_BALL_R + (double) (gen() % (int) (MAX_BALL_R - MIN_BALL_R));
}

void Ball::set_rand_center(std::mt19937 &gen) {
  center_ = {radius_ + (double) (gen() % (int) (WINDOW_X - 2 * radius_)),
             radius_ + (double) (gen() % (int) (WINDOW_Y - 2 * radius_))};
}

void Ball::set_rand_properties(std::mt19937 &gen) {
  // Direction vector must be a unit vector if we use
  // suggested move-method. Also this vector can't be a
  // null vector if Ball has non-zero velocity_. So after we get
  // not null direction vector we need to normalize it.
  Shape::set_rand_properties(gen);
  set_rand_radius(gen);
  set_rand_center(gen);
}

void Ball::set_properties(const Vector2d &center,
                          const Vector2d &direction,
                          double radius,
                          double velocity,
                          bool is_collided) {
  Shape::set_properties(direction, velocity, is_collided);
  center_ = center;
  radius_ = radius;
}

void Ball::set_center(const Vector2d &center) {
  center_ = center;
}

void Ball::set_collided(bool state) {
  is_collided_ = state;
}

void Ball::move(double deltaTime) {
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

bool Ball::intersects(const Shape &other) const {
  // not implemented
}

bool Ball::intersects(const Ball &other) const {
  auto delta = center_ - other.center_;
  return get_length(delta) <= radius_ + other.radius_;
}

bool operator==(const Ball &ball_1, const Ball &ball_2) {
  return ball_1.center_ == ball_2.center_ &&
      ball_1.direction_ == ball_2.direction_ &&
      ball_1.radius_ == ball_2.radius_ &&
      ball_1.velocity_ == ball_2.velocity_;
}

bool operator!=(const Ball &ball_1, const Ball &ball_2) {
  return !(ball_1 == ball_2);
}

double Ball::get_tetta() const {
  double tetta = 0;
  if (velocity_ != 0) {
    tetta = acos(direction_.x);
    if (direction_.y < 0) {
      tetta *= -1;
    }
  }
  return tetta;
}

void Ball::update_velocity(double v_1, double v_2,
                           double m_1, double m_2,
                           double tetta_1, double tetta_2,
                           double phi) {
  Vector2d v_1_upd;
  v_1_upd.x = ((v_1 * cos(tetta_1 - phi) * (m_1 - m_2)) +
      2 * m_2 * v_2 * cos(tetta_2 - phi)) * cos(phi) / (m_1 + m_2) +
      v_1 * sin(tetta_1 - phi) * cos(phi + PI / 2);
  v_1_upd.y = ((v_1 * cos(tetta_1 - phi) * (m_1 - m_2)) +
      2 * m_2 * v_2 * cos(tetta_2 - phi)) * sin(phi) / (m_1 + m_2) +
      v_1 * sin(tetta_1 - phi) * sin(phi + PI / 2);
  double upd_velocity = get_length(v_1_upd);
  if (upd_velocity == 0) {
    velocity_ = 0;
    direction_ = {0, 0};
  } else {
    direction_ = {v_1_upd.x / upd_velocity,
                  v_1_upd.y / upd_velocity};
    velocity_ = upd_velocity;
  }
}

void handle_collision(Ball &ball_1, Ball &ball_2) {
  double m_1 = ball_1.radius_ * ball_1.radius_,
      m_2 = ball_2.radius_ * ball_2.radius_;
  double v_1 = ball_1.velocity_,
      v_2 = ball_2.velocity_;
  double tetta_1 = ball_1.get_tetta(),
      tetta_2 = ball_2.get_tetta();
  Vector2d center_1 = ball_1.get_center(),
      center_2 = ball_2.get_center();
  Vector2d center_line = center_2 - center_1;
  double r_1 = ball_1.get_radius(),
      r_2 = ball_2.get_radius();
  // TODO: there may be several exotic cases:
  // 1. second ball with new center goes outside the window
  // 2. second balls with new center is superimposed on some another ball
  // in both cases, further imposition of some balls is possible.
  // But such cases are unlikely.
  ball_2.set_center(
      center_1 + center_line / get_length(center_line) * (r_1 + r_2));
  double phi = PI / 2;
  if (center_line.x != 0) {
    phi = atan(center_line.y / center_line.x);
  }
  ball_1.update_velocity(v_1, v_2, m_1, m_2, tetta_1, tetta_2, phi);
  ball_2.update_velocity(v_2, v_1, m_2, m_1, tetta_2, tetta_1, phi);
}