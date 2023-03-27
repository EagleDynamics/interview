#include "simulation.h"
#include "ball.h"

template<typename T>
void Simulation<T>::init_simulation() {
  std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());

  // rand() returns new number every time we invoke it.
  // So we had to take this function-call out of the for-loop condition
  size_t figures_count = gen() % (MAX_FIGURES - MIN_FIGURES) + MIN_FIGURES;
  figures_.resize(figures_count);

  // randomly initialize figures
  for (size_t i = 0; i < figures_count; ++i) {
    do {
      figures_[i].set_rand_properties(gen);
    } while (quad_tree_.is_collided(figures_[i]));
    quad_tree_.insert(figures_[i]);
  }
}

template<typename T>
void Simulation<T>::recalculate(double delta_time) {
  for (auto &figure : figures_) {
    figure.set_collided(false);
  }

  quad_tree_.update(figures_);

  size_t figures_count = figures_.size();
  for (uint32_t i = 0; i < figures_count; ++i) {
    T &figure = figures_[i];
    auto close_figures_id = quad_tree_.get_close_figures(figure);
    for (uint32_t id : close_figures_id) {
      if (id > i) {
        T &other = figures_[id];
        if (figure.intersects(other)) {
          figure.set_collided(true);
          other.set_collided(true);
          handle_collision(figure, other);
        }
      }
    }
  }

  for (auto &figure : figures_) {
    figure.move(delta_time);
  }
}

template<typename T>
Simulation<T>::~Simulation() {}

template<typename T>
void SimulationUI<T>::init_window() {
  window.setFramerateLimit(120);
}

template<typename T>
void SimulationUI<T>::redraw() {
  // We need to specify &. Otherwise we'll create figure's
  // copy while we iterate through figures vector.
  for (const auto &figure : this->figures_) {
    sf::CircleShape gfigure;
    auto radius = figure.get_radius();
    gfigure.setRadius(radius);
    gfigure.setPosition(sf::Vector2f(
        figure.get_center() - Vector2d{radius, radius}));
    if (figure.is_collided()) {
      gfigure.setFillColor(sf::Color(150, 50, 250));
    }
    window.draw(gfigure);
  }
}

template<typename T>
void SimulationUI<T>::draw_fps() {
  char c[32];
  snprintf(c, 32, "FPS: %f", this->fpscounter_.getAverage());
  std::string string(c);
  sf::String str(c);
  window.setTitle(str);
}

template<typename T>
void SimulationUI<T>::handle_event() {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
  }
}

template<typename T>
double SimulationUI<T>::get_delta_time() {
  double current_time = clock_.getElapsedTime().asSeconds();
  double delta_time = current_time - last_time_;
  fpscounter_.push(1.0f / (current_time - last_time_));
  last_time_ = current_time;
  return delta_time;
}

template<typename T>
void SimulationUI<T>::start() {
  init_window();
  this->init_simulation();
  last_time_ = clock_.restart().asSeconds();
  while (window.isOpen()) {
    handle_event();
    this->recalculate(get_delta_time());
    window.clear();
    redraw();
    draw_fps();
    window.display();
  }
}

template
class Simulation<Ball>;
template
class SimulationUI<Ball>;
