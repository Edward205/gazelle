#pragma once
#include <iostream>

#include "definitions.h"
#include "common_resources.h"
#include "ui.h"
#include "luahandler.h"
#include "lua_ui.h"

//forward declarations
int lua_redirect(lua_State* L);
int lua_getcanvaswidth(lua_State* L);

void reload_lua(std::string path)
{
	url_bar.update_text(renderer, path);
	current_page_path.assign(path);

	main_canvas.clear();
	L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "render_page", lua_render_page);
	lua_register(L, "redirect", lua_redirect);
	lua_register(L, "getCanvasWidth", lua_getcanvaswidth);

	CheckLua(L, luaL_dofile(L, path.c_str()));
	main_canvas.load(renderer);

	main_canvas.user_composer();
}

int lua_redirect(lua_State* L)
{
    int n = lua_gettop(L);
    if (n > 1)
    {
        std::cout << "Redirect recieved more arguments than expected (1 expected, " << n << " recieved)" << std::endl;
        return -1;
    }
    else if (n < 1)
    {
        std::cout << "Redirect recieved less arguments than expected (1 expected, " << n << " recieved)" << std::endl;
        return -1;
    }

    if (lua_isstring(L, -1))
    {
        const char* redirect_addr = lua_tostring(L, -1);
        std::cout << "Redirecting to: " << redirect_addr << std::endl;
        reload_lua(redirect_addr);
    }
    return 1;
}

int lua_getcanvaswidth(lua_State* L)
{
    lua_pushinteger(L, window_w);
    return 1;
}
