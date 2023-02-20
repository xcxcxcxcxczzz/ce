#pragma once
#include <entt/entt.hpp>
#include <set>

#include "sdl_warper.h"
#include "std_utils.h"
#include "time.h"

namespace ce {

class RenderSystem {
 public:
  RenderSystem(entt::registry& registry, SDLcontext& sdl_context, uint32_t visible_mask = 1);
  void update();

 private:
  entt::registry& m_EnttRegistry;
  SDLcontext& m_SdlContext;
  uint32_t m_Mask;
};

class AnimationSystem {
 public:
  AnimationSystem(entt::registry& registry, SDLcontext& sdl_context);
  void setTimeStep(milliseconds step);
  void update();

 private:
  entt::registry& m_EnttRegistry;
  SDLcontext& m_SdlContext;
  milliseconds m_TimeStep;
};

using entity_pair = std::pair<entt::entity, entt::entity>;

class CollisionDetectSystem {
 public:
  CollisionDetectSystem(entt::registry& registry);
  void update();
  std::vector<entity_pair>& CollidedPairs() { return m_CollideList; }

 private:
  entt::registry& m_EnttRegistry;
  std::vector<entity_pair> m_CollideList;
};

class CollisionPhysicSystem {
 public:
  CollisionPhysicSystem(CollisionDetectSystem& detect, entt::registry& registry);
  void resolve();

 private:
  CollisionDetectSystem& m_DetectSystem;
  entt::registry& m_EnttRegistry;
};

class PhysicSystem {
 public:
  PhysicSystem(entt::registry& registry);
  void setTimeStep(milliseconds step);
  void update();

 private:
  entt::registry& m_EnttRegistry;
  milliseconds m_TimeStep;
};

class ObjectSystem {
 public:
  ObjectSystem(entt::registry& registry);
  void setTimeStep(milliseconds step);
  void update();

 private:
  entt::registry& m_EnttRegistry;
  milliseconds m_TimeStep;
};

}  // namespace ce