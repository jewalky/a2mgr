;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  !include "MUI_EXTRAPAGES.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Аллоды II"
  OutFile "final\a2patch${VER}_ru.exe"

  ;Default installation folder
  InstallDir "C:\Allods 2"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\1C\Allods 2" "INSTALLDIR"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin ; главным образом из-за записи в HKLM

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

!include FileFunc.nsh
!include LogicLib.nsh
!include TextFunc.nsh
!include WinMessages.nsh
!include WordFunc.nsh
!include MUI.nsh
  !insertmacro GetParameters
  !insertmacro GetOptions

;--------------------------------
;Init
Function .onInit
  ${GetParameters} $R0
  ClearErrors
  ${GetOptions} $R0 /EXECUTE= $0
FunctionEnd

;--------------------------------
;Close
Function .onInstSuccess
  Exec '"$INSTDIR\$0"'
FunctionEnd

;--------------------------------
;Pages
Function DirectoryLeave
  IfSilent GrCont
  IfFileExists "$INSTDIR\graphics.res" GrCont GrNoGraphics
GrNoGraphics:
  MessageBox MB_OK "Выберите папку с установленной игрой Аллоды II."
  Abort
GrCont:
FunctionEnd

  !insertmacro MUI_PAGE_README "nsis\patch${VER}.txt"
  !insertmacro MUI_PAGE_COMPONENTS

  !define MUI_DIRECTORYPAGE_VERIFYONLEAVE
  !define MUI_PAGE_CUSTOMFUNCTION_LEAVE DirectoryLeave
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\1C\Allods 2"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "Russian"

  ;Set up install lang strings for 1st lang
  ${ReadmeLanguage} "${LANG_RUSSIAN}" \
          "Информация" \
          "Пожалуйста, просмотрите следующую информацию." \
          "Об $(^name):" \
          "$\n  Используйте кнопки прокрутки или клавишу Page Down для просмотра всего текста."

;--------------------------------
;Installer Sections

Section "Обновление ${VER} (для уже установленной игры)" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  SetOverwrite on

File "nsis\a2mgr.dll"
File "nsis\smackw32.dll"
File "nsis\allods2.cfg"
File "nsis\allods2.exe"
File "nsis\helper32.dll"
File "nsis\patch.res"
File "nsis\patch${VER}.txt"
File "nsis\locale.res"
File "nsis\world.res"
File "nsis\SDL.dll"
File "nsis\SDL_image.dll"
File "nsis\libpng12-0.dll"
File "nsis\jpeg.dll"
File "nsis\zlib1.dll"
File "nsis\libtiff-3.dll"

  CreateDirectory "$INSTDIR\maps"
  
  ;Store installation folder
  WriteRegStr HKLM "Software\1C\Allods 2" "INSTALLDIR" $INSTDIR

 ;Store compatibility mode
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\allods2.exe" "WIN95"
 
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall1.${VER}.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Играть на Allods2.eu (1024x768).lnk" "$INSTDIR\allods2.exe" "--res:1024x768"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Играть на Allods2.eu (800x600).lnk" "$INSTDIR\allods2.exe" "--res:800x600"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Информация об обновлении ${VER}.lnk" "$INSTDIR\patch${VER}.txt"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Деинсталлировать обновление ${VER}.lnk" "$INSTDIR\Uninstall1.${VER}.exe"

    CreateShortCut "$DESKTOP\Играть на Allods2.eu (1024x768).lnk" "$INSTDIR\allods2.exe" "--res:1024x768"
    CreateShortCut "$DESKTOP\Играть на Allods2.eu (800x600).lnk" "$INSTDIR\allods2.exe" "--res:800x600"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_RUSSIAN} "Обновление Аллодов II для игры на хете Allods2.eu."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\a2mgr${VER}.dll"
  Delete "$INSTDIR\smackw32.dll"
  Delete "$INSTDIR\allods2.conf"
  Delete "$INSTDIR\allods2com${VER}.exe"
  Delete "$INSTDIR\helper32.dll"
  Delete "$INSTDIR\patch${VER}.res"
  Delete "$INSTDIR\patch${VER}.txt"
  Delete "$INSTDIR\rom2me${VER}.exe"
  Delete "$INSTDIR\templates${VER}.bin"
  Delete "$INSTDIR\world${VER}.res"
  Delete "$INSTDIR\world${VER}s.res"
  Delete "$INSTDIR\Description Checks.ini"
  Delete "$INSTDIR\Description Instants.ini"
  Delete "$INSTDIR\Music.ini"

  Delete "$INSTDIR\Uninstall1.${VER}.exe"

  RMDir "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Играть на Allods2.eu (1024x768).lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Играть на Allods2.eu (800x600).lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Информация об обновлении ${VER}.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Деинсталлировать обновление ${VER}.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  

    Delete "$DESKTOP\Играть на Allods2.eu (1024x768).lnk"
    Delete "$DESKTOP\Играть на Allods2.eu (800x600).lnk"

    DeleteRegKey /ifempty HKLM "Software\1C\Allods 2"

SectionEnd
