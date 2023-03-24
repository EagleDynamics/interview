#include "Ball.h"
#include "vector_operations.h"
#include <cmath>

void Ball::normalize_dir() {
  double dir_length = get_length(direction_);
  assert(dir_length != 0);
  direction_ /= dir_length;
}

void Ball::set_rand_dir(std::mt19937 &gen) {
  while (direction_ == Vector2d{0, 0}) {
    direction_ = {-DIR_SCALE + (double) (gen() % (2 * DIR_SCALE)),
                  -DIR_SCALE + (double) (gen() % (2 * DIR_SCALE))};
  }
  // vector normalization
  normalize_dir();
}

void Ball::set_rand_radius(std::mt19937 &gen) {
  radius_ = MIN_BALL_R + (double) (gen() % (int) (MAX_BALL_R - MIN_BALL_R));
}

void Ball::set_rand_speed(std::mt19937 &gen) {
  // TODO: update
  velocity_ = MIN_BALL_SPEED + (double) (gen() % (int) (MAX_BALL_SPEED));
//    velocity_ = 0;
}

void Ball::set_rand_place(std::mt19937 &gen) {
  center_ = {radius_ + (double) (gen() % (int) (WINDOW_X - 2 * radius_)),
             radius_ + (double) (gen() % (int) (WINDOW_Y - 2 * radius_))};
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

void Ball::set_properties(Vector2d center,
                    Vector2d direction,
                    double radius,
                    double velocity,
                    bool is_collided) {
  center_ = center;
  direction_ = direction;
  radius_ = radius;
  velocity_ = velocity;
  is_collided_ = is_collided;
}

void Ball::set_collided(bool state) {
  is_collided_ = state;
}

void Ball::draw(sf::RenderWindow &window) const {
  sf::CircleShape gball;
  gball.setRadius(radius_);
  gball.setPosition(sf::Vector2f(center_ - Vector2d{radius_, radius_}));
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

// wikipedia.org/wiki/Elastic_collision
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

#include <iostream>

std::ostream& operator<<(std::ostream& os, const Vector2d& obj) {
  os << "(" << obj.x << ", " << obj.y << ")";
  return os;
}

void Ball::get_info() const {
  std::cout << "center = " << center_ << '\n';
  std::cout << "direction = " << direction_ << '\n';
  std::cout << "radius = " << radius_ << '\n';
  std::cout << "velocity = " << velocity_ << '\n';
}

void collide(Ball &ball_1, Ball &ball_2, double deltaTime) {
  double m_1 = ball_1.radius_ * ball_1.radius_,
      m_2 = ball_2.radius_ * ball_2.radius_;
  double v_1 = ball_1.velocity_,
      v_2 = ball_2.velocity_;
  double tetta_1 = ball_1.get_tetta(),
      tetta_2 = ball_2.get_tetta();
  Vector2d center_1 = ball_1.get_center(),
      center_2 = ball_2.get_center();
  Vector2d center_line = center_2 - center_1;
  double phi = 0;
  if (center_line.x != 0) {
    phi = atan(center_line.y / center_line.x);
  } else {
    phi = PI / 2;
  }
  auto tmp_1 = ball_1, tmp_2 = ball_2;
  ball_1.update_velocity(v_1, v_2, m_1, m_2, tetta_1, tetta_2, phi);
  ball_2.update_velocity(v_2, v_1, m_2, m_1, tetta_2, tetta_1, phi);
  ball_1.move(deltaTime);
  ball_2.move(deltaTime);
  if (ball_1.intersects(ball_2)) {
    std::cout << "----Ball 1----\n";
    tmp_1.get_info();
    std::cout << "----Ball 2----\n";
    tmp_2.get_info();

    std::cout << "\n\n----UPDATE----\n";
    std::cout << "----Ball 1----\n";
    ball_1.get_info();
    std::cout << "----Ball 2----\n";
    ball_2.get_info();
//    assert(false);
  }
}