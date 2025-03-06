#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
// Aici sunt declarate variabile care sunt accesate in mai multe fisiere

SDL_Window* window;
int window_w = 480;

class common_resources {
public:
	bool loaded = false;
	SDL_Texture* bt_default_tex;
	SDL_Texture* bt_hover_tex;
	SDL_Texture* bt_pressed_tex;
	void load(SDL_Renderer* renderer)
	{
		char bt_default[] = "./resources/default_button.png";
		char bt_default_pressed[] = "./resources/default_button_pressed.png";
		char bt_default_hover[] = "./resources/default_button_hover.png";

		SDL_Surface* surface;

		surface = IMG_Load(bt_default);
		bt_default_tex = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		surface = IMG_Load(bt_default_pressed);
		bt_pressed_tex = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		surface = IMG_Load(bt_default_hover);
		bt_hover_tex = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

	}
private:
	
};
common_resources common_res;