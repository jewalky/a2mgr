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
  Name "Allods2.eu Online Client"
  OutFile "final\a2inst${VER}_en.exe"

  ;Default installation folder
  InstallDir "C:\ROM2"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\1C\Allods2.eu" "INSTALLDIR"

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
  !insertmacro MUI_PAGE_README "nsis\patch${VER}.txt"
  !insertmacro MUI_PAGE_COMPONENTS

  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\1C\Allods2.eu"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

  ;Set up install lang strings for 1st lang
  ${ReadmeLanguage} "${LANG_ENGLISH}" \
          "Information" \
          "Please, read the following information." \
          "About $(^name):" \
          "$\n  Use scrollbar or Page Down key to view the whole text."

;--------------------------------
;Installer Sections

Section "Allods2.eu Online Client (version ${VER})" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  SetOverwrite on

  File /r "nsis\*"

  CreateDirectory "$INSTDIR\maps"
  CreateDirectory "$INSTDIR\screenshots"
  
  ;Store installation folder
  WriteRegStr HKLM "Software\1C\Allods2.eu" "INSTALLDIR" $INSTDIR

 ;Store compatibility mode
  WriteRegStr HKCU "Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers" "$INSTDIR\allods2.exe" ""
 
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall1.${VER}.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Play at Allods2.eu (1024x768).lnk" "$INSTDIR\allods2.exe" "--res:1024x768"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Play at Allods2.eu (800x600).lnk" "$INSTDIR\allods2.exe" "--res:800x600"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Update details for version ${VER}.lnk" "$INSTDIR\patch${VER}.txt"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall1.${VER}.exe"

    CreateShortCut "$DESKTOP\Play at Allods2.eu (1024x768).lnk" "$INSTDIR\allods2.exe" "--res:1024x768"
    CreateShortCut "$DESKTOP\Play at Allods2.eu (800x600).lnk" "$INSTDIR\allods2.exe" "--res:800x600"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "Allods2.eu Online Client setup (version ${VER})"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  RMDir /r "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Play at Allods2.eu (1024x768).lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Play at Allods2.eu (800x600).lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Update details for version ${VER}.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  

    Delete "$DESKTOP\Play at Allods2.eu (1024x768).lnk"
    Delete "$DESKTOP\Play at Allods2.eu (800x600).lnk"

    DeleteRegKey /ifempty HKLM "Software\1C\Allods2.eu"

SectionEnd
