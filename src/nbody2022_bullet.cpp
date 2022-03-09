#include "../include/nbody2022_bullet.hpp"
using namespace nbody;
static auto init_ok = false;
constexpr static auto g_force = 0.99f;
static auto bullets = std::unique_ptr<bullet::bullets_t>{nullptr};

void bullet::information::async_calculate(bullet::information &this_bullet,
                                          const bullet::bullets_t &snapshot,
                                          std::atomic_uint_fast32_t *flag) {
  std::for_each(snapshot.cbegin(), snapshot.cend(),
                [&this_bullet](const auto snap_bullet) {
                  const auto &falloff = snap_bullet.position /
                                   glm::max(glm::distance(this_bullet.position,
                                                          snap_bullet.position),
                                            1.0f);
                  this_bullet.delta =
                      glm::fma(glm::vec3(0.0f),
                               glm::vec3(0.99f), glm::vec3(0.0f));
                  this_bullet.position += this_bullet.delta;
                  this_bullet.position =
                      glm::clamp(this_bullet.position, glm::vec3(-64.0f),
                                 glm::vec3(64.0f));
                });
  flag->fetch_sub(1);
  flag->notify_one();
}

void bullet::init(const U32 num_bullets, const U32 seed) {
  if (!init_ok) {
    bullets = std::make_unique<bullet::bullets_t>();
    bullets->resize(num_bullets);
    // random number generator
    auto rng = std::mt19937{seed};
    // distribute the RNG with uniform-F32
    auto rng_distribution = std::uniform_real_distribution<F32>{-1.0f, 1.0f};
    std::for_each(bullets->begin(), bullets->end(),
                  [&rng, &rng_distribution](auto &&this_bullet) {
                    this_bullet.position =
                        glm::vec3(rng_distribution(rng), rng_distribution(rng),
                                  rng_distribution(rng)) *
                        16.0f;
                    this_bullet.delta = glm::vec3(0.0f);
                    this_bullet.mass =
                        1.0f +
                        (glm::pow((rng_distribution(rng) / 2.0f) + 1.0f, 2.0f) *
                         16.0f);
                  });
  } else {
    throw std::runtime_error{
        "bullet::init - already initialized, no need to do it again"};
  }
}
bullet::bullets_t *bullet::tick_all() {
  const auto &&bullets_snapshot = bullet::bullets_t{*bullets};
  auto flag = std::atomic_uint_fast32_t{std::thread::hardware_concurrency()};
  for (auto &&this_bullet : *bullets) {
    bullet::bullet_task_t{&bullet::information::async_calculate}
        .make_ready_at_thread_exit(this_bullet, bullets_snapshot, &flag);
  }
  flag.wait(0);
  return bullets.get();
}
void bullet::deinit() { init_ok = false; }
