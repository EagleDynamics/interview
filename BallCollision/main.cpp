#include <chrono>
#include <random>
#include "SFML/Graphics.hpp"
#include "lib/middle_average_filter.h"
#include "lib/Ball.h"
#include "quad_tree.h"
Math::middle_average_filter<double, 100> fpscounter;

void draw_fps(sf::RenderWindow &window, double fps) {
  char c[32];
  snprintf(c, 32, "FPS: %f", fps);
  std::string string(c);
  sf::String str(c);
  window.setTitle(str);
}

int main() {
  sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "Ball collision demo");
  std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());

  HeapQuadTree quad_tree({std::max(WINDOW_Y, WINDOW_X) / 2,
                          std::max(WINDOW_Y, WINDOW_X) / 2,
                          std::max(WINDOW_Y, WINDOW_X) / 2});

  // rand() returns new number every time we invoke it.
  // So we had to take this function-call out of the for-loop condition
  size_t balls_count = gen() % (MAX_BALLS - MIN_BALLS) + MIN_BALLS;
  std::vector<Ball> balls(balls_count);

  // randomly initialize balls
  for (size_t i = 0; i < balls_count; ++i) {
    Ball newBall;
    do {
      newBall.set_rand_properties(gen);
    } while (quad_tree.is_collided(newBall));
    quad_tree.insert(newBall);
    balls[i] = newBall;
  }

  window.setFramerateLimit(120);

  sf::Clock clock;
  double lastime = clock.restart().asSeconds();

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    double current_time = clock.getElapsedTime().asSeconds();
    double deltaTime = current_time - lastime;
    fpscounter.push(1.0f / (current_time - lastime));
    lastime = current_time;

    for (auto &ball : balls) {
      ball.set_collided(false);
    }

    quad_tree.update(balls);

    for (uint32_t i = 0; i < balls_count; ++i) {
      Ball &ball = balls[i];
      auto close_balls_id = quad_tree.get_close_balls(ball);
      for (uint32_t id : close_balls_id) {
        if (id > i) {
          Ball &other = balls[id];
          if (ball.intersects(other)) {
            ball.set_collided(true);
            other.set_collided(true);
            collide(ball, other);
          }
        }
      }
    }

    for (auto &ball : balls) {
      ball.move(deltaTime);
    }

    window.clear();
    // We need to specify &. Otherwise we'll create Ball's
    // copy while we iterate through balls vector.
    for (const auto &ball : balls) {
      ball.draw(window);
    }

    draw_fps(window, fpscounter.getAverage());
    window.display();
  }
  return 0;
}
