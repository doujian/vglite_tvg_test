@echo off
call "D:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d D:\Projects\vglite_thorvg_cts\vglite_thorvg\libs\thorvg-1.0.2\builddir
ninja %*
