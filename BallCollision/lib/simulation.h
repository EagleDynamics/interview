#ifndef MAIN_BALLCOLLISION_LIB_SIMULATION_H_
#define MAIN_BALLCOLLISION_LIB_SIMULATION_H_

#include <vector>
#include <chrono>
#include <random>
#include "ball.h"
#include "quad_tree.h"
#include "SFML/Graphics.hpp"
#include "middle_average_filter.h"

class Simulation {
 protected:
  std::vector<Ball> balls_;
  heap_quad_tree quad_tree_;
  sf::Clock clock;
  double last_time_{0};
  Math::middle_average_filter<double, 100> fpscounter;

 protected:
  void init_simulation();
  void recalculate();

 public:
  Simulation() = default;

  virtual void start() = 0;
};

class SimulationUI: protected Simulation {
 private:
  sf::RenderWindow window{sf::VideoMode(WINDOW_X, WINDOW_Y), "Ball collision demo"};

 private:
  void init_window();
  void redraw();
  void draw_fps();
  void handle_event();

 public:
  SimulationUI() = default;
  void start() override;
};

#endif //MAIN_BALLCOLLISION_LIB_SIMULATION_H_
