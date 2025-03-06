#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <functional>

SDL_Surface* loadSurface(std::string path, SDL_Surface* gScreenSurface)
{
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

class textRender {
public:
    int x, y;
    int texW, texH;

    SDL_Color color;
    std::string text;

    void render(SDL_Renderer* renderer) {
        rect.x = x;
        rect.y = y;
        SDL_RenderCopy(renderer, text_texture, NULL, &rect);
    };
    void update(TTF_Font* font, SDL_Renderer* renderer) {
        rect.x = x;
        rect.y = y;
        char* c = const_cast<char*>(text.c_str());

        if (loaded)
        {
            SDL_DestroyTexture(text_texture);
            SDL_FreeSurface(surface);
        }
        surface = TTF_RenderText_Blended(font, c, color);
        loaded = true;
        text_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(text_texture, NULL, NULL, &rect.w, &rect.h);
        texW = rect.w;
        texH = rect.h;
    }
    void dealocate()
    {
        if (!loaded)
        {
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(text_texture);
        }
        loaded = false;
    }
private:
    SDL_Surface* surface = NULL;
    SDL_Texture* text_texture = NULL;
    SDL_Rect rect;
    bool loaded = false;
};

class globalContextMenuItem {
public:
    std::string text;
    std::function<const int& ()> onPressed;
    void doPressed() {
        if(onPressed != NULL)
            onPressed();
    }
};
class globalContextMenu {
public:
    int x, y, w, h;
    int index_hovered = -1; // index of which item is hovered, so we can highlight it
    bool open = false, loaded = false, hovered = false;
    std::vector<globalContextMenuItem> items;
    void load(SDL_Renderer* renderer)
    {
        SDL_Surface* surface;
        surface = IMG_Load("./resources/contextmenu_background.png");
        bg_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = IMG_Load("./resources/contextmenu_separator.png");
        separator_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        surface = IMG_Load("./resources/contextmenu_item_hovered.png");
        hovered_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    void render(SDL_Renderer* renderer)
    {
        if (open)
        {
            if (!loaded)
            {
                for (int i = 0; i < texts.size(); ++i)
                    texts[i].dealocate();
                texts.clear();

                font = TTF_OpenFont("./resources/TitilliumWeb.ttf", 15);
                for (int i = 0; i < items.size(); ++i)
                {
                    textRender text;
                    text.x = x + 6;
                    text.y = 3 + y + (24 * i);
                    text.text = items[i].text;
                    texts.push_back(text);
                    texts[i].update(font, renderer);
                }
                TTF_CloseFont(font);
                loaded = true;
            }
            else
            {
                SDL_Rect rect;
                rect.x = x;
                rect.y = y;
                rect.w = w;
                rect.h = h;
                SDL_RenderCopy(renderer, bg_texture, NULL, &rect);
                for (int i = 0; i < texts.size(); ++i)
                {
                    if (i != 0)
                    {
                        rect.x = x;
                        rect.y = texts[i].y - 2;
                        rect.w = w;
                        rect.h = 2;
                        SDL_RenderCopy(renderer, separator_texture, NULL, &rect);
                    }
                    if (i == index_hovered)
                    {
                        rect.x = x;
                        rect.y = texts[i].y - 2;
                        rect.w = w;
                        rect.h = (((24 * (i + 1)) + 6) - ((24 * i) + 6)) + 2;
                        SDL_RenderCopy(renderer, hovered_texture, NULL, &rect);
                    }

                    texts[i].render(renderer);
                }
            }

        }
    }
    void openMenu(std::vector<globalContextMenuItem> items_param, int x_param, int y_param)
    {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        items = items_param;
        x = x_param;
        y = y_param;
        w = 120;
        h = (24 * items_param.size()) + 6;
        open = true;
        loaded = false;
    }
    void closeMenu()
    {
        open = false;
        for (int i = 0; i < texts.size(); ++i)
            texts[i].dealocate();
        texts.clear();
        items.clear();
    }
    void handle_input(SDL_Event e)
    {
        switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_RIGHT)
            {

            }
            else if (e.button.button == SDL_BUTTON_LEFT)
            {
                if (!hovered)
                    closeMenu();
                else
                {
                    if (index_hovered != -1)
                    {
                        items[index_hovered].doPressed();
                        closeMenu();
                    }
                }
            }

        case SDL_MOUSEBUTTONUP:
            break;


        case SDL_MOUSEMOTION:
            if (e.button.x < x + w && e.button.x > x && e.button.y < y + h && e.button.y > y)
            {
                hovered = true;
            }
            else
            {
                hovered = false;
                index_hovered = -1;
            }
            for (int i = 0; i < texts.size(); ++i)
            {
                if (e.button.y > y + (24 * i) + 6 && e.button.y < y + (24 * (i + 1)) + 6 && hovered)
                    index_hovered = i;
            }
            break;
        }
    };

private:
    SDL_Texture* bg_texture;
    SDL_Texture* separator_texture;
    SDL_Texture* hovered_texture;
    std::vector<textRender> texts;
    TTF_Font* font;
};
globalContextMenu globalContext;


class button {
public:
    int x, y, w, h;
    std::function<const int& ()> onPress;
   // std::function<void()> onPress;
    void load(SDL_Renderer* renderer, char path[], char path_hover[], char path_pushed[])
    {
        SDL_Surface* surface;
        if (path != "")
        {
            surface = IMG_Load(path);
            texture_idle = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
        if (path_hover != "")
        {
            surface = IMG_Load(path_hover);
            texture_hover = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
        if (path_pushed != "")
        {
            surface = IMG_Load(path_pushed);
            texture_pushed = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
        texture = texture_idle;
    }
    void load_manual(SDL_Texture *manual_default_tex, SDL_Texture* manual_hover_tex, SDL_Texture* manual_pushed_tex, bool dealocate)
    {
        texture_idle = manual_default_tex;
        texture_hover = manual_hover_tex;
        texture_pushed = manual_pushed_tex;
        texture = texture_idle;
        dealocate_manual = dealocate; // retine daca texturile incarcate manual se vor dealoca in delocate()
    }
    void render(SDL_Renderer* renderer)
    {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
    void handle_input(SDL_Event e)
    {
        int relative_x = w / 2 + x, relative_y = h / 2 + y;

        switch (e.key.keysym.sym)
        {
            // neimplementat
            // pentru taste apasate pe buton
        }
        switch (e.type) {
            case SDL_MOUSEBUTTONDOWN:
                pushed = true;
                if(hovering)
                    texture = texture_pushed;
                break;
            case SDL_MOUSEBUTTONUP:
                if(hovering)
                    if (onPress != nullptr)
                    {
                        onPress();
                    }
                pushed = false;
                texture = texture_idle;
                // aici a fost ratat un break intentionat pentru ca switch-ul sa coboare si la mousemotion

            case SDL_MOUSEMOTION:
                if (e.button.x > relative_x - w / 2 && e.button.x < relative_x + w / 2 && e.button.y > relative_y - h / 2 && e.button.y < relative_y + h / 2)
                {
                    hovering = true;
                    if(!pushed)
                        texture = texture_hover;
                }
                else
                {
                    hovering = false;
                    texture = texture_idle;
                }
                break;
        }
    }
    void dealocate()
    {
        if (dealocate_manual)
        {
            SDL_DestroyTexture(texture);
            SDL_DestroyTexture(texture_idle);
            SDL_DestroyTexture(texture_hover);
            SDL_DestroyTexture(texture_pushed);
        }
    }
private:
    SDL_Texture* texture = NULL;
    SDL_Texture* texture_idle = NULL;
    SDL_Texture* texture_hover = NULL;
    SDL_Texture* texture_pushed = NULL;
    bool hovering = false;
    bool pushed = false;
    bool dealocate_manual = true;
};

class repeatingBackground {
public:
    int x, y, w, h;

    void load(SDL_Renderer* renderer, char path[])
    {
        SDL_Surface* surface;

        surface = IMG_Load(path);
        texW = surface->w;
        texH = surface->h;
        texture = SDL_CreateTextureFromSurface(renderer, surface);
    }
    void render(SDL_Renderer* renderer)
    {
        SDL_Rect rect;
        SDL_Rect crop_rect;
        crop_rect.x = 0;
        crop_rect.y = 0;
        crop_rect.w = texW;
        crop_rect.h = h;
        for (int i = 0; i <= (w / texW) + 0.5; ++i)
        {
            rect.x = x + (texW * i);
            rect.y = y;
            rect.w = texW;
            rect.h = h;
            SDL_RenderCopy(renderer, texture, &crop_rect, &rect);
        }
    }

private:
    SDL_Texture* texture;
    int texW, texH;
};
class textField {
public:
    int x, y, w, h;
    bool hovered = false;
    textRender text;
    std::function<const int& ()> onPressed;

    void load(SDL_Renderer* renderer, char bgpath[], char leftpath[], char rightpath[], char fontpath[])
    {
        SDL_Surface* surface;
        surface = IMG_Load(leftpath);
        texture_left = SDL_CreateTextureFromSurface(renderer, surface);

        surface = IMG_Load(rightpath);
        texture_right = SDL_CreateTextureFromSurface(renderer, surface);

        bg.load(renderer, bgpath);

        text.x = x + 5;
        text.y = y;
        text.color = { 0, 0, 0 };
        font = TTF_OpenFont(fontpath, 15);
        text.update(font, renderer); // se corecteaza inaltimea textului dupa ce se afla inaltimea fontului
        text.y = text.y;
    }
    void render(SDL_Renderer* renderer)
    {
        SDL_Rect rect;
        int left_texture_w;
        SDL_QueryTexture(texture_left, NULL, NULL, &left_texture_w, NULL);
        rect.x = x;
        rect.y = y;
        rect.w = left_texture_w;
        rect.h = h;
        SDL_RenderCopy(renderer, texture_left, NULL, &rect);

        int right_texture_w;
        SDL_QueryTexture(texture_right, NULL, NULL, &right_texture_w, NULL);
        rect.x = x + w;
        rect.y = y;
        rect.w = right_texture_w;
        rect.h = h;
        SDL_RenderCopy(renderer, texture_right, NULL, &rect);

        bg.x = x + left_texture_w;
        bg.y = y;
        bg.w = w - right_texture_w;
        bg.h = h - 2; // o mica eroare care nu stiu daca persista la mai multe textfield-uri in afara de cel de url
        bg.render(renderer);

        text.render(renderer);

        if (focused)
        {
            text.update(font, renderer);

            if (flashOn) // DE SCHIMBAT
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawLine(renderer, text.x + text.texW, (text.y + 3), text.x + text.texW, (text.y + text.texH) - 3);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
                flashOn = false;
            }
            else
            {
                flashOn = true;
            }
        }
    }
    void paste_clipboard()
    {
        char* clipboard_text = SDL_GetClipboardText();
        text.text.assign(clipboard_text);
        SDL_free(clipboard_text);
        // bug: daca cutia nu este selectata, textul nu apare pana cand este selectata
    }
    void handle_input(SDL_Event e)
    {
        int relative_x = w / 2 + x, relative_y = h / 2 + y;

        switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_RIGHT)
            {
                if (hovered)
                {
                    std::vector<globalContextMenuItem> contextmenu;

                    globalContextMenuItem paste;
                    paste.text = "Paste";
                    paste.onPressed = ([&] { paste_clipboard(); return 0; });

                    globalContextMenuItem clear;
                    clear.text = "Clear all";
                    clear.onPressed = ([&] { text.text.clear(); return 0; }); // bug: atunci cand se sterge text, cursorul ramane unde era textul

                    contextmenu.push_back(paste);
                    contextmenu.push_back(clear);

                    globalContext.openMenu(contextmenu, e.button.x, e.button.y);
                }
            }
            else if (e.button.button == SDL_BUTTON_LEFT)
            {
                if (e.button.x > relative_x - w / 2 && e.button.x < relative_x + w / 2 && e.button.y > relative_y - h / 2 && e.button.y < relative_y + h / 2)
                    focused = !focused;
                else
                    focused = false;
            }

        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_TEXTINPUT:
            if (focused)
            {
                text.text += e.edit.text;
            }
        case SDL_KEYDOWN:
            if (e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
                if (focused)
                    if (!text.text.empty())
                        text.text.pop_back();
        

        case SDL_MOUSEMOTION:
            if (e.button.x > relative_x - w / 2 && e.button.x < relative_x + w / 2 && e.button.y > relative_y - h / 2 && e.button.y < relative_y + h / 2)
            {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM));
                hovered = true;
            }
            else
            {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
                hovered = false;
            }
            break;
        }
    };
    void update_text(SDL_Renderer* renderer, std::string new_text)
    {
        text.text.assign(new_text);
        text.update(font, renderer);
    }

private:
    SDL_Texture* texture_left;
    SDL_Texture* texture_right;
    TTF_Font* font;
    repeatingBackground bg;
    bool focused = false;
    bool flashOn = false;
    /*void clearText()
    {
        text.text = "";
    }*/
};

class animation {
    int value1, value2, step;
};

class animationController {

};