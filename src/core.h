#pragma once
#include <entt/entt.hpp>
#include <functional>
#include <vector>

#include "sdl_warper.h"
#include "std_utils.h"
#include "time.h"

namespace ce {

struct EventList {
  std::vector<SDL_Event> list;
  int keys_pressed = 0;
  int keys_released = 0;
  bool is_quit = 0;
};

struct CreditUpdateTime {
  chrono_time physic, render;
  void reset() {
    physic = chrono_clock::now();
    render = chrono_clock::now();
  }
};

class Core {
 public:
  Core() = default;

  bool init();

  void reset();

  void pollEvent();
  void updatePhysic();
  void updateGraphic();

  void enableCollisionCallback(bool set);

  bool IsQuit();

  entt::registry& Registry() { return m_EnttRegistry; }
  const EventList& Events() { return m_EventList; }
  SDLResource& SdlResource() { return *m_SdlResource; }
  SDLcontext& SdlContext() { return *m_SdlContext; }
  shared_ptr<SDLcontext> getSdlContextRef() { return m_SdlContext; }

  void setCollisionCallback(std::function<void(entt::entity, entt::entity)> callback) { m_CollisionCallback = callback; }

 private:
  shared_ptr<SDLcontext> m_SdlContext;
  shared_ptr<SDLResource> m_SdlResource;
  // shared_ptr<VulkanGraphic> m_vulkan;

 private:
  entt::registry m_EnttRegistry;
  EventList m_EventList;
  bool m_IsQuit;
  bool m_EnableCollideTrigger = false;
  std::function<void(entt::entity, entt::entity)> m_CollisionCallback;

  CreditUpdateTime m_TimePoints;
};

}  // namespace ce
