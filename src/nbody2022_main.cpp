#include "../include/nbody2022.hpp"
#include "../include/nbody2022_sim.hpp"
using namespace nbody;

int main(int argc, char **argv) {
  auto status = EXIT_FAILURE;

  try {
    sim::init(1920, 1080, 60, false);
    while (!WindowShouldClose()) {
      sim::tick();
    }
    sim::deinit();
    status = EXIT_SUCCESS;
  } catch (const std::exception &e) {
    std::fprintf(stderr, "FATAL: %s\n", e.what());
  }

  return status;
}