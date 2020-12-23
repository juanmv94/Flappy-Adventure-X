@echo off
ccpsx -O3 -Xo$80010000 main.c -omain.cpe
del main.exe
cpe2x main.cpe
del main.cpe