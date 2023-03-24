#include <chrono>
#include <random>
#include "SFML/Graphics.hpp"
#include "lib/MiddleAverageFilter.h"
#include "lib/Ball.h"
#include "BaseQuadTree.h"
Math::MiddleAverageFilter<float, 100> fpscounter;

void draw_fps(sf::RenderWindow &window, float fps) {
  char c[32];
  snprintf(c, 32, "FPS: %f", fps);
  std::string string(c);
  sf::String str(c);
  window.setTitle(str);
}

int main() {
  sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "ball collision demo");
  std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());

  // rand() returns new number every time we invoke it.
  // So we had to take this function-call out of the for-loop condition
  size_t balls_count = gen() % (MAX_BALLS - MIN_BALLS) + MIN_BALLS;
  std::vector<Ball> balls(balls_count);

  // randomly initialize balls
  for (size_t i = 0; i < balls_count; ++i) {
    Ball newBall;
    newBall.set_rand_properties(gen);
    balls[i] = newBall;
  }

  // TODO: May be we need to set framerate limit before balls insertion?
  window.setFramerateLimit(120);

  sf::Clock clock;
  float lastime = clock.restart().asSeconds();

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    float current_time = clock.getElapsedTime().asSeconds();
    float deltaTime = current_time - lastime;
    fpscounter.push(1.0f / (current_time - lastime));
    lastime = current_time;

    for (auto &ball : balls) {
      ball.set_collided(false);
    }

    HeapQuadTree quad_tree(balls, {std::max(WINDOW_Y, WINDOW_X) / 2,
                                   std::max(WINDOW_Y, WINDOW_X) / 2,
                                   std::max(WINDOW_Y, WINDOW_X) / 2});

    for (auto &ball : balls) {
      auto close_balls_id = quad_tree.get_close_balls(ball);
      for (uint32_t id : close_balls_id) {
        Ball other = balls[id];
        if (ball != other) {
          if (ball.intersects(other)) {
            ball.set_collided(true);
            other.set_collided(true);
          }
        }
      }
    }

    for (auto &ball : balls) {
      ball.move(deltaTime);
    }

    window.clear();
    // We need to specify &. Otherwise we'll create ball's
    // copy while we iterate through balls vector.
    for (const auto &ball : balls) {
      ball.draw(window);
    }

    draw_fps(window, fpscounter.getAverage());
    window.display();
  }
  return 0;
}
