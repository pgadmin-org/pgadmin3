;pgadmin3.nsi
;
SetCompress auto
;--------------------------------

;!ifdef HAVE_UPX
;!packhdr tmp.dat "upx\upx -9 tmp.dat"
;!endif

;--------------------------------


LoadLanguageFile Languages\English.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_ENGLISH} "Program (required)"
LangString Short ${LANG_ENGLISH} "Start Menu Shortcuts"
LangString Doc ${LANG_ENGLISH} "Documentation"
LangString Lang ${LANG_ENGLISH} "User languages"


LoadLanguageFile Languages\Arabic.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_ARABIC} "Program (required)"
LangString Short ${LANG_ARABIC} "Start Menu Shortcuts"
LangString Doc ${LANG_ARABIC} "Documentation"
LangString Lang ${LANG_ARABIC} "User languages"

LoadLanguageFile Languages\Danish.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_DANISH} "Program (required)"
LangString Short ${LANG_DANISH} "Start Menu Shortcuts"
LangString Doc ${LANG_DANISH} "Documentation"
LangString Lang ${LANG_DANISH} "User languages"

LoadLanguageFile Languages\German.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "Nutzung und Verteilung von pgAdmin III unterliegen den folgenden Nutzungsbestimmungen:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_GERMAN} "Programm (erforderlich)"
LangString Short ${LANG_GERMAN} "Start Menu Shortcuts"
LangString Doc ${LANG_GERMAN} "Dokumentation"
LangString Lang ${LANG_GERMAN} "Benutzersprachen"

; Farsi fa_IR not available at installation time

LoadLanguageFile Languages\French.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_FRENCH} "Program (required)"
LangString Short ${LANG_FRENCH} "Start Menu Shortcuts"
LangString Doc ${LANG_FRENCH} "Documentation"
LangString Lang ${LANG_FRENCH} "User languages"


LoadLanguageFile Languages\Croatian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_CROATIAN} "Program (required)"
LangString Short ${LANG_CROATIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_CROATIAN} "Documentation"
LangString Lang ${LANG_CROATIAN} "User languages"

LoadLanguageFile Languages\Hungarian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_HUNGARIAN} "Program (required)"
LangString Short ${LANG_HUNGARIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_HUNGARIAN} "Documentation"
LangString Lang ${LANG_HUNGARIAN} "User languages"

; Indonesian id_ID not available at installation time

LoadLanguageFile Languages\Italian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_ITALIAN} "Program (required)"
LangString Short ${LANG_ITALIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_ITALIAN} "Documentation"
LangString Lang ${LANG_ITALIAN} "User languages"

LoadLanguageFile Languages\Japanese.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_JAPANESE} "Program (required)"
LangString Short ${LANG_JAPANESE} "Start Menu Shortcuts"
LangString Doc ${LANG_JAPANESE} "Documentation"
LangString Lang ${LANG_JAPANESE} "User languages"

; Norwegian nb_NO not available at installation time

LoadLanguageFile Languages\Polish.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_POLISH} "Program (required)"
LangString Short ${LANG_POLISH} "Start Menu Shortcuts"
LangString Doc ${LANG_POLISH} "Documentation"
LangString Lang ${LANG_POLISH} "User languages"

LoadLanguageFile Languages\Romanian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_ROMANIAN} "Program (required)"
LangString Short ${LANG_ROMANIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_ROMANIAN} "Documentation"
LangString Lang ${LANG_ROMANIAN} "User languages"

LoadLanguageFile Languages\Russian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_RUSSIAN} "Program (required)"
LangString Short ${LANG_RUSSIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_RUSSIAN} "Documentation"
LangString Lang ${LANG_RUSSIAN} "User languages"

LoadLanguageFile Languages\SimpChinese.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_SIMPCHINESE} "Program (required)"
LangString Short ${LANG_SIMPCHINESE} "Start Menu Shortcuts"
LangString Doc ${LANG_SIMPCHINESE} "Documentation"
LangString Lang ${LANG_SIMPCHINESE} "User languages"

LoadLanguageFile Languages\TradChinese.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_TRADCHINESE} "Program (required)"
LangString Short ${LANG_TRADCHINESE} "Start Menu Shortcuts"
LangString Doc ${LANG_TRADCHINESE} "Documentation"
LangString Lang ${LANG_TRADCHINESE} "User languages"

LoadLanguageFile Languages\Turkish.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_TURKISH} "Program (required)"
LangString Short ${LANG_TURKISH} "Start Menu Shortcuts"
LangString Doc ${LANG_TURKISH} "Documentation"
LangString Lang ${LANG_TURKISH} "User languages"

LoadLanguageFile Languages\Bulgarian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_BULGARIAN} "Program (required)"
LangString Short ${LANG_BULGARIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_BULGARIAN} "Documentation"
LangString Lang ${LANG_BULGARIAN} "User languages"

LoadLanguageFile Languages\Dutch.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_DUTCH} "Program (required)"
LangString Short ${LANG_DUTCH} "Start Menu Shortcuts"
LangString Doc ${LANG_DUTCH} "Documentation"
LangString Lang ${LANG_DUTCH} "User languages"

LoadLanguageFile Languages\Greek.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_GREEK} "Program (required)"
LangString Short ${LANG_GREEK} "Start Menu Shortcuts"
LangString Doc ${LANG_GREEK} "Documentation"
LangString Lang ${LANG_GREEK} "User languages"

LoadLanguageFile Languages\NORWEGIAN.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_NORWEGIAN} "Program (required)"
LangString Short ${LANG_NORWEGIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_NORWEGIAN} "Documentation"
LangString Lang ${LANG_NORWEGIAN} "User languages"

LoadLanguageFile Languages\Portuguese.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_PORTUGUESE} "Program (required)"
LangString Short ${LANG_PORTUGUESE} "Start Menu Shortcuts"
LangString Doc ${LANG_PORTUGUESE} "Documentation"
LangString Lang ${LANG_PORTUGUESE} "User languages"

LoadLanguageFile Languages\Serbian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_SERBIAN} "Program (required)"
LangString Short ${LANG_SERBIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_SERBIAN} "Documentation"
LangString Lang ${LANG_SERBIAN} "User languages"

LoadLanguageFile Languages\Slovak.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_SLOVAK} "Program (required)"
LangString Short ${LANG_SLOVAK} "Start Menu Shortcuts"
LangString Doc ${LANG_SLOVAK} "Documentation"
LangString Lang ${LANG_SLOVAK} "User languages"

LoadLanguageFile Languages\Slovenian.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_SLOVENIAN} "Program (required)"
LangString Short ${LANG_SLOVENIAN} "Start Menu Shortcuts"
LangString Doc ${LANG_SLOVENIAN} "Documentation"
LangString Lang ${LANG_SLOVENIAN} "User languages"

LoadLanguageFile Languages\Swedish.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_SWEDISH} "Program (required)"
LangString Short ${LANG_SWEDISH} "Start Menu Shortcuts"
LangString Doc ${LANG_SWEDISH} "Documentation"
LangString Lang ${LANG_SWEDISH} "User languages"

LoadLanguageFile Languages\Spanish.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_SPANISH} "Program (required)"
LangString Short ${LANG_SPANISH} "Start Menu Shortcuts"
LangString Doc ${LANG_SPANISH} "Documentation"
LangString Lang ${LANG_SPANISH} "User languages"

LoadLanguageFile Languages\Czech.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_CZECH} "Program (required)"
LangString Short ${LANG_CZECH} "Start Menu Shortcuts"
LangString Doc ${LANG_CZECH} "Documentation"
LangString Lang ${LANG_CZECH} "User languages"

LoadLanguageFile Languages\Farsi.nlf
Name "pgAdmin3"
Caption "pgAdmin III Installation"
LicenseText "The use and distribution of pgAdmin III is subject to the following Artistic Licence:"
LicenseData "licence.rtf"
ComponentText " "
DirText " "
LangString Prg ${LANG_FARSI} "Program (required)"
LangString Short ${LANG_FARSI} "Start Menu Shortcuts"
LangString Doc ${LANG_FARSI} "Documentation"
LangString Lang ${LANG_FARSI} "User languages"


Icon images\normal-install.ico
OutFile "pgadmin3-install.exe"

SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
AddBrandingImage left 100

InstallDir "$PROGRAMFILES\pgAdmin3"
InstallDirRegKey HKLM "SOFTWARE\pgAdmin III" "Install_Dir"
;CheckBitmap Images\checksX.bmp


Page license showImage
Page components showImage
Page directory showImage
Page instfiles showImage

Function showImage
  GetTempFileName $0
  File /oname=$0 Images\elephant.bmp
  SetBrandingImage $0
  Delete $0
FunctionEnd






Section !$(Prg) Prg
  SectionIn RO
  ; Set output path to the installation directory.
  CreateDirectory $INSTDIR\ui
  CreateDirectory $INSTDIR\docs
  CreateDirectory $INSTDIR\docs\en_US
  
  SetOutPath $INSTDIR
  File ..\..\src\Unicode_SSL_Release\pgAdmin3.exe

  SetOutPath $INSTDIR\ui
  File ..\..\src\ui\wxstd.mo
  File ..\..\src\ui\pgadmin3.lng

  SetOutPath $INSTDIR\docs\en_US
  File ..\..\docs\en_US\bugreport.html
  File ..\..\docs\en_US\tips.txt
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\pgAdmin III" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pgAdmin3" "DisplayName" "pgAdmin III (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pgAdmin3" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
SectionEnd


Section !$(Short) Short
  CreateDirectory "$SMPROGRAMS\pgAdmin III"
  CreateShortCut "$SMPROGRAMS\pgAdmin III\pgAdmin III.lnk" "$INSTDIR\pgAdmin3.exe" "" "$INSTDIR\pgAdmin3.exe" 0
  CreateShortCut "$SMPROGRAMS\pgAdmin III\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd


Section !$(Doc) Doc
  CreateDirectory $INSTDIR\docs\en_US
  SetOutPath $INSTDIR\docs\en_US

  File ..\..\docs\en_US\pgadmin3.chm
SectionEnd


SubSection !$(Lang) Lang
  Section "Afrikaans" af_ZA
    CreateDirectory $INSTDIR\ui\af_ZA
    SetOutPath $INSTDIR\ui\af_ZA
    FILE /r ..\..\src\ui\af_ZA\*.mo
  SectionEnd
  Section "Bulgarian" bg_BG
    CreateDirectory $INSTDIR\ui\bg_BG
    SetOutPath $INSTDIR\ui\bg_BG
    FILE /r ..\..\src\ui\bg_BG\*.mo
  SectionEnd
  Section "Czech" cs_CZ
    CreateDirectory $INSTDIR\ui\cs_CZ
    SetOutPath $INSTDIR\ui\cs_CZ
    FILE /r ..\..\src\ui\cs_CZ\*.mo
  SectionEnd
  Section "Dansk" da_DK
    CreateDirectory $INSTDIR\ui\da_DK
    SetOutPath $INSTDIR\ui\da_DK
    FILE /r ..\..\src\ui\da_DK\*.mo
  SectionEnd
  Section "Deutsch" de_DE
    CreateDirectory $INSTDIR\ui\de_DE
    SetOutPath $INSTDIR\ui\de_DE
    FILE /r ..\..\src\ui\de_DE\*.mo
  SectionEnd
  Section "Greek" el_GR
    CreateDirectory $INSTDIR\ui\el_GR
    SetOutPath $INSTDIR\ui\el_GR
    FILE /r ..\..\src\ui\el_GR\*.mo
  SectionEnd
  Section "Spanish" es_ES
    CreateDirectory $INSTDIR\ui\es_ES
    SetOutPath $INSTDIR\ui\es_ES
    FILE /r ..\..\src\ui\es_ES\*.mo
  SectionEnd
  Section "Farsi" fa_IR
    CreateDirectory $INSTDIR\ui\fa_IR
    SetOutPath $INSTDIR\ui\fa_IR
    FILE /r ..\..\src\ui\fa_IR\*.mo
  SectionEnd
  Section "Français" fr_FR
    CreateDirectory $INSTDIR\ui\fr_FR
    SetOutPath $INSTDIR\ui\fr_FR
    FILE /r ..\..\src\ui\fr_FR\*.mo
  SectionEnd
  Section "Galego" gl_ES
    CreateDirectory $INSTDIR\ui\gl_ES
    SetOutPath $INSTDIR\ui\gl_ES
    FILE /r ..\..\src\ui\gl_ES\*.mo
  SectionEnd
  Section "Hrvatski" hr_HR
    CreateDirectory $INSTDIR\ui\hr_HR
    SetOutPath $INSTDIR\ui\hr_HR
    FILE /r ..\..\src\ui\hr_HR\*.mo
  SectionEnd
  Section "Magyar" hu_HU
    CreateDirectory $INSTDIR\ui\hu_HU
    SetOutPath $INSTDIR\ui\hu_HU
    FILE /r ..\..\src\ui\hu_HU\*.mo
  SectionEnd
  Section "Indonesia" id_ID
    CreateDirectory $INSTDIR\ui\id_ID
    SetOutPath $INSTDIR\ui\id_ID
    FILE /r ..\..\src\ui\id_ID\*.mo
  SectionEnd
  Section "Italiano" it_IT
    CreateDirectory $INSTDIR\ui\it_IT
    SetOutPath $INSTDIR\ui\it_IT
    FILE /r ..\..\src\ui\it_IT\*.mo
  SectionEnd
  Section "Japanese" ja_JP
    CreateDirectory $INSTDIR\ui\ja_JP
    SetOutPath $INSTDIR\ui\ja_JP
    FILE /r ..\..\src\ui\ja_JP\*.mo
  SectionEnd
  Section "Latviešu" lv_LV
    CreateDirectory $INSTDIR\ui\lv_LV
    SetOutPath $INSTDIR\ui\lv_LV
    FILE /r ..\..\src\ui\lv_LV\*.mo
  SectionEnd
  Section "Norwegian Bokmål" nb_NO
    CreateDirectory $INSTDIR\ui\nb_NO
    SetOutPath $INSTDIR\ui\nb_NO
    FILE /r ..\..\src\ui\nb_NO\*.mo
  SectionEnd
  Section "Nederlands" nl_NL
    CreateDirectory $INSTDIR\ui\nl_NL
    SetOutPath $INSTDIR\ui\nl_NL
    FILE /r ..\..\src\ui\nl_NL\*.mo
  SectionEnd
  Section "Polski" pl_PL
    CreateDirectory $INSTDIR\ui\pl_PL
    SetOutPath $INSTDIR\ui\pl_PL
    FILE /r ..\..\src\ui\pl_PL\*.mo
  SectionEnd
  Section "Português" pt_PT
    CreateDirectory $INSTDIR\ui\pt_PT
    SetOutPath $INSTDIR\ui\pt_PT
    FILE /r ..\..\src\ui\pt_PT\*.mo
  SectionEnd
  Section "Português (Brasileiro)" pt_BR
    CreateDirectory $INSTDIR\ui\pt_BR
    SetOutPath $INSTDIR\ui\pt_BR
    FILE /r ..\..\src\ui\pt_BR\*.mo
  SectionEnd
  Section "Romana" ro_RO
    CreateDirectory $INSTDIR\ui\ro_RO
    SetOutPath $INSTDIR\ui\ro_RO
    FILE /r ..\..\src\ui\ro_RO\*.mo
  SectionEnd
  Section "Russian" ru_RU
    CreateDirectory $INSTDIR\ui\ru_RU
    SetOutPath $INSTDIR\ui\ru_RU
    FILE /r ..\..\src\ui\ru_RU\*.mo
  SectionEnd
  Section "Slovensky" sk_SK
    CreateDirectory $INSTDIR\ui\Sk_SK
    SetOutPath $INSTDIR\ui\sk_SK
    FILE /r ..\..\src\ui\sk_SK\*.mo
  SectionEnd
  Section "Slovenšcina" sl_SI
    CreateDirectory $INSTDIR\ui\sl_SI
    SetOutPath $INSTDIR\ui\sl_SI
    FILE /r ..\..\src\ui\sl_SI\*.mo
  SectionEnd
  Section "Serbian" sr_YU
    CreateDirectory $INSTDIR\ui\sr_YU
    SetOutPath $INSTDIR\ui\sr_YU
    FILE /r ..\..\src\ui\sr_YU\*.mo
  SectionEnd
  Section "Svenska" sv_SE
    CreateDirectory $INSTDIR\ui\sv_SE
    SetOutPath $INSTDIR\ui\sv_SE
    FILE /r ..\..\src\ui\sv_SE\*.mo
  SectionEnd
  Section "Türkçe" tr_TR
    CreateDirectory $INSTDIR\ui\tr_TR
    SetOutPath $INSTDIR\ui\tr_TR
    FILE /r ..\..\src\ui\tr_TR\*.mo
  SectionEnd
  Section "Chinese Simplified" zh_CN
    CreateDirectory $INSTDIR\ui\zh_CN
    SetOutPath $INSTDIR\ui\zh_CN
    FILE /r ..\..\src\ui\zh_CN\*.mo
  SectionEnd
  Section "Chinese Traditional" zh_TW
   CreateDirectory $INSTDIR\ui\zh_TW
    SetOutPath $INSTDIR\ui\zh_TW
    FILE /r ..\..\src\ui\zh_TW\*.mo
  SectionEnd
SubSectionEnd

;--------------------------------

; Uninstaller

UninstallText "This will uninstall pgAdmin III. Hit next to continue."
UninstallIcon images\normal-install.ico

Section "Uninstall"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pgAdmin3"
;  DeleteRegKey HKLM "SOFTWARE\pgAdmin III"
  Delete "$INSTDIR\pgAdmin3.exe"
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$SMPROGRAMS\pgAdmin III\*.*"
  RMDir "$SMPROGRAMS\pgAdmin III"
  
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\ui"
  RMDir "$INSTDIR"

  IfFileExists "$INSTDIR" 0 NoErrorMsg
    MessageBox MB_OK "Note: $INSTDIR could not be removed!" IDOK 0 ; skipped if file doesn't exist
  NoErrorMsg:

SectionEnd


Function .onInit
  Push ""
  Push ${LANG_ENGLISH}
  Push "English"
;  Push ${LANG_ARABIC}
;  Push "Arabic"
  Push ${LANG_BULGARIAN}
  Push "Bulgarian"
  Push ${LANG_CROATIAN}
  Push "Hrvatski"
  Push ${LANG_DANISH}
  Push "Dansk"
  Push ${LANG_DUTCH}
  Push "Nederlands"
  Push ${LANG_GERMAN}
  Push "Deutsch"
  Push ${LANG_GREEK}
  Push "Greek"
  Push ${LANG_FRENCH}
  Push "Français"
  Push ${LANG_HUNGARIAN}
  Push "Magyar"
  Push ${LANG_ITALIAN}
  Push "Italiano"
  Push ${LANG_JAPANESE}
  Push "Japanese"
  Push ${LANG_NORWEGIAN}
  Push "Norwegian"
  Push ${LANG_POLISH}
  Push "Polski"
  Push ${LANG_PORTUGUESE}
  Push "Português"
  Push ${LANG_ROMANIAN}
  Push "Romana"
  Push ${LANG_SERBIAN}
  Push "Serbian"
  Push ${LANG_SLOVAK}
  Push "Slovenski"
  Push ${LANG_SLOVENIAN}
  Push "Slovenšcina"
  Push ${LANG_SWEDISH}
  Push "Svenska"
  Push ${LANG_SPANISH}
  Push "Spanish"
  Push ${LANG_RUSSIAN}
  Push "Russian"
  Push ${LANG_SIMPCHINESE}
  Push "Chinese Simplified"
  Push ${LANG_TRADCHINESE}
  Push "Chinese Traditional"
  Push ${LANG_TURKISH}
  Push "Türkçe"
  Push A ; A means auto count languages
      ; for the auto count to work the first empty push (Push "") must remain
  LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

  Pop $LANGUAGE
  StrCmp $LANGUAGE "cancel" 0 +2
  	Abort

  SectionSetFlags ${af_ZA} 0
;  StrCmp $LANGUAGE ${LANG_ARABIC} +2 0
;    SectionSetFlags ${ar_SA} 0
  StrCmp $LANGUAGE ${LANG_BULGARIAN} +2 0
  SectionSetFlags ${bg_BG} 0
  StrCmp $LANGUAGE ${LANG_CZECH} +2 0
  SectionSetFlags ${cs_CZ} 0
  StrCmp $LANGUAGE ${LANG_DANISH} +2 0
    SectionSetFlags ${da_DK} 0
  StrCmp $LANGUAGE ${LANG_GERMAN} +2 0
    SectionSetFlags ${de_DE} 0
  StrCmp $LANGUAGE ${LANG_GREEK} +2 0
  SectionSetFlags ${el_GR} 0
  StrCmp $LANGUAGE ${LANG_SPANISH} +2 0
  SectionSetFlags ${es_ES} 0
  StrCmp $LANGUAGE ${LANG_FARSI} +2 0
  SectionSetFlags ${fa_IR} 0
  StrCmp $LANGUAGE ${LANG_FRENCH} +2 0
    SectionSetFlags ${fr_FR} 0
  StrCmp $LANGUAGE ${LANG_GALICIAN} +2 0
    SectionSetFlags ${gl_ES} 0
  StrCmp $LANGUAGE ${LANG_CROATIAN} +2 0
    SectionSetFlags ${hr_HR} 0
  StrCmp $LANGUAGE ${LANG_HUNGARIAN} +2 0
    SectionSetFlags ${hu_HU} 0
  StrCmp $LANGUAGE ${LANG_INDONESIAN} +2 0
  SectionSetFlags ${id_ID} 0
  StrCmp $LANGUAGE ${LANG_ITALIAN} +2 0
    SectionSetFlags ${it_IT} 0
  StrCmp $LANGUAGE ${LANG_JAPANESE} +2 0
  SectionSetFlags ${ja_JP} 0
  StrCmp $LANGUAGE ${LANG_PORTUGUESE} +2 0
  SectionSetFlags ${lv_LV} 0
  StrCmp $LANGUAGE ${LANG_NORWEGIAN} +2 0
  SectionSetFlags ${nb_NO} 0
  StrCmp $LANGUAGE ${LANG_DUTCH} +2 0
  SectionSetFlags ${nl_NL} 0
  StrCmp $LANGUAGE ${LANG_POLISH} +2 0
  SectionSetFlags ${pl_PL} 0
  StrCmp $LANGUAGE ${LANG_PORTUGUESE} +3 0
  SectionSetFlags ${pt_BR} 0
  SectionSetFlags ${pt_PT} 0
  StrCmp $LANGUAGE ${LANG_ROMANIAN} +2 0
    SectionSetFlags ${ro_RO} 0
  StrCmp $LANGUAGE ${LANG_RUSSIAN} +2 0
    SectionSetFlags ${ru_RU} 0
  StrCmp $LANGUAGE ${LANG_SLOVAK} +2 0
  SectionSetFlags ${sk_SK} 0
  StrCmp $LANGUAGE ${LANG_SLOVENIAN} +2 0
  SectionSetFlags ${sl_SI} 0
  StrCmp $LANGUAGE ${LANG_SERBIAN} +2 0
  SectionSetFlags ${sr_YU} 0
  StrCmp $LANGUAGE ${LANG_SWEDISH} +2 0
  SectionSetFlags ${sv_SE} 0
  StrCmp $LANGUAGE ${LANG_TURKISH} +2 0
    SectionSetFlags ${tr_TR} 0
  StrCmp $LANGUAGE ${LANG_SIMPCHINESE} +2 0
    SectionSetFlags ${zh_CN} 0
  StrCmp $LANGUAGE ${LANG_TRADCHINESE} +2 0
    SectionSetFlags ${zh_TW} 0
FunctionEnd

