#pragma once
#include <vector>
#include <string>
extern "C" {
#include "lua542/include/lua.h"
#include "lua542/include/lauxlib.h"
#include "lua542/include/lualib.h"
}

//se poate adauga si o limita de dimensiune ca parametru pentru a nu citi parametri degeaba si a irosi memoria
std::vector<int> parse_int_paramater(lua_State* L, std::string parse_for)
{
    std::vector<int> result;
    lua_pushstring(L, parse_for.c_str());
    lua_gettable(L, -2);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return result;
    }
    lua_pushnil(L);
    while (lua_next(L, -2)) {

        if (lua_isinteger(L, -1))
           result.push_back(lua_tointeger(L, -1));
        else
            printf("Non-fatal error: Skipped parsing a non-integer paramater\n");
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
	return result;
}