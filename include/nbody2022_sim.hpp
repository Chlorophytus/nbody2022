#pragma once
#include "nbody2022.hpp"
namespace nbody {
namespace sim {
void init(const U32, const U32, const U32, const bool);
void tick();
void deinit();
} // namespace sim
} // namespace nbody