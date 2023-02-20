#include "sdl_warper.h"

#include <fstream>

SDLcontext::~SDLcontext() {
  if (!m_Init) return;
  SDL_DestroyRenderer(m_Renderer);
  SDL_DestroyWindow(m_Window);
  IMG_Quit();
  SDL_Quit();
}

bool SDLcontext::init(int w, int h, std::string title) {
  m_Init = true;
  m_Title = title;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
  auto window_init_flag = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN;
  m_Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              w, h,
                              window_init_flag);
  if (!m_Window) return false;

  int img_flags = IMG_INIT_PNG;
  if (!(IMG_Init(img_flags) & img_flags)) return false;
  m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
  return true;
}

SDLResource::~SDLResource() {
  free();
}

void SDLResource::init(shared_ptr<SDLcontext> sdl_context) { m_SdlContext = std::move(sdl_context); }

void SDLResource::free() {
  for (SDL_Texture* p : m_CreatedTextures) {
    SDL_DestroyTexture(p);
  }
  m_CreatedTextures.clear();
}

optional<SDLTextureData> SDLResource::loadTexture(const std::string& path) {
  //   if (m_SdlContext.get() == nullptr) return null_optional;
  SDL_Renderer* renderer = std::get<SDL_Renderer*>(m_SdlContext->Context());

  SDL_Texture* new_texture = nullptr;
  SDL_Surface* surface = IMG_Load(path.c_str());

  if (!surface) return null_optional;

  int w{surface->w}, h{surface->h};

  new_texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  if (!new_texture) return null_optional;

  m_CreatedTextures.push_back(new_texture);
  return SDLTextureData{new_texture, w, h};
}

