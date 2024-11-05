!include "MUI2.nsh"
!include "x64.nsh"

; The name of the installer
Name "Hypercube"
; Program version
!define VERSION "1.7.2"

; The file to write
OutFile "Hypercube-${VERSION}.exe"

RequestExecutionLevel admin

; The default installation directory
InstallDir "$PROGRAMFILES\Hypercube"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Hypercube" "Install_Dir"

; Registry key for uninstaller
!define REGENTRY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hypercube" 

; Start menu page configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
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
  WriteRegStr HKLM SOFTWARE\Hypercube "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "${REGENTRY}" "DisplayName" "Hypercube"
  WriteRegStr HKLM "${REGENTRY}" "Publisher" "Martin Tuma"
  WriteRegStr HKLM "${REGENTRY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${REGENTRY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "${REGENTRY}" "NoModify" 1
  WriteRegDWORD HKLM "${REGENTRY}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Create start menu entry and add links
  SetShellVarContext all
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

  File "Qt5Core.dll"
  File "Qt5Gui.dll"
  File "Qt5Widgets.dll"
  File "libGLESv2.dll" 
  File /r "platforms"
 
SectionEnd

Section "MSVC runtime" SEC_MSVC

  DetailPrint "Checking whether Visual C++ 2015 Redistributable is already installed..."
  ${If} ${RunningX64}
    ReadRegDword $R0 HKLM "SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" "Installed"
  ${Else}
    ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" "Installed"
  ${EndIf}

  StrCmp $R0 "1" 0 +3
  DetailPrint "Visual C++ 2015 Redistributable is already installed, skipping install."
  Goto done

  DetailPrint "Installing Visual C++ 2015 Redistributable..."
  SetOutPath $TEMP
  File "VC_redist.x86.exe"
  ExecWait '"$TEMP/VC_redist.x86.exe" /install /quiet /norestart'

  done:
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "${REGENTRY}"
  DeleteRegKey HKLM SOFTWARE\Hypercube

  ; Remove directories used
  RMDir /r "$INSTDIR"

  ; Remove Start menu entries
  SetShellVarContext all
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
  "Visual C++ 2015 runtime components. Unselct only if you have the runtime already installed!" 
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