#pragma once
#include "nbody2022.hpp"
namespace nbody {
namespace bullet {
struct information;
using bullets_t = std::vector<information>;
using bullet_task_t =
    std::packaged_task<void(bullet::information &, const bullets_t &,
                            std::atomic_uint_fast32_t *)>;
struct information {
  glm::vec3 position;
  glm::vec3 delta;
  F32 mass;
  static void async_calculate(bullet::information &, const bullets_t &,
                              std::atomic_uint_fast32_t *);
};
void init(const U32, const U32);
bullets_t *tick_all();
void deinit();
} // namespace bullet
} // namespace nbody