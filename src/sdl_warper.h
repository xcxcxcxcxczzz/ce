#pragma once
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <sdl2/SDL_vulkan.h>

#include <list>
#include <string>
#include <vector>

#include "std_utils.h"

class SDLcontext {
 public:
  SDLcontext() = default;
  ~SDLcontext();
  bool init(int w = 800, int h = 600, std::string title = "title");

  const std::pair<int, int> WindowSize() {
    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);
    return {w, h};
  }

  tuple<SDL_Window*, SDL_Renderer*> Context() { return {m_Window, m_Renderer}; }

 private:
  std::string m_Title;
  SDL_Window* m_Window = nullptr;
  SDL_Renderer* m_Renderer = nullptr;
  bool m_Init = false;
};

struct SDLTextureData {
  SDL_Texture* data;
  int w, h;
};

class SDLResource {
 public:
  SDLResource() = default;
  ~SDLResource();
  void init(shared_ptr<SDLcontext> sdlContext);
  optional<SDLTextureData> loadTexture(const std::string& path);
  void free();

 private:
  shared_ptr<SDLcontext> m_SdlContext;

 private:
  // unordered_map<std::string, int> m_CreatedId;
  std::vector<SDL_Texture*> m_CreatedTextures;
};

// optional<std::vector<SDL_Texture*>> loadTextureSequence(SDLResource& sdl_res, const std::string& dir_path);

inline void scaleSDLRect(float ratio, SDL_Rect& rect) {
  rect.w *= ratio;
  rect.h *= ratio;
}

inline float getAspectSDLRect(const SDL_Rect& rect) {
  return static_cast<float>(rect.w) / rect.h;
}

inline void copyAspectSDLRectw(SDL_Rect& out, const SDL_Rect& in) {
  out.h = out.w / getAspectSDLRect(in);
}

inline void copyAspectSDLRectw(SDL_Rect& out, float w, float h) {
  out.h = out.w / (w / h);
}

inline void copyAspectSDLRecth(SDL_Rect& out, const SDL_Rect& in) {
  out.w = out.h * getAspectSDLRect(in);
}

inline void copyAspectSDLRecth(SDL_Rect& out, float w, float h) {
  out.w = out.h * (w / h);
}
