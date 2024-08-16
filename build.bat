@echo off
mkdir out 2> NUL
cl /nologo /MD /O2 /W4 main.c /Fe"out/vdbdump.exe" /Fo"out/"