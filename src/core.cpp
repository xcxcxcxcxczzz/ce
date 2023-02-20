#include "core.h"

#include "systems.h"

namespace ce {

bool Core::init() {
  bool ret = true;
  m_SdlContext = shared_ptr<SDLcontext>(new SDLcontext);
  m_SdlResource = shared_ptr<SDLResource>(new SDLResource);
  //   m_vulkan = shared_ptr<VulkanGraphic>(new VulkanGraphic);

  ret &= m_SdlContext->init();
  // ret &= m_vulkan->init(getSDLContextRef());

  m_TimePoints.reset();

  m_SdlResource->init(m_SdlContext);

  m_CollisionCallback = [](auto, auto) {};

  return ret;
}

bool Core::IsQuit() {
  return m_IsQuit;
}

void Core::reset() {
  m_SdlResource->free();
  m_EnttRegistry.clear();
}

void Core::pollEvent() {
  auto& events = m_EventList;
  bool& is_quit = m_IsQuit;

  events = EventList{};
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    events.list.push_back(e);
    switch (e.type) {
      case SDL_QUIT:
        events.is_quit = true;
        is_quit = true;
        break;
      case SDL_KEYDOWN:
        events.keys_pressed += 1;
        break;
      case SDL_KEYUP:
        events.keys_released += 1;
        break;
      default:
        break;
    }
  }
}

void Core::updatePhysic() {
  auto& registry = m_EnttRegistry;
  milliseconds step = makeStep(m_TimePoints.physic);

  ObjectSystem object_system{registry};
  object_system.setTimeStep(step);
  object_system.update();

  PhysicSystem physic_system{registry};
  physic_system.setTimeStep(step);
  physic_system.update();

  CollisionDetectSystem detect(registry);
  detect.update();

  CollisionPhysicSystem physic(detect, registry);
  physic.resolve();

  if (m_EnableCollideTrigger) {
    for (auto& [e1, e2] : detect.CollidedPairs())
      m_CollisionCallback(e1, e2);
  }
}

void Core::updateGraphic() {
  auto& registry = m_EnttRegistry;
  auto& sdl_context = *m_SdlContext;
  milliseconds step = makeStep(m_TimePoints.render);

  AnimationSystem anim_system{registry, sdl_context};
  anim_system.setTimeStep(step);
  anim_system.update();

  RenderSystem system{registry, sdl_context};
  system.update();
}


void Core::enableCollisionCallback(bool set) {
  m_EnableCollideTrigger = set;
}

}  // namespace ce
