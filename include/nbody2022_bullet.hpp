#pragma once
#include "nbody2022.hpp"
namespace nbody {
namespace bullet {
struct information;
using bullets_t = std::vector<information>;
using bullet_task_t = std::packaged_task<information(
    const bullet::information &, const bullets_t &)>;
struct information {
  glm::vec3 position;
  glm::vec3 delta;
  F32 mass;
  static information async_calculate(const bullet::information &,
                                     const bullets_t &);
};
void init(const U32, const U32);
void tick_all();
void deinit();
} // namespace bullet
} // namespace nbody