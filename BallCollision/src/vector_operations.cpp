#include "vector_operations.h"
#include <cmath>

double get_length(const Vector2d &vec) {
  return (double) sqrt(vec.x * vec.x + vec.y * vec.y);
}

double dot_product(const Vector2d &vec_1,
                  const Vector2d &vec_2) {
  return vec_1.x * vec_2.x + vec_1.y * vec_2.y;
}

double vector_product(const Vector2d &vec_1,
                     const Vector2d &vec_2) {
  return vec_1.x * vec_2.y - vec_1.y * vec_2.x;
}