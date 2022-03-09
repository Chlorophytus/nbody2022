#include "../include/nbody2022_sim.hpp"
#include "../include/nbody2022_bullet.hpp"
using namespace nbody;
static auto init_ok = false;
static auto init_height = 0;
static auto init_width = 0;
static auto init_camera = std::unique_ptr<Camera3D>{nullptr};

// textures
static auto tex_rounded = std::unique_ptr<Texture2D>{nullptr};

void sim::init(const U32 width, const U32 height, const U32 fps_target = 60,
               const bool fullscreen = false) {
  if (!init_ok) {
    init_width = width;
    init_height = height;
    InitWindow(width, height, "Nbody2022 " nbody2022_VSTRING_FULL);
    SetTargetFPS(fps_target);
    // This is an XOR, it is useful here.
    if (fullscreen ^ IsWindowFullscreen()) {
      ToggleFullscreen();
    }
    init_ok = true;
    init_camera = std::make_unique<Camera3D>(Camera3D{
        .position = Vector3{.x = 0.0f, .y = 1.0f, .z = -8.0f},
        .target = Vector3{.x = 0.0f, .y = 0.0f, .z = 0.0f},
        .up = Vector3{.x = 0.0f, .y = 1.0f, .z = 0.0f},
        .fovy = 110.0f,
        .projection = CAMERA_PERSPECTIVE,
    });
    tex_rounded =
        std::make_unique<Texture2D>(LoadTexture("./priv/round_blurred.png"));
    bullet::init(64, 69);
  } else {
    throw std::runtime_error{
        "sim::init - already initialized, no need to do it again"};
  }
}
void sim::tick() {
  BeginDrawing();
  ClearBackground(BLACK);
  UpdateCamera(init_camera.get());
  BeginMode3D(*init_camera);
  DrawTriangle3D(Vector3{.x = 0.0f, .y = 0.0f, .z = -6.0f},
                 Vector3{.x = 0.5f, .y = 0.0f, .z = -7.0f},
                 Vector3{.x = -0.5f, .y = 0.0f, .z = -7.0f}, RAYWHITE);
  auto &&bullets = bullet::tick_all();
  for (auto &&i : *bullets) {
    auto pos = Vector3{.x = i.position.x, .y = i.position.y, .z = i.position.z};
    DrawRay(Ray{.position = pos, .direction = Vector3{.x = i.delta.x, .y = i.delta.y, .z = i.delta.z}}, RAYWHITE);
      DrawSphere(pos, i.mass / 256.0f, WHITE);
  }
  EndMode3D();
  DrawFPS(100, 100);
  EndDrawing();
  }
  void sim::deinit() {
    bullet::deinit();
    UnloadTexture(*tex_rounded);
    tex_rounded = nullptr;
    CloseWindow();
    init_ok = false;
  }