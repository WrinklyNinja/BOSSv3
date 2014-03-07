﻿;LOOT NSIS Installer Script

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
	Name "LOOT v0.5.0"
	OutFile "..\build\LOOT Installer.exe"
	VIProductVersion 0.5.0.0

	;Request application privileges for Windows Vista/7
	RequestExecutionLevel admin

	;Icon for installer\uninstaller
	!define MUI_ICON "..\resources\icon.ico"
	!define MUI_UNICON "..\resources\icon.ico"

	; This causes an "are you sure?" message to be displayed if you try to quit the installer or uninstaller.
	!define MUI_ABORTWARNING
	!define MUI_UNABORTWARNING

	;Checks that the installer's CRC is correct.
	CRCCheck force

	;The SOLID lzma compressor gives the best compression ratio.
	SetCompressor /SOLID lzma

    ;Default install directory.
    InstallDir "$PROGRAMFILES\LOOT"
    InstallDirRegKey HKLM "Software\LOOT" "Installed Path"

;--------------------------------
;Pages

	!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	!define MUI_FINISHPAGE_NOAUTOCLOSE
	!define MUI_FINISHPAGE_RUN "$INSTDIR\LOOT.exe"
	!define MUI_FINISHPAGE_RUN_TEXT "$(Text_Run)"
	!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Docs\LOOT Readme.html"
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

	VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "LOOT"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "LOOT Team"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2009-2014 LOOT Team"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Installer for LOOT 0.5.0"
	VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "0.5.0"

	LangString TEXT_MESSAGEBOX ${LANG_ENGLISH} "LOOT is already installed, and must be uninstalled before continuing. $\n$\nClick `OK` to remove the previous version or `Cancel` to cancel this upgrade."
	LangString TEXT_RUN ${LANG_ENGLISH} "Run LOOT"
	LangString TEXT_SHOWREADME ${LANG_ENGLISH} "View Readme"
	LangString TEXT_MAIN ${LANG_ENGLISH} "All LOOT's files, minus userlists and settings file."
	LangString TEXT_USERFILES ${LANG_ENGLISH} "LOOT's userlist files and settings file."

;--------------------------------
;Russian (Русский) Strings

	VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductName" "LOOT"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "CompanyName" "LOOT Team"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalCopyright" "© 2009-2014 LOOT Team"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileDescription" "Установщик для LOOT 0.5.0"
	VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileVersion" "0.5.0"

	LangString TEXT_MESSAGEBOX ${LANG_RUSSIAN} "LOOT уже установлен и должен быть удален перед продолжением. $\n$\nНажмите `OK` для удаления предыдущей версии или `Отмена` для отмены обновления."
	LangString TEXT_RUN ${LANG_RUSSIAN} "Запустить LOOT"
	LangString TEXT_SHOWREADME ${LANG_RUSSIAN} "Просмотр документации"
	LangString TEXT_MAIN ${LANG_RUSSIAN} "Все файлы LOOT, кроме пользовательских списков и файла настроек"
	LangString TEXT_USERFILES ${LANG_RUSSIAN} "Файлы пользовательских списков LOOT и файл настроек."

;--------------------------------
;French (Français) Strings

	VIAddVersionKey /LANG=${LANG_FRENCH} "ProductName" "LOOT"
	VIAddVersionKey /LANG=${LANG_FRENCH} "CompanyName" "LOOT Team"
	VIAddVersionKey /LANG=${LANG_FRENCH} "LegalCopyright" "© 2009-2014 LOOT Team"
	VIAddVersionKey /LANG=${LANG_FRENCH} "FileDescription" "Programme d'installation pour LOOT 0.5.0"
	VIAddVersionKey /LANG=${LANG_FRENCH} "FileVersion" "0.5.0"

	LangString TEXT_MESSAGEBOX ${LANG_FRENCH} "LOOT est déjà installé, et doit être désinstallé avant de continuer. $\n$\nCliquer sur `OK` pour désinstaller la version précédente ou 'Annuler' pour annuler la mise à jour."
	LangString TEXT_RUN ${LANG_FRENCH} "Lancer LOOT"
	LangString TEXT_SHOWREADME ${LANG_FRENCH} "Afficher la documentation"
	LangString TEXT_MAIN ${LANG_FRENCH} "Tous les fichiers de LOOT, sauf les userlists et les règlages."
	LangString TEXT_USERFILES ${LANG_FRENCH} "L'userlist de LOOT et les règlages."

;--------------------------------
;Spanish (castellano) Strings

	VIAddVersionKey /LANG=${LANG_SPANISH} "ProductName" "LOOT"
	VIAddVersionKey /LANG=${LANG_SPANISH} "CompanyName" "LOOT Team"
	VIAddVersionKey /LANG=${LANG_SPANISH} "LegalCopyright" "© 2009-2014 LOOT Team"
	VIAddVersionKey /LANG=${LANG_SPANISH} "FileDescription" "El instalador para LOOT 0.5.0"
	VIAddVersionKey /LANG=${LANG_SPANISH} "FileVersion" "0.5.0"

	LangString TEXT_MESSAGEBOX ${LANG_SPANISH} "LOOT está instalado, y debe ser desinstalado antes de continuar. $\n$\nPresione `OK` para eliminar la versión anterior o `Cancel` para cancelar la actualización."
	LangString TEXT_RUN ${LANG_SPANISH} "Ejecutar LOOT"
	LangString TEXT_SHOWREADME ${LANG_SPANISH} "Ver Léame"
	LangString TEXT_MAIN ${LANG_SPANISH} "Todos los archivos de LOOT, menos la lista de usuario y configuraciónes."
	LangString TEXT_USERFILES ${LANG_SPANISH} "La lista de usuario y configuraciónes."

;--------------------------------
;Simplified Chinese (简体中文) Strings

    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductName" "LOOT"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "CompanyName" "LOOT Team"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "LegalCopyright" "© 2009-2014 LOOT Team"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileDescription" "LOOT 0.5.0安装包"
    VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileVersion" "0.5.0"

    LangString TEXT_MESSAGEBOX ${LANG_SIMPCHINESE} "检测到旧版LOOT，您需要先卸载旧版才能安装新版。$\n$\n点击“确定”卸载旧版本或者“取消”取消更新。"
    LangString TEXT_RUN ${LANG_SIMPCHINESE} "运行LOOT"
    LangString TEXT_SHOWREADME ${LANG_SIMPCHINESE} "查看说明"
    LangString TEXT_MAIN ${LANG_SIMPCHINESE} "所有LOOT文件（除userlist和配置文件）"
    LangString TEXT_USERFILES ${LANG_SIMPCHINESE} "LOOT的userlist和配置文件。"

;--------------------------------
;Polish (POLSKI) Strings

	VIAddVersionKey /LANG=${LANG_POLISH} "ProductName" "LOOT"
	VIAddVersionKey /LANG=${LANG_POLISH} "CompanyName" "LOOT Team"
	VIAddVersionKey /LANG=${LANG_POLISH} "LegalCopyright" "© 2009-2014 LOOT Team"
	VIAddVersionKey /LANG=${LANG_POLISH} "FileDescription" "Instalator dla LOOT 0.5.0"
	VIAddVersionKey /LANG=${LANG_POLISH} "FileVersion" "0.5.0"

	LangString TEXT_MESSAGEBOX ${LANG_POLISH} "LOOT jest już zainstalowany i musi zostać odinstalowany przed instalowaniem tej wersji. $\n$\nClick `OK` aby odinstalować lub `Cancel` aby anulować aktualizację."
	LangString TEXT_RUN ${LANG_POLISH} " Uruchom LOOT"
	LangString TEXT_SHOWREADME ${LANG_POLISH} "Czytaj Readme"
	LangString TEXT_MAIN ${LANG_POLISH} "Wszystkie pliki LOOT bez ustawień i plików użytkownika."
	LangString TEXT_USERFILES ${LANG_POLISH} "Wszystkie pliki LOOT oraz ustawienia użytkownika ."

;--------------------------------
;Initialisations

    Var InstallPath ;Path to existing LOOT install.

	Function .onInit

		!insertmacro MUI_LANGDLL_DISPLAY

	FunctionEnd

	Function onGUIInit
		; First check to see if LOOT is already installed via installer, and launch the existing uninstaller if so.
		ReadRegStr $InstallPath HKLM "Software\LOOT" "Installed Path"
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
		File "..\build\LOOT.exe"

		;Now install API DLLs.
		;SetOutPath "$INSTDIR\API"
		;File "..\build\loot32.dll"
		;File "..\build\loot64.dll"

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
		File "..\docs\LOOT Metadata Syntax.html"
		File "..\docs\LOOT Readme.html"

		;Now install language files.
		SetOutPath "$INSTDIR\resources\l10n\ru\LC_MESSAGES"
		File "..\resources\l10n\ru\LC_MESSAGES\loot.mo"
		File "..\resources\l10n\ru\LC_MESSAGES\wxstd.mo"
		SetOutPath "$INSTDIR\resources\l10n\es\LC_MESSAGES"
		File "..\resources\l10n\es\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\es\LC_MESSAGES\loot.mo"
		SetOutPath "$INSTDIR\resources\l10n\zh_CN\LC_MESSAGES"
		File "..\resources\l10n\zh_CN\LC_MESSAGES\loot.mo"
		File "..\resources\l10n\zh_CN\LC_MESSAGES\wxstd.mo"
		SetOutPath "$INSTDIR\resources\l10n\fr\LC_MESSAGES"
		File "..\resources\l10n\fr\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\fr\LC_MESSAGES\loot.mo"
		SetOutPath "$INSTDIR\resources\l10n\pl\LC_MESSAGES"
		File "..\resources\l10n\pl\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\pl\LC_MESSAGES\loot.mo"
		SetOutPath "$INSTDIR\resources\l10n\pt_BR\LC_MESSAGES"
		File "..\resources\l10n\pt_BR\LC_MESSAGES\wxstd.mo"
		File "..\resources\l10n\pt_BR\LC_MESSAGES\loot.mo"

        ;Install settings file.
        SetOutPath "$LOCALAPPDATA\LOOT"
        File "..\resources\settings.yaml"

		;Write language setting to settings.yaml.
		StrCmp $LANGUAGE ${LANG_RUSSIAN} 0 +5
            Push "$LOCALAPPDATA\LOOT\settings.yaml"
            Push "Language:"
            Push "Language: ru"
            Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_SPANISH} 0 +5
            Push "$LOCALAPPDATA\LOOT\settings.yaml"
            Push "Language:"
            Push "Language: es"
            Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_SIMPCHINESE} 0 +5
            Push "$LOCALAPPDATA\LOOT\settings.yaml"
            Push "Language:"
            Push "Language: zh_CN"
            Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_FRENCH} 0 +5
            Push "$LOCALAPPDATA\LOOT\settings.yaml"
            Push "Language:"
            Push "Language: fr"
            Call ReplaceLineStr
		StrCmp $LANGUAGE ${LANG_POLISH} 0 +5
            Push "$LOCALAPPDATA\LOOT\settings.yaml"
            Push "Language:"
            Push "Language: pl"
            Call ReplaceLineStr
		;StrCmp $LANGUAGE ${LANG_PORTUGUESEBR} 0 +5
        ;    Push "$LOCALAPPDATA\LOOT\settings.yaml"
        ;    Push "Language:"
        ;    Push "Language: pt_BR"
        ;    Call ReplaceLineStr

		;Add Start Menu shortcuts. Set out path back to $INSTDIR otherwise the shortcuts start in the wrong place.
		;Set Shell Var Context to all so that shortcuts are installed for all users, not just admin.
		SetOutPath "$INSTDIR"
		SetShellVarContext all
		CreateDirectory "$SMPROGRAMS\LOOT"
		CreateShortCut "$SMPROGRAMS\LOOT\LOOT.lnk" "$INSTDIR\LOOT.exe"
		CreateShortCut "$SMPROGRAMS\LOOT\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
		CreateShortCut "$SMPROGRAMS\LOOT\Readme.lnk" "$INSTDIR\Docs\LOOT Readme.html"


		;Store installation folder in registry key.
		WriteRegStr HKLM "Software\LOOT" "Installed Path" "$INSTDIR"
		;Write registry keys for Windows' uninstaller.
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "DisplayName" "LOOT"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "UninstallString" '"$INSTDIR\Uninstall.exe"'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "URLInfoAbout" 'http://loot.github.io/'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "HelpLink" 'http://loot.github.io/'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "Publisher" 'LOOT Development Team'
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "DisplayVersion" '0.5.0'
		WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "NoModify" 1
		WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT" "NoRepair" 1

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

        ${If} $R0 == "1"
            DetailPrint "Visual C++ 2013 Redistributable is already installed; skipping!"
        ${Else}
            DetailPrint "Visual C++ 2013 Redistributable registry key was not found; assumed to be uninstalled."
            DetailPrint "Downloading Visual C++ 2013 Redistributable Setup..."
            SetOutPath $TEMP
            NSISdl::download "http://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x86.exe" "vcredist_x86.exe"

            Pop $R0 ;Get the return value
            ${If} $R0 == "success"
                DetailPrint "Running Visual C++ 2013 Redistributable Setup..."
                Sleep 2000
                HideWindow
                ExecWait '"$TEMP\vcredist_x86.exe" /qb'
                BringToFront
                DetailPrint "Finished Visual C++ 2013 SP1 Redistributable Setup"

                Delete "$TEMP\vcredist_x86.exe"
            ${Else}
                DetailPrint "Could not contact Microsoft.com, or the file has been (re)moved!"
            ${EndIf}
        ${EndIf}
    SectionEnd

;--------------------------------
;Uninstaller Section


	Section "un.LOOT" Main

		;Remove main executables.
		Delete "$INSTDIR\LOOT.exe"

		;Remove API DLLs.
		;Delete "$INSTDIR\API\loot32.dll"
		;Delete "$INSTDIR\API\loot64.dll"
		;RMDir  "$INSTDIR\API"

		;Remove readme images.
		RMDir /r "$INSTDIR\docs\images"

        ;Remove licenses.
        RMDir /r "$INSTDIR\docs\licenses"

		;Remove readme files.
		Delete "$INSTDIR\docs\LOOT Metadata Syntax.html"
		Delete "$INSTDIR\docs\LOOT Readme.html"
		RMDir  "$INSTDIR\docs"

        ;Remove resource files.
        Delete "$INSTDIR\resources\polyfill.js"
        Delete "$INSTDIR\resources\script.js"
        Delete "$INSTDIR\resources\style.css"

		;Remove language files.
		Delete "$INSTDIR\resources\l10n\ru\LC_MESSAGES\loot.mo"
		Delete "$INSTDIR\resources\l10n\ru\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\es\LC_MESSAGES\loot.mo"
		Delete "$INSTDIR\resources\l10n\es\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\zh_CN\LC_MESSAGES\loot.mo"
		Delete "$INSTDIR\resources\l10n\zh_CN\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\fr\LC_MESSAGES\loot.mo"
		Delete "$INSTDIR\resources\l10n\fr\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\pl\LC_MESSAGES\loot.mo"
		Delete "$INSTDIR\resources\l10n\pl\LC_MESSAGES\wxstd.mo"
		Delete "$INSTDIR\resources\l10n\pt_BR\LC_MESSAGES\loot.mo"
		Delete "$INSTDIR\resources\l10n\pt_BR\LC_MESSAGES\wxstd.mo"
		RMDir  "$INSTDIR\resources\l10n\ru\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\ru"
		RMDir  "$INSTDIR\resources\l10n\es\LC_MESSAGES"
		RMDir  "$INSTDIR\resources\l10n\es"
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

		;Now we have to remove the files LOOT generates when it runs.
		Delete "$LOCALAPPDATA\LOOT\LOOTDebugLog.txt"
		;Trying to delete a file that doesn't exist doesn't cause an error, so delete all games' files.
        ;This doesn't handle user-defined games.
		Delete "$LOCALAPPDATA\LOOT\Oblivion\report.html"
		Delete "$LOCALAPPDATA\LOOT\Oblivion\masterlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\Nehrim\report.html"
		Delete "$LOCALAPPDATA\LOOT\Nehrim\masterlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\Skyrim\report.html"
		Delete "$LOCALAPPDATA\LOOT\Skyrim\masterlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\Fallout3\report.html"
		Delete "$LOCALAPPDATA\LOOT\Fallout3\masterlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\FalloutNV\report.html"
		Delete "$LOCALAPPDATA\LOOT\FalloutNV\masterlist.yaml"
		RMDir  "$LOCALAPPDATA\LOOT\Oblivion"
		RMDir  "$LOCALAPPDATA\LOOT\Nehrim"
		RMDir  "$LOCALAPPDATA\LOOT\Skyrim"
		RMDir  "$LOCALAPPDATA\LOOT\Fallout3"
		RMDir  "$LOCALAPPDATA\LOOT\FalloutNV"
        RMDir  "$LOCALAPPDATA\LOOT"

		;Remove uninstaller.
		Delete "$INSTDIR\Uninstall.exe"

		;Remove install directory.
		RMDir "$INSTDIR"

		;Delete registry key.
		DeleteRegKey HKLM "Software\LOOT"

		;Delete stupid Windows created registry keys:
		DeleteRegKey HKCU "Software\LOOT"
		DeleteRegKey HKLM "Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\LOOT"
		DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LOOT"
		DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\App Management\ARPCache\LOOT"
		DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR"
		DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\LOOT.exe"
		DeleteRegValue HKCR "Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\Uninstall.exe"
		DeleteRegValue HKCU "Software\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR"
		DeleteRegValue HKCU "Software\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\LOOT.exe"
		DeleteRegValue HKCU "Software\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache" "$INSTDIR\Uninstall.exe"
		DeleteRegValue HKCU "Software\Microsoft\Windows\ShellNoRoam\MuiCache" "$INSTDIR"
		DeleteRegValue HKCU "Software\Microsoft\Windows\ShellNoRoam\MuiCache" "$INSTDIR\LOOT.exe"
		DeleteRegValue HKCU "Software\Microsoft\Windows\ShellNoRoam\MuiCache" "$INSTDIR\Uninstall.exe"

		;Delete Start Menu folder.
		SetShellVarContext all
		RMDir /r "$SMPROGRAMS\LOOT"

	SectionEnd

	Section /o "un.User Files" UserFiles
		;The following user files are only removed if set to.
		Delete "$LOCALAPPDATA\LOOT\settings.yaml"
		Delete "$LOCALAPPDATA\LOOT\Oblivion\userlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\Nehrim\userlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\Skyrim\userlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\Fallout3\userlist.yaml"
		Delete "$LOCALAPPDATA\LOOT\FalloutNV\userlist.yaml"
		;Also try removing the folders storing them, in case they are otherwise empty.
		RMDir  "$LOCALAPPDATA\LOOT\Oblivion"
		RMDir  "$LOCALAPPDATA\LOOT\Nehrim"
		RMDir  "$LOCALAPPDATA\LOOT\Skyrim"
		RMDir  "$LOCALAPPDATA\LOOT\Fallout3"
		RMDir  "$LOCALAPPDATA\LOOT\FalloutNV"
		;Try removing install directory.
        RMDir  "$LOCALAPPDATA\LOOT"
	SectionEnd

;--------------------------------
;Languages - Description Strings

	!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
	  !insertmacro MUI_DESCRIPTION_TEXT ${Main} "$(Text_Main)"
	  !insertmacro MUI_DESCRIPTION_TEXT ${UserFiles} "$(Text_UserFiles)"
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_END

;--------------------------------
