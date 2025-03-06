#pragma once
#include "SDL.h"
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <optional>

#include "definitions.h"
#include "common_resources.h"
#include "ui.h"
#include "luahandler.h"
#include "lua_ui.h"

SDL_Renderer* renderer;
SDL_Surface* gScreenSurface = NULL;

button back;
button forwardButton;
button reload;
button go;
TTF_Font* defaultfont;

textRender fps_counter;

textField url_bar;
repeatingBackground barBg;
repeatingBackground barShadow;

lua_State* L;

Uint32 lastUpdate = SDL_GetTicks();

Uint64 start;

std::string current_page_path;