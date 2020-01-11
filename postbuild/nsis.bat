@echo off

set VER=27

call log --------------------------------------
call log copying temporary files...

if exist final\a2inst%VER%_ru.exe del /Q /F final\a2inst%VER%_ru.exe
if exist final\a2full%VER%_ru.zip del /Q /F final\a2full%VER%_ru.zip
if exist final\a2inst%VER%_en.exe del /Q /F final\a2inst%VER%_en.exe
if exist final\a2full%VER%_en.zip del /Q /F final\a2full%VER%_en.zip

copy /Y release\a2mgr.dll nsis\a2mgr.dll >nul
copy /Y release\allods2.cfg nsis\allods2.cfg >nul
copy /Y release\allods2.exe nsis\allods2.exe >nul
copy /Y release\helper32.dll nsis\helper32.dll >nul
rem copy /Y release\patch.res nsis\patch.res >nul
copy /Y release\patch%VER%.txt nsis\patch%VER%.txt >nul
copy /Y release\smackw32.dll nsis\smackw32.dll >nul
rem copy /Y release\world.res nsis\world.res >nul
rem copy /Y release\locale.res nsis\locale.res >nul
rem copy /Y release\curllib.dll nsis\curllib.dll >nul
rem copy /Y release\openldap.dll nsis\openldap.dll >nul
copy /Y release\SDL.dll nsis\SDL.dll >nul
copy /Y release\SDL_image.dll nsis\SDL_image.dll >nul
copy /Y release\libpng12-0.dll nsis\libpng12-0.dll >nul
copy /Y release\jpeg.dll nsis\jpeg.dll >nul
copy /Y release\zlib1.dll nsis\zlib1.dll >nul
copy /Y release\libtiff-3.dll nsis\libtiff-3.dll >nul
rem copy /Y release\libeay32.dll nsis\libeay32.dll >nul
rem copy /Y release\ssleay32.dll nsis\ssleay32.dll >nul
rem copy /Y release\libsasl.dll nsis\libsasl.dll >nul
rem copy /Y vcredist_x86.exe nsis\vcredist_x86.exe >nul

call log compiling NSIS installer..

rem TODO: make single nsis script

call "makensis.exe" /DVER=%VER% /V2 "%CD%\a2patch.nsi"
if errorlevel 1 goto fail

rem cd nsis
rem ..\7za a -r ..\final\a2full%VER%_ru.zip *.*
rem cd ..

copy /Y release\allods2_en.cfg nsis\allods2.cfg >nul
copy /Y release\patch%VER%_en.txt nsis\patch%VER%.txt >nul

call "makensis.exe" /DVER=%VER% /V2 "%CD%\a2patch_en.nsi"
if errorlevel 1 goto fail

rem cd nsis
rem ..\7za a -r ..\final\a2full%VER%_en.zip *.*
rem cd ..


call log NSIS compiled succeful!
goto remove_temp
:fail
call log NSIS compiled unsucceful!
rem exit 1
:remove_temp

call log deleting temporary files...
if exist nsis\a2mgr.dll del /Q /F nsis\a2mgr.dll >nul
if exist nsis\allods2.exe del /Q /F nsis\allods2.exe >nul
if exist nsis\helper32.dll del /Q /F nsis\helper32.dll >nul
if exist nsis\patch%VER%.txt del /Q /F nsis\patch%VER%.txt >nul
if exist nsis\rom2me%VER%.exe del /Q /F nsis\rom2me%VER%.exe >nul
if exist nsis\templates%VER%.bin del /Q /F nsis\templates%VER%.bin >nul
if exist nsis\allods2.cfg del /Q /F nsis\allods2.cfg >nul
if exist nsis\smackw32.dll del /Q /F nsis\smackw32.dll >nul
if exist nsis\curllib.dll del /Q /F nsis\curllib.dll >nul
if exist nsis\openldap.dll del /Q /F nsis\openldap.dll >nul
if exist nsis\SDL.dll del /Q /F nsis\SDL.dll >nul
if exist nsis\SDL_image.dll del /Q /F nsis\SDL_image.dll >nul
if exist nsis\libpng12-0.dll del /Q /F nsis\libpng12-0.dll >nul
if exist nsis\jpeg.dll del /Q /F nsis\jpeg.dll >nul
if exist nsis\zlib1.dll del /Q /F nsis\zlib1.dll >nul
if exist nsis\libtiff-3.dll del /Q /F nsis\libtiff-3.dll >nul
if exist nsis\libeay32.dll del /Q /F nsis\libeay32.dll >nul
if exist nsis\ssleay32.dll del /Q /F nsis\ssleay32.dll >nul
if exist nsis\libsasl.dll del /Q /F nsis\libsasl.dll >nul
if exist nsis\vcredist_x86.exe del /Q /F nsis\vcredist_x86.exe >nul

if exist "nsis\Description Checks.ini" del /Q /F "nsis\Description Checks.ini" >nul
if exist "nsis\Description Instants.ini" del /Q /F "nsis\Description Instants.ini" >nul
if exist "nsis\Music.ini" del /Q /F "nsis\Music.ini" >nul

call log --------------------------------------
