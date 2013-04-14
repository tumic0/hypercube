;Include Modern UI
!include "MUI2.nsh"

; The name of the installer
Name "Hypercube"

; The file to write
OutFile "install.exe"

RequestExecutionLevel user

; The default installation directory
InstallDir "$LOCALAPPDATA\Hypercube"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKCU "Software\Hypercube" "Install_Dir"

; Registry key for uninstaller
!define REGENTRY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hypercube" 

; Start menu page configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Hypercube" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Hypercube" 

Var StartMenuFolder

;--------------------------------

; Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "licence.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

; Languages
!insertmacro MUI_LANGUAGE "English"


; The stuff to install
Section "Hypercube (required)" SEC_GUI_APP

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "hypercube.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKCU SOFTWARE\Hypercube "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKCU "${REGENTRY}" "DisplayName" "Hypercube"
  WriteRegStr HKCU "${REGENTRY}" "Publisher" "Martin Tuma"
  WriteRegStr HKCU "${REGENTRY}" "DisplayVersion" "1.3.0"
  WriteRegStr HKCU "${REGENTRY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKCU "${REGENTRY}" "NoModify" 1
  WriteRegDWORD HKCU "${REGENTRY}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Create start menu entry and add links
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application  
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Hypercube.lnk" "$INSTDIR\hypercube.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "Hypercube-cli" SEC_CLI_APP

  File "hypercube-cli.exe"

  ; Add Start menu link
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application  
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Hypercube-cli.lnk" "$INSTDIR\hypercube-cli.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "QT libs" SEC_QT

  File "QtCore4.dll"
  File "QtGui4.dll"
 
SectionEnd

Section "MSVC runtime" SEC_MSVC

  File "msvcp100.dll"
  File "msvcr100.dll"

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKCU "${REGENTRY}"
  DeleteRegKey HKCU SOFTWARE\Hypercube

  ; Remove files and uninstaller
  Delete "$INSTDIR\*.*"

  ; Remove directories used
  RMDir "$INSTDIR"

  ; Remove Start menu entries
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
  RMDir "$SMPROGRAMS\$StartMenuFolder"  

SectionEnd

;-------------------------------

;Descriptions

;Language strings
LangString DESC_QT ${LANG_ENGLISH} \
  "QT Library. Unselct only if you have QT already installed!"
LangString DESC_MSVC ${LANG_ENGLISH} \
  "Visual C++ 2010 runtime components. Unselct only if you have the runtime already installed!"
LangString DESC_GUI_APP ${LANG_ENGLISH} \
  "Hypercube application"
LangString DESC_CLI_APP ${LANG_ENGLISH} \
  "Hypercube command line tool"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_QT} $(DESC_QT)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_MSVC} $(DESC_MSVC) 
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_GUI_APP} $(DESC_GUI_APP)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_CLI_APP} $(DESC_CLI_APP)
!insertmacro MUI_FUNCTION_DESCRIPTION_END