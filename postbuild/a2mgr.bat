@echo off

set VER=30

if not exist final mkdir final >nul
if not exist nsis mkdir nsis >nul
if exist release del /F /Q /S release >nul
if exist release rmdir release >nul
mkdir release >nul

call log building allods2com.exe
if exist allods2com.exe del /Q /F allods2com.exe >nul

call log building patch.res...
if exist patch.res del /Q /F patch.res >nul
if exist patch_en.res del /Q /F patch_en.res >nul
res ar patch.res patch patch >nul
copy /Y patch.res patch_en.res >nul

if exist tmp rmdir /S/Q tmp
mkdir tmp
mkdir tmp\data

call log building world.res...
copy world0.res world.res >nul
if exist world2.res del /Q /F world2.res >nul
if exist world.res del /Q /F world.res >nul
copy /Y world\ai.reg tmp\data\ai.reg >nul
databin2xml.exe world\data.xml world\data.bin
copy /Y world\data.bin tmp\data\data.bin >nul
del /Q /F world\data.bin >nul
itemname2xml.exe --reverse tmp\data\itemname.bin tmp\data\itemname.pkt locale\en\itemserv.txt locale\en\itemname.txt world\data.xml world\itemname-en.xml
itemname2xml.exe --reverse tmp\data\itemname.bin tmp\data\itemname.pkt locale\ru\itemserv.txt locale\ru\itemname.txt world\data.xml world\itemname-ru.xml
copy /Y world\map.reg tmp\data\map.reg >nul
res ar world2.res world2 tmp >nul
move /Y world2.res world.res >nul
:skipworld

goto skipworlds
call log building worlds.res...
if exist worlds.res del /Q /F worlds.res >nul
copy world0.res worlds.res >nul
res.exe a worlds.res worlds\data world.single\ai.reg >nul
databin2xml.exe world.single\data.xml world.single\data.bin
res.exe a worlds.res worlds\data world.single\data.bin >nul
del /Q /F world.single\data.bin >nul
res.exe a worlds.res worlds\data world.single\itemname.bin >nul
res.exe a worlds.res worlds\data world.single\itemname.pkt >nul
res.exe a worlds.res worlds\data world.single\map.reg >nul
:skipworlds

call log building locale.res...
if exist locale.res del /Q /F locale.res > nul
res.exe ar locale.res locale locale >nul

copy original\allods2.exe allods2.exe >nul
call log adding section .anet...
add_sect allods2.exe .anet 60000060 4000 >nul
call log patching...
for /f %%A in (list.txt) do call p patches\%%A allods2.exe
call log adding a2mgr...
add_dll allods2.exe a2mgr.dll allods2.mp >nul
if errorlevel 1 goto add_dll_fail

rem call log vmprotecting dll...
rem copy /Y a2mgr.dll a2mgr.dll2
rem vmprotect_con a2mgr.dll
rem copy /Y a2mgr.dll a2mgr.vmp.dll >nul
rem if errorlevel 1 goto vmp_fail

move /Y allods2.exe release >nul
move /Y patch.res release >nul
move /Y patch_en.res release >nul
move /Y world.res release >nul
rem move /Y worlds.res release >nul
copy /Y whatsnew.txt release\patch%VER%.txt >nul
copy /Y whatsnew_en.txt release\patch%VER%_en.txt >nul
move /Y a2mgr.dll release\a2mgr.dll >nul
copy /Y add\helper32.dll release >nul
copy /Y locale.res release >nul
copy /Y allods2.cfg release >nul
copy /Y allods2_en.cfg release >nul
copy /Y ..\curl\src\DLL-Release\curllib.dll release >nul
copy /Y ..\curl\src\DLL-Release\openldap.dll release >nul
copy /Y SDL\* release >nul
copy /Y ..\curl\libeay32.dll release >nul
copy /Y ..\curl\ssleay32.dll release >nul
copy /Y ..\curl\libsasl.dll release >nul

copy /Y add\smackw32.dll release >nul
copy /Y add\templates.bin release\templates.bin >nul

rem call nsis.bat %VER%
rem if errorlevel 1 goto nsis_fail

call log cleaning up...
del /Q a2mgr.dll
del /Q locale.res

call log done!

exit 0

:patch_fail
call log patching fail failed!
pause
exit 1

:vmp_fail
call log vmpprotect failed!
pause
exit 1

:nsis_fail
call log nsis failed!
pause
exit 1

:add_dll_fail
call log add_dll failed!
pause
exit 1
