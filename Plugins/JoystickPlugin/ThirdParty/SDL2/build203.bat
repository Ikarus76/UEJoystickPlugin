echo on

call "C:\Programme\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
call "C:\Program Files\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"

call "C:\Programme\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"

SET SDL_DIR="SDL2-2.0.3"
SET BUILD_DIR=build-%SDL_DIR%

mkdir %SDL_DIR%\Lib

mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake -DCMAKE_GENERATOR_PLATFORM=x64 -DSDL_STATIC=ON -DSDL_SHARED=ON -DFORCE_STATIC_VCRT=ON -DEPIC_EXTENSIONS=OFF --build-64bit --config Release ../%SDL_DIR%

msbuild sdl2.sln /t:SDL2-static /p:Configuration="Release"

copy Release\SDL2.lib ..\%SDL_DIR%\Lib\SDL2-static.lib

cd ..
