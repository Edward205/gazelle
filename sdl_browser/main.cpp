#define SOL_ALL_SAFETIES_ON 1

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
#include "lua_api.h"

extern "C" {
#include "lua542/include/lua.h"
#include "lua542/include/lauxlib.h"
#include "lua542/include/lualib.h"
}

#ifdef _WIN64
#pragma comment(lib, "lua542/liblua54.a")
#endif

using namespace std;

const float DEG2RAD = 3.14159 / 180;

Uint32 tick_time1 = 0, tick_time2 = 0;
bool frame_switcher = false;
float fps;
bool loop() {
	if(frame_switcher)
	{
		tick_time1 = SDL_GetTicks();
		frame_switcher = !frame_switcher;
	}
	else {
		tick_time2 = SDL_GetTicks();
		frame_switcher = !frame_switcher;
	}

	SDL_Event e;
	// Event loop

	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT)
			return false;
		switch (e.key.keysym.sym)
		{
			case SDLK_0:
				break;
			case SDLK_1:
				break;

			case SDLK_UP:
				break;
			case SDLK_DOWN:
				break;
			case SDLK_LEFT:
				break;
			case SDLK_RIGHT:
				break;
		}
		switch (e.type) {
			case SDL_MOUSEBUTTONDOWN:
				cout << e.button.x << " " << e.button.y << endl;
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					
				}
				break;
			case SDL_MOUSEMOTION:
				break;

			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
					SDL_GetWindowSize(window, &window_w, nullptr);
					barBg.w = window_w;
					barShadow.w = window_w;
					url_bar.w = window_w - 212;
					fps_counter.x = window_w - 40;
					go.x = url_bar.x + url_bar.w + 15;
				}
		}

		if (!globalContext.open)
		{
			back.handle_input(e);
			forwardButton.handle_input(e);
			reload.handle_input(e);
			url_bar.handle_input(e);
			go.handle_input(e);
			main_canvas.handle_input(e);
		}
		else
		{
			globalContext.handle_input(e);
		}

	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);


	barBg.render(renderer);
	back.render(renderer);
	forwardButton.render(renderer);
	reload.render(renderer);
	go.render(renderer);
	url_bar.render(renderer);
	barShadow.render(renderer);

	main_canvas.render(renderer);

	globalContext.render(renderer);

	fps = 1000.0f / (max(tick_time1, tick_time2) - min(tick_time1, tick_time2));
	fps_counter.text = std::to_string(fps);
	fps_counter.update(defaultfont, renderer);
	fps_counter.render(renderer);

	SDL_RenderPresent(renderer);
	
	return true;
}

bool init() {

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}

	window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window) {
		cout << "Error creating window: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}
	SDL_SetWindowResizable(window, SDL_TRUE);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	gScreenSurface = SDL_GetWindowSurface(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		cout << "Error creating renderer: " << SDL_GetError() << endl;
		return false;
	}

	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
	SDL_StartTextInput();

	defaultfont = TTF_OpenFont("./resources/TitilliumWeb.ttf", 25);

	back.x = 5;
	back.y = 5;
	back.w = 30;
	back.h = 30;
	char back_png[] = "./resources/back.png";
	char hover_png[] = "./resources/back_hover.png";
	char pushed_png[] = "./resources/back_pushed.png";
	back.load(renderer, back_png, hover_png, pushed_png);

	forwardButton.x = 40;
	forwardButton.y = 5;
	forwardButton.w = 30;
	forwardButton.h = 30;
	char forward_png[] = "./resources/forward.png";
	char forwardhover_png[] = "./resources/forward_hover.png";
	char forwardpush_png[] = "./resources/forward_pushed.png";
	forwardButton.load(renderer, forward_png, forwardhover_png, forwardpush_png);

	reload.x = 75;
	reload.y = 5;
	reload.w = 30;
	reload.h = 30;
	char reload_png[] = "./resources/reload.png";
	char reloadhover_png[] = "./resources/reload_hover.png";
	char reloadpushed_png[] = "./resources/reload_pushed.png";
	reload.load(renderer, reload_png, reloadhover_png, reloadpushed_png);
	reload.onPress = ([&] { reload_lua(current_page_path); return 0; });


	url_bar.x = 120;
	url_bar.y = 7;
	int window_w, window_h;
	SDL_GetWindowSize(window, &window_w, &window_h);
	url_bar.w = window_w - 212;
	std::cout << url_bar.w << std::endl;
	url_bar.h = 25;
	url_bar.text.text = (Uint16) "";
	char url_bar_png[] = "./resources/url_bar_bg.png";
	char url_bar_left_png[] = "./resources/url_bar_left.png";
	char url_bar_right_png[] = "./resources/url_bar_right.png";
	char font_path[] = "./resources/TitilliumWeb.ttf";
	url_bar.load(renderer, url_bar_png, url_bar_left_png, url_bar_right_png, font_path);

	go.x = url_bar.x + url_bar.w + 15;
	go.y = 5;
	go.w = 64;
	go.h = 30;
	char go_png[] = "./resources/go.png";
	char gohover_png[] = "./resources/go_hover.png";
	char gopushed_png[] = "./resources/go_pushed.png";
	go.onPress = ([&] {reload_lua(url_bar.text.text); return 0; });
	go.load(renderer, go_png, gohover_png, gopushed_png);

	barBg.x = 0;
	barBg.y = 0;
	barBg.w = window_w;
	barBg.h = 40;
	char bg_png[] = "./resources/background.png";
	barBg.load(renderer, bg_png);

	barShadow.x = 0;
	barShadow.y = 40;
	barShadow.w = window_w;
	barShadow.h = 6;
	char shadow_png[] = "./resources/shadow.png";
	barShadow.load(renderer, shadow_png);

	common_res.load(renderer);

	globalContext.load(renderer);
	
	fps_counter.x = 560;
	fps_counter.y = 40;
	fps_counter.text = "fps";
	SDL_Color green;
	green.r = 0;
	green.g = 255;
	green.b = 0;
	green.a = 255;
	fps_counter.color = green;
	fps_counter.update(defaultfont, renderer);

	current_page_path = "default.lua";

	reload_lua(current_page_path.c_str());

	main_canvas.y = 40;
	main_canvas.load(renderer);
	main_canvas.user_composer();

	return true;
}

void kill() {
	// Quit
	main_canvas.clear();
	lua_close(L);
	IMG_Quit();
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char** args) {

	if (!init()) return 1;

	while (loop()) {
		SDL_Delay(10);
	}

	kill();
	return 0;
}