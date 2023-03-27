#include "simulation.h"

void Simulation::init_simulation() {
  std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());

  // rand() returns new number every time we invoke it.
  // So we had to take this function-call out of the for-loop condition
  size_t balls_count = gen() % (MAX_BALLS - MIN_BALLS) + MIN_BALLS;
  balls_.resize(balls_count);

  // randomly initialize balls
  for (size_t i = 0; i < balls_count; ++i) {
    do {
      balls_[i].set_rand_properties(gen);
    } while (quad_tree_.is_collided(balls_[i]));
    quad_tree_.insert(balls_[i]);
  }

  last_time_ = clock_.restart().asSeconds();
}

void Simulation::recalculate() {
  double current_time = clock_.getElapsedTime().asSeconds();
  double deltaTime = current_time - last_time_;
  fpscounter_.push(1.0f / (current_time - last_time_));
  last_time_ = current_time;

  for (auto &ball : balls_) {
    ball.set_collided(false);
  }

  quad_tree_.update(balls_);

  size_t balls_count = balls_.size();
  for (uint32_t i = 0; i < balls_count; ++i) {
    Ball &ball = balls_[i];
    auto close_balls_id = quad_tree_.get_close_figures(ball);
    for (uint32_t id : close_balls_id) {
      if (id > i) {
        Ball &other = balls_[id];
        if (ball.intersects(other)) {
          ball.set_collided(true);
          other.set_collided(true);
          handle_collision(ball, other);
        }
      }
    }
  }

  for (auto &ball : balls_) {
    ball.move(deltaTime);
  }
}

Simulation::~Simulation() {}

void SimulationUI::init_window() {
  window.setFramerateLimit(120);
}

void SimulationUI::redraw() {
  // We need to specify &. Otherwise we'll create Ball's
  // copy while we iterate through balls vector.
  for (const auto &ball : balls_) {
    sf::CircleShape gball;
    auto radius = ball.get_radius();
    gball.setRadius(radius);
    gball.setPosition(sf::Vector2f(ball.get_center() - Vector2d{radius, radius}));
    if (ball.is_collided()) {
      gball.setFillColor(sf::Color(150, 50, 250));
    }
    window.draw(gball);
  }
}

void SimulationUI::draw_fps() {
  char c[32];
  snprintf(c, 32, "FPS: %f", fpscounter_.getAverage());
  std::string string(c);
  sf::String str(c);
  window.setTitle(str);
}

void SimulationUI::handle_event() {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
  }
}

void SimulationUI::start() {
  init_window();
  init_simulation();

  while (window.isOpen()) {
    handle_event();
    recalculate();
    window.clear();
    redraw();
    draw_fps();
    window.display();
  }
}