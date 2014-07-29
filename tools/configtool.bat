set olddir=%CD%
cd %~dp0\configtool
wxLua_win32\wxLua.exe configtool.lua
cd %olddir%
