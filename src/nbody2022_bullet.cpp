#include "../include/nbody2022_bullet.hpp"
using namespace nbody;
static auto init_ok = false;
constexpr static auto g_force = 0.99f;
static auto bullets = std::unique_ptr<bullet::bullets_t>{nullptr};

bullet::information
bullet::information::async_calculate(const bullet::information &past_bullet,
                                     const bullet::bullets_t &snapshot) {
  auto this_bullet = past_bullet;
  std::for_each(
      snapshot.cbegin(), snapshot.cend(),
      [&this_bullet](const auto snap_bullet) {
        auto calculated_falloff =
            g_force / glm::max(glm::pow(glm::distance(this_bullet.position,
                                                      snap_bullet.position),
                                        2.0f),
                               0.1f);
        this_bullet.delta = calculated_falloff * snap_bullet.position;
        this_bullet.position += this_bullet.delta;
        this_bullet.position = glm::clamp(this_bullet.position,
                                          glm::vec3(-32.0f), glm::vec3(32.0f));
      });
  return this_bullet;
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
                                  rng_distribution(rng));
                    this_bullet.delta =
                        glm::vec3(rng_distribution(rng), rng_distribution(rng),
                                  rng_distribution(rng)) /
                        16.0f;
                    this_bullet.mass = glm::pow(rng_distribution(rng), 2.0f);
                  });
  } else {
    throw std::runtime_error{
        "bullet::init - already initialized, no need to do it again"};
  }
}
void bullet::tick_all() {
  const auto bullets_snapshot = bullet::bullets_t{*bullets};
  auto bullet_futures = std::vector<std::future<bullet::information>>{};
  auto bullets_new = bullet::bullets_t{};
  bullets_new.reserve(bullets->size());
  std::for_each(bullets->begin(), bullets->end(),
                [&bullets_snapshot, &bullet_futures](auto &&this_bullet) {
                  auto task = bullet::bullet_task_t{
                      &bullet::information::async_calculate};
                  bullet_futures.push_back(task.get_future());
                  task.make_ready_at_thread_exit(this_bullet, bullets_snapshot);
                });
  const auto cnt = std::thread::hardware_concurrency();
  while (!bullet_futures.empty()) {
    auto atomic_cnt = std::atomic_uint_fast32_t{cnt};
    std::for_each_n(bullet_futures.begin(), cnt,
                    [&atomic_cnt](auto &&bullet_future) {
                      std::thread{[&atomic_cnt, &bullet_future](auto &&f) {
                        bullet_future.wait();
                        atomic_cnt -= 1;
                        atomic_cnt.notify_one();
                      }}.detach();
                    });
    atomic_cnt.wait(0);
    std::for_each_n(bullet_futures.begin(), cnt,
                    [&bullets_new](auto &&bullet_future) {
                      bullets_new.emplace_back(bullet_future.get());
                    });
    std::remove_if(bullet_futures.begin(), bullet_futures.end(),
                   [](auto &&bullet_future) { return bullet_future.valid(); });
  }
}
void bullet::deinit() { init_ok = false; }
