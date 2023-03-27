#ifndef INTERVIEW_BALLCOLLISION_BALL_H_
#define INTERVIEW_BALLCOLLISION_BALL_H_

#include "shape.h"

class Ball : public Shape {
 private:
  Vector2d center_{0, 0};
  double radius_ = 0;

 private:
  void set_rand_radius(std::mt19937 &gen);
  void set_rand_center(std::mt19937 &gen);

  double get_tetta() const;
  void update_velocity(double v_1, double v_2,
                       double m_1, double m_2,
                       double tetta_1, double tetta_2,
                       double phi);

 public:
  Vector2d get_center() const;
  double get_radius() const;
  Vector2d get_leftmost() const override;
  Vector2d get_rightmost() const override;
  Vector2d get_topmost() const override;
  Vector2d get_bottommost() const override;
  void set_rand_properties(std::mt19937 &gen) override;
  void set_properties(const Vector2d &center,
                      const Vector2d &direction,
                      double radius,
                      double velocity,
                      bool is_collided);
  void set_center(const Vector2d &center);

  void move(double deltaTime) override;

  bool intersects(const Shape &other) const override;
  bool intersects(const Ball &other) const;

 public:
  friend bool operator==(const Ball &ball_1, const Ball &ball_2);
  friend bool operator!=(const Ball &ball_1, const Ball &ball_2);

  friend void handle_collision(Ball &ball_1, Ball &ball_2);
};

bool operator==(const Ball &ball_1, const Ball &ball_2);
bool operator!=(const Ball &ball_1, const Ball &ball_2);

void handle_collision(Ball &ball_1, Ball &ball_2);

#endif //INTERVIEW_BALLCOLLISION_BALL_H_
