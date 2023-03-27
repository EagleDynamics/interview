#ifndef MAIN_BALLCOLLISION_LIB_SIMULATION_H_
#define MAIN_BALLCOLLISION_LIB_SIMULATION_H_

#include <vector>
#include <chrono>
#include <random>
#include "constants.h"
#include "quad_tree.h"
#include "SFML/Graphics.hpp"
#include "middle_average_filter.h"

template<typename T>
class Simulation {
 protected:
  std::vector<T> figures_;
  heap_quad_tree<T> quad_tree_;

 protected:
  void init_simulation();
  void recalculate(double );

 public:
  Simulation() = default;

  virtual ~Simulation() = 0;
};

template<typename T>
class SimulationUI : protected Simulation<T> {
 private:
  sf::RenderWindow
      window{sf::VideoMode(WINDOW_X, WINDOW_Y), "Figure collision demo"};
  sf::Clock clock_;
  double last_time_{0};
  Math::middle_average_filter<double, 100> fpscounter_;

 private:
  void init_window();
  void redraw();
  void draw_fps();
  void handle_event();
  double get_delta_time();

 public:
  SimulationUI() = default;
  void start();
};

#endif //MAIN_BALLCOLLISION_LIB_SIMULATION_H_
