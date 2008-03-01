@echo off

set command=%1
shift

set destdir=%1
shift

call process_file.bat %command% %destdir% glew-1.4.0-win32.zip glew glew bin glew32.dll lib glew32.lib
call process_file.bat %command% %destdir% SDL-devel-1.2.12-VC8.zip SDL-1.2.12 sdl lib sdl.dll lib sdl.lib lib sdlmain.lib
call process_file.bat %command% %destdir% SDL_ttf-devel-2.0.9-VC8.zip SDL_ttf-2.0.9 sdl_ttf lib sdl_ttf.dll lib sdl_ttf.lib lib zlib1.dll lib libfreetype-6.dll
call process_file.bat %command% %destdir% SDL_image-devel-1.2.6-VC8.zip SDL_image-1.2.6 sdl_image lib sdl_image.dll lib sdl_image.lib lib zlib1.dll lib jpeg.dll lib libpng12-0.dll lib libtiff-3.dll
