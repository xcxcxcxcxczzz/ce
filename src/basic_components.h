#pragma once
#include <glm/vec2.hpp>

#include "sdl_warper.h"
#include "std_utils.h"
#include "time.h"

namespace ce {

namespace Comp {

using Vec2 = glm::tvec2<float>;
using Vec4 = glm::tvec4<float>;

struct Position {
  Vec2 p{0}, v{0};
};

struct Renderable {
  using mask_flag = uint32_t;
  static const uint32_t default_hide_flag = 1;

  SDL_Texture* texture = nullptr;
  SDL_Rect rect = {1};
  int priority = 0;
  mask_flag visible_group = 0;
};

struct Animation {
  chrono_time next_update;
  int index;
  std::vector<std::pair<SDL_Texture*, milliseconds>> frame;
};

// minmax AABB
struct MMAABB {
  Vec2 min{0};
  Vec2 max{0};
};

// min-width AABB
struct MWAABB {
  Vec2 min{0};
  union {
    struct {
      float dx, dy;
    };
    array<float, 2> w{0};
  };
};

// center-radius AABB
struct CRAABB {
  Vec2 center{0};
  union {
    struct {
      float dx, dy;
    };
    array<float, 2> half_w{0};
  };
};

struct Glow {
  bool value = false;
};

struct Camera {
  Position pos;
};

struct Lifetime {
  milliseconds left = 1ms;
};

}  // namespace Comp
}  // namespace ce