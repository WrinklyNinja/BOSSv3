﻿;BOSS NSIS Installer Script

;--------------------------------
;Include NSIS files.

	!include "MUI2.nsh"
    !include "x64.nsh"
	!include "LogicLib.nsh"
	!include "nsDialogs.nsh"

;--------------------------------
; Helper Function

Function ReplaceLineStr
 Exch $R0 ; string to replace that whole line with
 Exch
 Exch $R1 ; string that line should start with
 Exch
 Exch 2
 Exch $R2 ; file
 Push $R3 ; file handle
 Push $R4 ; temp file
 Push $R5 ; temp file handle
 Push $R6 ; global
 Push $R7 ; input string length
 Push $R8 ; line string length
 Push $R9 ; global

  StrLen $R7 $R1

  GetTempFileName $R4

  FileOpen $R5 $R4 w
  FileOpen $R3 $R2 r

  ReadLoop:
  ClearErrors
   FileRead $R3 $R6
    IfErrors Done

   StrLen $R8 $R6
   StrCpy $R9 $R6 $R7 -$R8
   StrCmp $R9 $R1 0 +3

    FileWrite $R5 "$R0$\r$\n"
    Goto ReadLoop

    FileWrite $R5 $R6
    Goto ReadLoop

  Done:

  FileClose $R3
  FileClose $R5

  SetDetailsPrint none
   Delete $R2
   Rename $R4 $R2
  SetDetailsPrint both

 Pop $R9
 Pop $R8
 Pop $R7
 Pop $R6
 Pop $R5
 Pop $R4
 Pop $R3
 Pop $R2
 Pop $R1
 Pop $R0
FunctionEnd

;--------------------------------
;General

	;Name, file and version info for installer.
	Name "BOSS v3.0.0"
	OutFile "..\build\BOSS Installer.exe"
	VIProductVersion 3.0.0.0

	;Request application privileges for Windows Vista/7
	RequestExecutionLevel admin

	;Icon for installer\uninstaller
	!define MUI_ICON "..\resources\icon.ico"
	!define MUI_UNICON "..\resources\icon.ico"

	; This causes an "are you sure?" message to be displayed if you try to quit the installer or uninstaller.
	!define MUI_ABORTWARNING
	!define MUI_UNABORTWARNING

	;Checks that the installer's CRC is correct (means we can remove installer CRC checking from BOSS).
	CRCCheck force

	;The SOLID lzma compressor gives the best compression ratio.
	SetCompressor /SOLID lzma

    ;Default install directory.
    InstallDir "$PROGRAMFILES\BOSS"
    InstallDirRegKey HKLM "Software\BOSS" "Installed Path"

;--------------------------------
;Pages

	!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	!define MUI_FINISHPAGE_NOAUTOCLOSE
	!define MUI_FINISHPAGE_RUN "$INSTDIR\BOSS.exe"
	!define MUI_FINISHPAGE_RUN_TEXT "$(Text_Run)"
	!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Docs\BOSS Readme.html"
	!define MUI_FINISHPAGE_SHOWREADME_TEXT "$(Text_ShowReadme)"
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_COMPONENTS
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

	!insertmacro MUI_LANGUAGE "English"
	!insertmacro MUI_LANGUAGE "Russian"
	;!insertmacro MUI_LANGUAGE "German"
	!insertmacro MUI_LANGUAGE "Spanish"
	!insertmacro MUI_LANGUAGE "SimpChinese"
	!insertmacro MUI_LANGUAGE "French"
	!insertmacro MUI_LANGUAGE "Polish"
	!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;English Strings

	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "BOSS"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2009-2014 BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Installer for BOSS 3.0.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "3.0.0"

	LangString TEXT_MESSAGEBOX ${LANG_ENGLISH} "BOSS is already installed, and must be uninstalled before continuing. $\n$\nClick `OK` to remove the previous version or `Cancel` to cancel this upgrade."
	LangString TEXT_RUN ${LANG_ENGLISH} "Run BOSS"
	LangString TEXT_SHOWREADME ${LANG_ENGLISH} "View Readme"
	LangString TEXT_MAIN ${LANG_ENGLISH} "All BOSS's files, minus userlists and settings file."
	LangString TEXT_USERFILES ${LANG_ENGLISH} "BOSS's userlist files and settings file."

;--------------------------------
;Russian (Русский) Strings

	VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductName" "BOSS"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "CompanyName" "BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalCopyright" "© 2009-2014 BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileDescription" "Установщик для BOSS 3.0.0"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileVersion" "3.0.0"

	LangString TEXT_MESSAGEBOX ${LANG_RUSSIAN} "BOSS уже установлен и должен быть удален перед продолжением. $\n$\nНажмите `OK` для удаления предыдущей версии или `Отмена` для отмены обновления."
	LangString TEXT_RUN ${LANG_RUSSIAN} "Запустить BOSS"
	LangString TEXT_SHOWREADME ${LANG_RUSSIAN} "Просмотр документации"
	LangString TEXT_MAIN ${LANG_RUSSIAN} "Все файлы BOSS, кроме пользовательских списков и файла настроек"
	LangString TEXT_USERFILES ${LANG_RUSSIAN} "Файлы пользовательских списков BOSS и файл настроек."

;--------------------------------
;German (Deutsch) Strings

	;VIAddVersionKey /LANG=${LANG_GERMAN} "ProductName" "BOSS"
	;VIAddVersionKey /LANG=${LANG_GERMAN} "CompanyName" "BOSS Development Team"
	;VIAddVersionKey /LANG=${LANG_GERMAN} "LegalCopyright" "© 2009-2014 BOSS Development Team"
	;VIAddVersionKey /LANG=${LANG_GERMAN} "FileDescription" "Installer für BOSS 3.0.0"
	;VIAddVersionKey /LANG=${LANG_GERMAN} "FileVersion" "3.0.0"

	;LangString TEXT_MESSAGEBOX ${LANG_GERMAN} "BOSS ist bereits installiert und muss deinstalliert werden, bevor fortgefahren wird. $\n$\nKlicke auf `Ok` um die vorherige Version zu entfernen oder auf `Abbrechen` um das Upgrade abzubrechen."
	;LangString TEXT_RUN ${LANG_GERMAN} "BOSS starten"
	;LangString TEXT_SHOWREADME ${LANG_GERMAN} "Readme lesen"
	;LangString TEXT_MAIN ${LANG_GERMAN} "Alle Dateien von BOSS ohne die Benutzerlisten und die BOSS.ini."
	;LangString TEXT_USERFILES ${LANG_GERMAN} "Benutzerliste von BOSS und die BOSS.ini-Datei."

;--------------------------------
;French (Français) Strings

	VIAddVersionKey /LANG=${LANG_FRENCH} "ProductName" "BOSS"
	VIAddVersionKey /LANG=${LANG_FRENCH} "CompanyName" "BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_FRENCH} "LegalCopyright" "© 2009-2014 BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_FRENCH} "FileDescription" "Programme d'installation pour BOSS 3.0.0"
	VIAddVersionKey /LANG=${LANG_FRENCH} "FileVersion" "3.0.0"

	LangString TEXT_MESSAGEBOX ${LANG_FRENCH} "BOSS est déjà installé, et doit être désinstallé avant de continuer. $\n$\nCliquer sur `OK` pour désinstaller la version précédente ou 'Annuler' pour annuler la mise à jour."
	LangString TEXT_RUN ${LANG_FRENCH} "Lancer BOSS"
	LangString TEXT_SHOWREADME ${LANG_FRENCH} "Afficher la documentation"
	LangString TEXT_MAIN ${LANG_FRENCH} "Tous les fichiers de BOSS, sauf les userlists et les règlages."
	LangString TEXT_USERFILES ${LANG_FRENCH} "L'userlist de BOSS et les règlages."

;--------------------------------
;Spanish (castellano) Strings

	VIAddVersionKey /LANG=${LANG_SPANISH} "ProductName" "BOSS"
	VIAddVersionKey /LANG=${LANG_SPANISH} "CompanyName" "BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_SPANISH} "LegalCopyright" "© 2009-2014 BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_SPANISH} "FileDescription" "El instalador para BOSS 3.0.0"
	VIAddVersionKey /LANG=${LANG_SPANISH} "FileVersion" "3.0.0"

	LangString TEXT_MESSAGEBOX ${LANG_SPANISH} "BOSS está instalado, y debe ser desinstalado antes de continuar. $\n$\nPresione `OK` para eliminar la versión anterior o `Cancel` para cancelar la actualización."
	LangString TEXT_RUN ${LANG_SPANISH} "Ejecutar BOSS"
	LangString TEXT_SHOWREADME ${LANG_SPANISH} "Ver Léame"
	LangString TEXT_MAIN ${LANG_SPANISH} "Todos los archivos de BOSS, menos la lista de usuario y configuraciónes."
	LangString TEXT_USERFILES ${LANG_SPANISH} "La lista de usuario y configuraciónes."

;--------------------------------
;Simplified Chinese (简体中文) Strings

    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductName" "BOSS"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "CompanyName" "BOSS Development Team"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "LegalCopyright" "© 2009-2014 BOSS Development Team"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileDescription" "BOSS 3.0.0安装包"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileVersion" "3.0.0"

    LangString TEXT_MESSAGEBOX ${LANG_SIMPCHINESE} "检测到旧版BOSS，您需要先卸载旧版才能安装新版。$\n$\n点击“确定”卸载旧版本或者“取消”取消更新。"
    LangString TEXT_RUN ${LANG_SIMPCHINESE} "运行BOSS"
    LangString TEXT_SHOWREADME ${LANG_SIMPCHINESE} "查看说明"
    LangString TEXT_MAIN ${LANG_SIMPCHINESE} "所有BOSS文件（除userlist和配置文件）"
    LangString TEXT_USERFILES ${LANG_SIMPCHINESE} "BOSS的userlist和配置文件。"
    
;--------------------------------
;Polish (POLSKI) Strings

	VIAddVersionKey /LANG=${LANG_POLISH} "ProductName" "BOSS"
	VIAddVersionKey /LANG=${LANG_POLISH} "CompanyName" "BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_POLISH} "LegalCopyright" "© 2009-2014 BOSS Development Team"
	VIAddVersionKey /LANG=${LANG_POLISH} "FileDescription" "Instalator dla BOSS 3.0.0"
	VIAddVersionKey /LANG=${LANG_POLISH} "FileVersion" "3.0.0"

	LangString TEXT_MESSAGEBOX ${LANG_POLISH} "BOSS jest już zainstalowany i musi zostać odinstalowany przed instalowaniem tej wersji. $\n$\nClick `OK` aby odinstalować lub `Cancel` aby anulować aktualizację."
	LangString TEXT_RUN ${LANG_POLISH} " Uruchom BOSS"
	LangString TEXT_SHOWREADME ${LANG_POLISH} "Czytaj Readme"
	LangString TEXT_MAIN ${LANG_POLISH} "Wszystkie pliki BOSS bez ustawień i plików użytkownika."
	LangString TEXT_USERFILES ${LANG_POLISH} "Wszystkie pliki BOSS oraz ustawienia użytkownika ."

;--------------------------------
;Initialisations

    Var InstallPath ;Path to existing BOSS install.

	Function .onInit

		!insertmacro MUI_LANGDLL_DISPLAY

	FunctionEnd

	Function onGUIInit
		; First check to see if BOSS is already installed via installer, and launch the existing uninstaller if so.
		ReadRegStr $InstallPath HKLM "Software\BOSS" "Installed Path"
		${If} $InstallPath != ""
			IfFileExists "$InstallPath\Uninstall.exe" 0 +8
				MessageBox MB_OKCANCEL|MB_ICONQUESTION "$(Text_MessageBox)" IDOK cont IDCANCEL cancel
				cancel:
					Quit
				cont:
					ExecWait '$InstallPath\Uninstall.exe _?=$InstallPath' ;Run the uninstaller in its folder and wait until it's done.
					Delete "$InstallPath\Uninstall.exe"
					RMDir "$InstallPath"
		${EndIf}
	FunctionEnd

	Function un.onInit

		!insertmacro MUI_LANGDLL_DISPLAY

	FunctionEnd

;--------------------------------
;Installer Sections

	Section "Installer Section"

		;Install executable.
		SetOutPath "$INSTDIR"
		File "..\build\BOSS.exe"

		;Now install API DLLs.
		;SetOutPath "$INSTDIR\API"
		;File "..\build\libboss32.dll"
		;File "..\build\libboss64.dll"

        ;Install resource files.
        SetOutPath "$INSTDIR\resources"
        File "..\resources\polyfill.js"
        File "..\resources\script.js"
        File "..\resources\style.css"

        ;Install documentation images.
		SetOutPath "$INSTDIR\docs\images"
        File "..\docs\images\*"

        ;Install licenses.
		SetOutPath "$INSTDIR\docs\licenses"
        File "..\docs\licenses\*"

		;Now install readme files.
		SetOutPath "$INSTDIR\docs"
		;File "..\docs\BOSS API Readme.html"
		File "..\docs\BOSS Metadata Syntax.html"
		File "..\docs\BOSS Readme.html"

		;Now install language files.
		SetOutPath "$INSTDIR\resources\l10n\ru\LC_MESSAGES"
		File "..\resources\l10n\ru\LC_MESSAGES\boss.mo"
		File "..\resources\l10n\ru\LC_MESSAGES\wxstd.mo"
		SetOutPath "$INSTDIR\resources\l10n\es\LC_MESSAGES"
		File "..\resources\l10n\es\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\es\LC_MESSAGES\boss.mo"
		;SetOutPath "$INSTDIR\resources\l10n\de\LC_MESSAGES"
		;File "..\resources\l10n\de\LC_MESSAGES\wxstd.mo"
		SetOutPath "$INSTDIR\resources\l10n\zh_CN\LC_MESSAGES"
		File "..\resources\l10n\zh_CN\LC_MESSAGES\boss.mo"
		File "..\resources\l10n\zh_CN\LC_MESSAGES\wxstd.mo"
		SetOutPath "$INSTDIR\resources\l10n\fr\LC_MESSAGES"
		File "..\resources\l10n\fr\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\fr\LC_MESSAGES\boss.mo"
		SetOutPath "$INSTDIR\resources\l10n\pl\LC_MESSAGES"
		File "..\resources\l10n\pl\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\pl\LC_MESSAGES\boss.mo"
		SetOutPath "$INSTDIR\resources\l10n\pt_BR\LC_MESSAGES"
		File "..\resources\l10n\pt_BR\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\pt_BR\LC_MESSAGES\boss.mo"
		
        ;Install settings file.
        SetOutPath "$LOCALAPPDATA\BOSS"
        File "..\resources\settings.yaml"

		;Write language setting to settings.yaml.
		StrCmp $LANGUAGE ${LANG_RUSSIAN} 0 +5
            Push "$LOCALAPPDATA\BOSS\settings.yaml"
            Push "Language:"
            Push "Language: ru"
            Call ReplaceLineStr
		;StrCmp $LANGUAGE ${LANG_GERMAN} 0 +5
        ;    Push "$LOCALAPPDATA\BOSS\settings.yaml"
        ;    Push "Language:"
        ;    Push "Language: de"
        ;    Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_SPANISH} 0 +5
            Push "$LOCALAPPDATA\BOSS\settings.yaml"
            Push "Language:"
            Push "Language: es"
            Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_SIMPCHINESE} 0 +5
            Push "$LOCALAPPDATA\BOSS\settings.yaml"
            Push "Language:"
            Push "Language: zh_CN"
            Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_FRENCH} 0 +5
            Push "$LOCALAPPDATA\BOSS\settings.yaml"
            Push "Language:"
            Push "Language: fr"
            Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_POLISH} 0 +5
            Push "$LOCALAPPDATA\BOSS\settings.yaml"
            Push "Language:"
            Push "Language: pl"
            Call ReplaceLineStr
		;StrCmp $LANGUAGE ${LANG_PORTUGUESEBR} 0 +5
        ;    Push "$LOCALAPPDATA\BOSS\settings.yaml"
        ;    Push "Language:"
        ;    Push "Language: pt_BR"
        ;    Call ReplaceLineStr
			
		;Add Start Menu shortcuts. Set out path back to $INSTDIR otherwise the shortcuts start in the wrong place.
		;Set Shell Var Context to all so that shortcuts are installed for all users, not just admin.
		SetOutPath "$INSTDIR"
		SetShellVarContext all
		CreateDirectory "$SMPROGRAMS\BOSS"
		CreateShortCut "$SMPROGRAMS\BOSS\BOSS.lnk" "$INSTDIR\BOSS.exe"
		CreateShortCut "$SMPROGRAMS\BOSS\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
		CreateShortCut "$SMPROGRAMS\BOSS\Readme.lnk" "$INSTDIR\Docs\BOSS Readme.html"


		;Store installation folder in registry key.
		WriteRegStr HKLM "Software\BOSS" "Installed Path" "$INSTDIR"
		;Write registry keys for Windows' uninstaller.
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "DisplayName" "BOSS"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "UninstallString" '"$INSTDIR\Uninstall.exe"'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "URLInfoAbout" 'http://boss-developers.github.io/'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "HelpLink" 'http://boss-developers.github.io/'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "Publisher" 'BOSS Development Team'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "DisplayVersion" '3.0.0'
		WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "NoModify" 1
		WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS" "NoRepair" 1

		;Create uninstaller
		WriteUninstaller "$INSTDIR\Uninstall.exe"

	SectionEnd
    
    Section "Microsoft Visual C++ 2013 SP1 Redist"
        ; Thanks to the pcsx2 installer for providing this!

        ; Detection made easy: Unlike previous redists, VC2013 now generates a platform
        ; independent key for checking availability.
        ; HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\12.0\VC\Runtimes\x86  for x64 Windows
        ; HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\12.0\VC\Runtimes\x86  for x86 Windows

        ; Download from:
        ; http://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x86.exe

        ClearErrors

        ${If} ${RunningX64}
        ReadRegDword $R0 HKLM "SOFTWARE\Wow6432Node\Microsoft\VisualStudio\12.0\VC\Runtimes\x86" "Installed"
        ${Else}
        ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\VisualStudio\12.0\VC\Runtimes\x86" "Installed"
        ${EndIf}

        IfErrors 0 +2
        DetailPrint "Visual C++ 2013 Redistributable registry key was not found; assumed to be uninstalled."
        StrCmp $R0 "1" 0 +3
        DetailPrint "Visual C++ 2013 Redistributable is already installed; skipping!"
        Goto done

        ; Use bundled VC redist, more important that the install doesn't fail than it gets whatever is the latest update.
        SetOutPath "$TEMP"
        File "..\build\vcredist_x86.exe"

        ;DetailPrint "Downloading Visual C++ 2013 Redistributable Setup..."
        ;DetailPrint "Contacting Microsoft.com..."
        ;NSISdl::download /TIMEOUT=15000 "http://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x86.exe" "vcredist_x86.exe"

        ;Pop $R0 ;Get the return value
        ;StrCmp $R0 "success" OnSuccess
        ;DetailPrint "Could not contact Microsoft.com, or the file has been (re)moved!"
        ;Goto done

        ;OnSuccess:
        DetailPrint "Running Visual C++ 2013 Redistributable Setup..."
        ExecWait '"$TEMP\vcredist_x86.exe" /qb'
        DetailPrint "Finished Visual C++ 2013 SP1 Redistributable Setup"

        Delete "$TEMP\vcredist_x86.exe"

        done:
    SectionEnd

;--------------------------------
;Uninstaller Section


	Section "un.BOSS" Main

		;Remove main executables.
		Delete "$INSTDIR\BOSS.exe"

		;Remove API DLLs.
		;Delete "$INSTDIR\API\libboss32.dll"
		;Delete "$INSTDIR\API\libboss64.dll"
		;RMDir  "$INSTDIR\API"

		;Remove readme images.
		RMDir /r "$INSTDIR\docs\images"

        ;Remove licenses.
        RMDir /r "$INSTDIR\docs\licenses"

		;Remove readme files.
		;Delete "$INSTDIR\docs\BOSS API Readme.html"
		Delete "$INSTDIR\docs\BOSS Metadata Syntax.html"
		Delete "$INSTDIR\docs\BOSS Readme.html"
		RMDir  "$INSTDIR\docs"

        ;Remove resource files.
        Delete "$INSTDIR\resources\polyfill.js"
        Delete "$INSTDIR\resources\script.js"
        Delete "$INSTDIR\resources\style.css"

		;Remove language files.
		Delete "$INSTDIR\resources\l10n\ru\LC_MESSAGES\boss.mo"
		Delete "$INSTDIR\resources\l10n\ru\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\es\LC_MESSAGES\boss.mo"
		Delete "$INSTDIR\resources\l10n\es\LC_MESSAGES\wxstd.mo"
		;Delete "$INSTDIR\resources\l10n\de\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\zh_CN\LC_MESSAGES\boss.mo"
		Delete "$INSTDIR\resources\l10n\zh_CN\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\fr\LC_MESSAGES\boss.mo"
		Delete "$INSTDIR\resources\l10n\fr\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\pl\LC_MESSAGES\boss.mo"
		Delete "$INSTDIR\resources\l10n\pl\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\pt_BR\LC_MESSAGES\boss.mo"
		Delete "$INSTDIR\resources\l10n\pt_BR\LC_MESSAGES\wxstd.mo"
		RMDir  "$INSTDIR\resources\l10n\ru\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\ru"
		RMDir  "$INSTDIR\resources\l10n\es\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\es"
		;RMDir  "$INSTDIR\resources\l10n\de\LC_MESSAGES"
		;RMDir  "$INSTDIR\resources\l10n\de"
		RMDir  "$INSTDIR\resources\l10n\zh_CN\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\zh_CN"
		RMDir  "$INSTDIR\resources\l10n\fr\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\fr"
		RMDir  "$INSTDIR\resources\l10n\pl\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\pl"
		RMDir  "$INSTDIR\resources\l10n\pt_BR\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\pt_BR"	
		RMDir  "$INSTDIR\resources\l10n"
        RMDir  "$INSTDIR\resources"

		;Now we have to remove the files BOSS generates when it runs.
		Delete "$LOCALAPPDATA\BOSS\BOSSDebugLog.txt"
		;Trying to delete a file that doesn't exist doesn't cause an error, so delete all games' files.
        ;This doesn't handle user-defined games.
		Delete "$LOCALAPPDATA\BOSS\Oblivion\report.html"
		Delete "$LOCALAPPDATA\BOSS\Oblivion\masterlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\Nehrim\report.html"
		Delete "$LOCALAPPDATA\BOSS\Nehrim\masterlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\Skyrim\report.html"
		Delete "$LOCALAPPDATA\BOSS\Skyrim\masterlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\Fallout3\report.html"
		Delete "$LOCALAPPDATA\BOSS\Fallout3\masterlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\FalloutNV\report.html"
		Delete "$LOCALAPPDATA\BOSS\FalloutNV\masterlist.yaml"
		RMDir  "$LOCALAPPDATA\BOSS\Oblivion"
		RMDir  "$LOCALAPPDATA\BOSS\Nehrim"
		RMDir  "$LOCALAPPDATA\BOSS\Skyrim"
		RMDir  "$LOCALAPPDATA\BOSS\Fallout3"
		RMDir  "$LOCALAPPDATA\BOSS\FalloutNV"
        RMDir  "$LOCALAPPDATA\BOSS"

		;Remove uninstaller.
		Delete "$INSTDIR\Uninstall.exe"

		;Remove install directory.
		RMDir "$INSTDIR"

		;Delete registry key.
		DeleteRegKey HKLM "Software\BOSS"

		;Delete stupid Windows created registry keys:
		DeleteRegKey HKCU "Software\BOSS"
		DeleteRegKey HKLM "Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\BOSS"
		DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BOSS"
		DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\App Management\ARPCache\BOSS"
		DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR"
		DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\BOSS.exe"
		DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\Uninstall.exe"
		DeleteRegValue HKCU "Software\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR"
		DeleteRegValue HKCU "Software\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\BOSS.exe"
		DeleteRegValue HKCU "Software\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\Uninstall.exe"
		DeleteRegValue HKCU "Software\Microsoft\Windows\ShellNoRoam\MuiCache" "$INSTDIR"
		DeleteRegValue HKCU "Software\Microsoft\Windows\ShellNoRoam\MuiCache" "$INSTDIR\BOSS.exe"
		DeleteRegValue HKCU "Software\Microsoft\Windows\ShellNoRoam\MuiCache" "$INSTDIR\Uninstall.exe"

		;Delete Start Menu folder.
		SetShellVarContext all
		RMDir /r "$SMPROGRAMS\BOSS"

	SectionEnd

	Section /o "un.User Files" UserFiles
		;The following user files are only removed if set to.
		Delete "$LOCALAPPDATA\BOSS\settings.yaml"
		Delete "$LOCALAPPDATA\BOSS\Oblivion\userlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\Nehrim\userlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\Skyrim\userlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\Fallout3\userlist.yaml"
		Delete "$LOCALAPPDATA\BOSS\FalloutNV\userlist.yaml"
		;Also try removing the folders storing them, in case they are otherwise empty.
		RMDir  "$LOCALAPPDATA\BOSS\Oblivion"
		RMDir  "$LOCALAPPDATA\BOSS\Nehrim"
		RMDir  "$LOCALAPPDATA\BOSS\Skyrim"
		RMDir  "$LOCALAPPDATA\BOSS\Fallout3"
		RMDir  "$LOCALAPPDATA\BOSS\FalloutNV"
		;Try removing install directory.
        RMDir  "$LOCALAPPDATA\BOSS"
	SectionEnd

;--------------------------------
;Languages - Description Strings

	!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
	  !insertmacro MUI_DESCRIPTION_TEXT ${Main} "$(Text_Main)"
	  !insertmacro MUI_DESCRIPTION_TEXT ${UserFiles} "$(Text_UserFiles)"
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_END

;--------------------------------
