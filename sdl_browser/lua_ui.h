#pragma once

#include "ui.h"
#include "common_resources.h"
#include <vector>
#include <SDL.h>

#include <memory>
#include <variant>
#include <algorithm>

class Interfata_container_instante
{
public:
	int x, y, w, h, grid[2], position[2] = {0, 0};
	virtual void render(SDL_Renderer* renderer) = 0;
	virtual void load(SDL_Renderer* renderer) = 0;
	virtual void handle_input(SDL_Event e) = 0;
	virtual void dealocate() = 0;
	virtual ~Interfata_container_instante() = default;
};

class lua_button : public Interfata_container_instante {
public:
	std::string text = "";
	std::string bg_img = "";
	std::string fontpath = "";
	lua_button(std::string text_constr, int grid_constr[2], int position_constr[2])
	{
		text = text_constr;
		grid[0] = grid_constr[0];
		grid[1] = grid_constr[1];
		position[0] = position_constr[0];
		position[1] = position_constr[1];
	}
	void load(SDL_Renderer* renderer)
	{
		bt_text.text = text;
		bt.x = x - 10; // creez un padding intre butoane si text
		bt.y = y - 5; 

		bt.load_manual(common_res.bt_default_tex, common_res.bt_hover_tex, common_res.bt_pressed_tex, false);

		bt_text.x = bt.x + 5;
		bt_text.y = bt.y + 2;
		
		bt_text.color = { 0, 0, 0 };
		if (fontpath != "")
		{
			font = TTF_OpenFont(fontpath.c_str(), 15);
		}
		else
		{
			char font_path[] = "./resources/TitilliumWeb.ttf";
			font = TTF_OpenFont(font_path, 15);
		}
		bt_text.update(font, renderer); // se corecteaza inaltimea textului dupa ce se afla inaltimea fontului
		TTF_CloseFont(font);
		bt.w = bt_text.texW + 10;
		bt.h = bt_text.texH + 5;
		h = bt.h;
		w = bt.w;
	}
	void render(SDL_Renderer* renderer)
	{
		bt.x = x;
		bt.y = y;
		bt_text.x = x + 5;
		bt_text.y = y + 2;
		bt.render(renderer);
		bt_text.render(renderer);
	}
	void handle_input(SDL_Event e)
	{
		bt.handle_input(e);
		// orice altceva
	}
	void dealocate()
	{
		bt.dealocate();
		bt_text.dealocate();
	}
	~lua_button()
	{
		dealocate();
	}

private:
	TTF_Font* font;
	button bt;
	textRender bt_text;
};

class lua_text : public Interfata_container_instante {
public:
	std::string text;
	std::string fontpath = "";
	lua_text(std::string text_constr, int grid_constr[2], int position_constr[2])
	{
		text = text_constr;
		grid[0] = grid_constr[0];
		grid[1] = grid_constr[1];
		position[0] = position_constr[0];
		position[1] = position_constr[1];
	}
	void load(SDL_Renderer* renderer) 
	{
		textrenderer.x = x - 7; // mici erori, nu stiu de ce apar
		textrenderer.y = y - 5;

		textrenderer.text = text;

		textrenderer.color = { 0, 0, 0 };
		if (fontpath != "")
		{
			font = TTF_OpenFont(fontpath.c_str(), 15);
		}
		else
		{
			char font_path[] = "./resources/TitilliumWeb.ttf";
			font = TTF_OpenFont(font_path, 15);
		}
		
		textrenderer.update(font, renderer);
		TTF_CloseFont(font);
		w = textrenderer.texW;
		h = textrenderer.texH;
	}
	void render(SDL_Renderer* renderer)
	{
		textrenderer.x = x;
		textrenderer.y = y; // coordontale render-ului trebuie sa fie identice cu cele ale textului

		textrenderer.render(renderer);
	}
	void handle_input(SDL_Event e)
	{

	}
	void dealocate() {
		textrenderer.dealocate();
	}
	~lua_text()
	{
		dealocate();
	}
private:
	textRender textrenderer;
	TTF_Font* font;
};


bool row_comparator(std::shared_ptr<Interfata_container_instante> const& a, std::shared_ptr<Interfata_container_instante> const& b)
{
	return a->grid[0] < b->grid[0];
}
bool col_comparator(std::shared_ptr<Interfata_container_instante> const& a, std::shared_ptr<Interfata_container_instante> const& b)
{
	return a->grid[1] < b->grid[1];
}

class lua_canvas {
public:
	int x, y, w, h;

	std::vector< std::shared_ptr<Interfata_container_instante> >  vct_pointeri;

	void render(SDL_Renderer *renderer)
	{
		for (int i = 0; i < vct_pointeri.size(); ++i)
		{
			vct_pointeri[i]->render(renderer);
		}
	}
	void handle_input(SDL_Event e)
	{
		for (int i = 0; i < vct_pointeri.size(); ++i)
		{
			vct_pointeri[i]->handle_input(e);
		}
	}
	void load(SDL_Renderer* renderer)
	{
		// de schimbat
		for (int i = 0; i < vct_pointeri.size(); ++i)
		{
			vct_pointeri[i]->load(renderer);
		}
	}
	void user_composer()
	{
		/*
		Prima cifra din pos retine randul, iar a doua coloana. 
		Daca se utilizeaza coordonate, prima cifra retine coordonata y, iar a doua coordonata x.
		*/

		std::vector<int> pos, pos_y, offsets_y;
		pos.push_back(0);
		pos_y.push_back(0);
		offsets_y.push_back(0);

		for (int i = 0; i < vct_pointeri.size(); ++i)
		{
			int empty_space = 0; // un int care retine daca s-a creat spatiu gol. acesti spatiu gol risipeste memorie si utilizatorul trebuie avertizat ca s-a creat.
			while (pos.size() <= vct_pointeri[i]->grid[0])
			{
				pos.push_back(0);
				pos_y.push_back(0);
				offsets_y.push_back(0);
				++empty_space;
			}
			if(empty_space > 1)
				std::cout << "Warning: There are " << empty_space << " empty vertical spaces left at position " << vct_pointeri[i]->grid[1] << ". This wastes memory and can potentially create other issues." << std::endl; // TODO: de specificat ce unitate are pozitia
			
			// sortam elementele dupa rand si coloana
			std::sort(vct_pointeri.begin(), vct_pointeri.end(), row_comparator);
			std::sort(vct_pointeri.begin(), vct_pointeri.end(), col_comparator);
			
			//adaugare offseturi (adica position)
			pos[vct_pointeri[i]->grid[0]] += vct_pointeri[i]->position[1];
			offsets_y[vct_pointeri[i]->grid[0]] += vct_pointeri[i]->position[0];

			//aranjare dupa grid
			vct_pointeri[i]->x = x + pos[vct_pointeri[i]->grid[0]];
			pos[vct_pointeri[i]->grid[0]] += vct_pointeri[i]->w;
			
			if (vct_pointeri[i]->grid[0] != 0)
			{
				vct_pointeri[i]->y = y + pos_y[vct_pointeri[i]->grid[0] - 1] + offsets_y[vct_pointeri[i]->grid[0]];
				pos_y[vct_pointeri[i]->grid[0]] = pos_y[vct_pointeri[i]->grid[0] - 1] + (vct_pointeri[i]->h);

			}
			else
			{
				vct_pointeri[i]->y = y + offsets_y[vct_pointeri[i]->grid[0]];
				pos_y[vct_pointeri[i]->grid[0]] = (vct_pointeri[i]->h);
			}
		}
	}
	void clear()
	{
		for (int i = 0; i < vct_pointeri.size(); ++i)
		{
			vct_pointeri[i]->dealocate();
		}
		vct_pointeri.clear();
	}
};