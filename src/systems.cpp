#include "systems.h"

#include "basic_components.h"
#include "std_utils.h"

static int TestAABBAABB(const ce::Comp::MMAABB& a, const ce::Comp::MMAABB& b) {
  if (a.max[0] < b.min[0] || a.min[0] > b.max[0]) return 0;
  if (a.max[1] < b.min[1] || a.min[1] > b.max[1]) return 0;
  return 1;
}

namespace ce {


PhysicSystem::PhysicSystem(entt::registry& registry)
    : m_EnttRegistry{registry}, m_TimeStep{0} {}

void PhysicSystem::setTimeStep(milliseconds step) {
  m_TimeStep = step;
}

void PhysicSystem::update() {
  using namespace Comp;
  auto& registry = m_EnttRegistry;

  float step = 0.001 * m_TimeStep.count();

  auto view = registry.view<Position>();
  view.each([step](Position& pos) {
    pos.p += pos.v * step;
  });
}

RenderSystem::RenderSystem(entt::registry& registry, SDLcontext& sdl_context, uint32_t visible_mask)
    : m_EnttRegistry{registry}, m_SdlContext{sdl_context}, m_Mask{visible_mask} {}

void RenderSystem::update() {
  using namespace Comp;
  auto& registry = m_EnttRegistry;
  auto renderer = std::get<SDL_Renderer*>(m_SdlContext.Context());

  Camera camera;
  for (auto&& [e, cam] : registry.view<Camera>().each()) {
    camera = cam;
    break;
  }

  SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
  SDL_RenderClear(renderer);

  registry.sort<Renderable>([](const Renderable& r1, const Renderable& r2) {
    return r1.priority < r2.priority;
  });

  for (auto&& [e, draw_data, pos] : registry.view<Renderable, Position>().each()) {
    if (m_Mask & draw_data.visible_group) continue;

    Vec2 render_pos = pos.p - camera.pos.p;
    draw_data.rect.x = render_pos.x;
    draw_data.rect.y = render_pos.y;

    auto glow = registry.try_get<Glow>(e);
    if (glow && glow->value) {
      Uint8 r, g, b;
      SDL_GetTextureColorMod(draw_data.texture, &r, &g, &b);
      SDL_SetTextureColorMod(draw_data.texture, r, g, 0);
      SDL_RenderCopy(renderer, draw_data.texture, NULL, &draw_data.rect);
      SDL_SetTextureColorMod(draw_data.texture, r, g, b);

      glow->value = false;
    } else
      SDL_RenderCopy(renderer, draw_data.texture, NULL, &draw_data.rect);
  }

  SDL_RenderPresent(renderer);
}

AnimationSystem::AnimationSystem(entt::registry& registry, SDLcontext& sdl_context)
    : m_EnttRegistry{registry}, m_SdlContext{sdl_context}, m_TimeStep{0} {}

void AnimationSystem::setTimeStep(milliseconds step) {
  m_TimeStep = step;
}

void AnimationSystem::update() {
  using namespace Comp;
  auto& registry = m_EnttRegistry;

  auto current = chrono_clock::now();
  auto view = registry.view<Renderable, Animation>();

  for (auto&& [e, draw_data, an] : registry.view<Renderable, Animation>().each()) {
    if (an.frame.empty()) return;

    bool is_updated = false;

    while (current > an.next_update) {
      int i = (an.index + 1) % an.frame.size();
      an.index = i;
      an.next_update += an.frame[i].second;
      is_updated = true;
    }

    if (is_updated) draw_data.texture = an.frame[an.index].first;
  }
}

CollisionDetectSystem::CollisionDetectSystem(entt::registry& registry)
    : m_EnttRegistry{registry} {}

void CollisionDetectSystem::update() {
  using namespace Comp;
  using coord = std::pair<int, int>;

  auto& collided_pairs = m_CollideList;
  auto& registry = m_EnttRegistry;
  const int grid_size = 100;

  collided_pairs.clear();

  auto find_check_range = [grid_size](Position& pos, MMAABB& box) -> std::vector<coord> {
    std::vector<coord> check_range;
    std::vector<coord> occupied;
    // check 3x3 around each occupied
    const std::vector<coord> near_grid{{0, 0}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};
    Vec2& min = pos.p;
    Vec2 max = pos.p + box.max; 
    for (int x = min.x; x < max.x; x += grid_size)
      for (int y = min.y; y < max.y; y += grid_size)
        occupied.emplace_back(x / grid_size, y / grid_size);
    std::unordered_map<coord, int, hash_pair> counted_grid;
    for (coord& c : occupied)
      for (const coord& r : near_grid) {
        coord f{c.first + r.first, c.second + r.second};
        if (counted_grid[f]++ == 0)
          check_range.push_back(f);
      }
    return check_range;
  };

  std::unordered_map<coord, std::vector<entt::entity>, hash_pair> partition;

  for (auto&& [e, pos, box] : registry.view<Position, MMAABB>().each()) {
    std::vector<coord> check_range = find_check_range(pos, box);

    std::set<entt::entity> skip;
    skip.emplace(e);

    for (auto p : check_range) {
      // test against each entity inside grid
      for (entt::entity in : partition[p]) {
        // skip if entity pair alreayd tested
        if (skip.find(in) != skip.end()) continue;
        skip.emplace(in);

        auto [in_pos, in_box] = registry.get<Position, MMAABB>(in);

        bool test = TestAABBAABB(box, in_box);
        if (test) {
          collided_pairs.push_back({e, in});
        }
      }
      partition[p].push_back(e);
    }
  }
}

CollisionPhysicSystem::CollisionPhysicSystem(CollisionDetectSystem& detect, entt::registry& registry)
    : m_DetectSystem{detect}, m_EnttRegistry{registry} {
}

void CollisionPhysicSystem::resolve() {
  using namespace Comp;

  for (auto& e : m_DetectSystem.CollidedPairs()) {
    // auto* comp = m_EnttRegistry.try_get<
    Glow& g1 = m_EnttRegistry.emplace_or_replace<Glow>(e.first);
    Glow& g2 = m_EnttRegistry.emplace_or_replace<Glow>(e.second);
    g1.value = true;
    g2.value = true;
  }
}

ObjectSystem::ObjectSystem(entt::registry& registry)
    : m_EnttRegistry{registry}, m_TimeStep{0} {}

void ObjectSystem::setTimeStep(milliseconds step) {
  m_TimeStep = step;
}

void ObjectSystem::update() {
  using namespace Comp;
  for (auto&& [e, life] : m_EnttRegistry.view<Lifetime>().each()) {
    life.left -= m_TimeStep;
    if (life.left <= 0ms) m_EnttRegistry.destroy(e);
  }
}

}  // namespace ce