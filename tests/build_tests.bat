@echo off
call "D:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d D:\Projects\vglite_thorvg_cts\vglite_thorvg\tests
if exist build rmdir /s /q build
cmake -B build -G "Visual Studio 18" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
