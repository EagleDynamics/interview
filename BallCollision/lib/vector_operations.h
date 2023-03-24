#ifndef INTERVIEW_BALLCOLLISION_LIB_VECTOR_OPERATIONS_H_
#define INTERVIEW_BALLCOLLISION_LIB_VECTOR_OPERATIONS_H_

#include "SFML/Graphics.hpp"

using Vector2d = sf::Vector2<double>;

double get_length(const Vector2d &vec);

double dot_product(const Vector2d &vec_1,
                  const Vector2d &vec_2);

double vector_product(const Vector2d &vec_1,
                     const Vector2d &vec_2);

#endif //INTERVIEW_BALLCOLLISION_LIB_VECTOR_OPERATIONS_H_
