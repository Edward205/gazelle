#pragma once
#include <iostream>

#include "SDL.h"
#include "ui.h"
#include "lua_ui.h"
#include "lua_parsers.h"

//#include "lua542/include/sol.hpp"

#include <variant>
#include <memory>

extern "C" {
#include "lua542/include/lua.h"
#include "lua542/include/lauxlib.h"
#include "lua542/include/lualib.h"
static void dumpstack(lua_State* L) {
    printf("\n");
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        printf("%d\t%s\t", i, luaL_typename(L, i));
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(L, i));
            break;
        case LUA_TSTRING:
            printf("%s\n", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            printf("%s\n", "nil");
            break;
        default:
            printf("%p\n", lua_topointer(L, i));
            break;
        }
    }
}
}

#ifdef _WIN64
#pragma comment(lib, "lua542/liblua54.a")
#endif

bool CheckLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Eroare", errormsg.c_str(), window);
		return false;
	}
	else
		return true;
}

lua_canvas main_canvas;

void parse_lua_elements(lua_State* L)
{
    lua_pushstring(L, "element");
    lua_gettable(L, -2);

    // de verificat daca este un string
    std::string elementType = lua_tostring(L, -1);
    const char* cstr = elementType.c_str();

    lua_pop(L, 1);
    if (elementType == "text")
    {
        lua_pushstring(L, "text");
        lua_gettable(L, -2);
        std::string testtext = lua_tostring(L, -1);
        lua_pop(L, 1);

        int i = 0;
        int grid[2], position[2];
        bool valid_positioning = true;

        std::vector<int> grid_param, position_param;

        grid_param = parse_int_paramater(L, "grid");
        if (grid_param.size() >= 2)
            for (int i = 0; i < 2; ++i)
                grid[i] = grid_param[i];
        else if (grid_param.size() < 2)
        {
            std::cout << "Non-fatal error: Fewer grid position paramaters than expected. Element will not be displayed." << std::endl;
            valid_positioning = false;
        }
        else if(grid_param.size() > 2)
            std::cout << "Non-fatal error: More grid position paramaters than expected. Extras will be ignored." << std::endl;


        position_param = parse_int_paramater(L, "pos");
        if (position_param.size() == 0)
        {
            position[0] = 0;
            position[1] = 0;
        }
        else
            if (position_param.size() >= 2)
                for (int i = 0; i < 2; ++i)
                    position[i] = position_param[i];
            else if (position_param.size() > 2)
                std::cout << "Non-fatal error: More position paramaters than expected. Extras will be ignored." << std::endl;

        lua_pushstring(L, "pivot");
        lua_gettable(L, -2);
        if (lua_isinteger(L, -1)) { // check if the value at the top of the stack is an integer
            int value = lua_tointeger(L, -1); // convert the value at the top of the stack to an integer and store it in "value"
        }
        else {
            // handle error or do something else
        }
        lua_pop(L, 1);

        if(valid_positioning)
            main_canvas.vct_pointeri.emplace_back(std::make_shared<lua_text>(testtext, grid, position));

    }

    else if (elementType == "button")
    {
        lua_pushstring(L, "text");
        lua_gettable(L, -2);
        std::string testtext = lua_tostring(L, -1);
        lua_pop(L, 1);

        int i = 0;
        int grid[2], position[2];
        bool valid_positioning = true;
        std::vector<int> grid_param, position_param;

        grid_param = parse_int_paramater(L, "grid");
        if (grid_param.size() >= 2)
            for (int i = 0; i < 2; ++i)
                grid[i] = grid_param[i];
        else if (grid_param.size() < 2)
        {
            std::cout << "Non-fatal error: Fewer grid position paramaters than expected. Element will not be displayed." << std::endl;
            valid_positioning = false;
        }
        else if (grid_param.size() > 2)
            std::cout << "Non-fatal error: More grid position paramaters than expected. Extras will be ignored." << std::endl;


        position_param = parse_int_paramater(L, "pos");
        if (position_param.size() == 0)
        {
            position[0] = 0;
            position[1] = 0;
        }
        else
            if (position_param.size() >= 2)
                for (int i = 0; i < 2; ++i)
                    position[i] = position_param[i];
            else if (position_param.size() > 2)
                std::cout << "Non-fatal error: More position paramaters than expected. Extras will be ignored." << std::endl;

        if (valid_positioning)
            main_canvas.vct_pointeri.emplace_back(std::make_shared<lua_button>(testtext, grid, position));
    }
    else
    {
        std::cout << "Potentially fatal error: Unknown element in page" << std::endl;
        lua_pop(L, 1);
    }
}

int lua_render_page(lua_State* L)
{
	int n = lua_gettop(L);
    if (n > 1)
    {
		std::cout << "Page renderer recieved more arguments than expected (1 expected, " << n << " recieved)" << std::endl;
        return -1;
    }
    else if (n < 1)
    {
		std::cout << "Page renderer recieved less arguments than expected (1 expected, " << n << " recieved)" << std::endl;
        return -1;
    }
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        if (lua_istable(L, -1))
        {
            parse_lua_elements(L);
        }
        else
            break;

        lua_pop(L, 1);
    }

	return 1;
}
