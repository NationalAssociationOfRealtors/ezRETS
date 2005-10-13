[Setup]
AppName=ezRETS
AppVerName=ezRETS 0.9.6
DefaultDirName={pf}\ezRETS
DefaultGroupName=ezRETS
DisableDirPage=yes
SourceDir=..\..
OutputDir=build
OutputBaseFilename=ezRETS-win32-0.9.6
UninstallFilesDir={app}\uninstall
VersionInfoVersion=0.9.6
VersionInfoCompany=Center for REALTOR Technology
LicenseFile=LICENSE

[Files]
Source: project\driver\src\release\ezrets.dll; DestDir: {sys}; Flags: restartreplace replacesameversion
Source: project\setup\src\release\ezretss.dll; DestDir: {sys}; Flags: restartreplace replacesameversion
Source: README; DestDir: {app}; DestName: README.TXT; Flags: isreadme
Source: LICENSE; DestDir: {app}; DestName: LICENSE.TXT
Source: NEWS; DestDir: {app}; DestName: NEWS.TXT
Source: doc\InstallTutorial\odbc6.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\ezRETSSetup.html; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\install1.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\install2.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\install3.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\install4.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\install_icon.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\odbc1.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\odbc2.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\odbc3.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\odbc4.png; DestDir: {app}\InstallTutorial
Source: doc\InstallTutorial\odbc5.png; DestDir: {app}\InstallTutorial
Source: doc\ExcelTutorial\sort.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\columns.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\data.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\datamenu.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\datasource.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\ExcelTutorial.html; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\filter.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\filtercity.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\filterprice.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\filterstatus.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\finish.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\plus.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\refresh.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\saveto.png; DestDir: {app}\ExcelTutorial
Source: doc\ExcelTutorial\selectedcolumns.png; DestDir: {app}\ExcelTutorial

[Icons]
Name: {commonprograms}\ezRETS\Readme; Filename: {app}\README.txt
Name: {commonprograms}\ezRETS\License; Filename: {app}\LICENSE.txt
Name: {commonprograms}\ezRETS\News; Filename: {app}\NEWS.txt
Name: {commonprograms}\ezRETS\Uninstall ezRETS; Filename: {uninstallexe}
Name: {commonprograms}\ezRETS\Excel Tutorial; Filename: {app}\ExcelTutorial\ExcelTutorial.html
Name: {commonprograms}\ezRETS\Install Tutorial; Filename: {app}\InstallTutorial\InstallTutorial.html

[Registry]
Root: HKLM; SubKey: SOFTWARE\ODBC\ODBCINST.INI\ezRETS ODBC Driver; Flags: uninsdeletekey
Root: HKLM; SubKey: SOFTWARE\ODBC\ODBCINST.INI\ezRETS ODBC Driver; ValueType: dword; ValueName: UsageCount; ValueData: $00000001
Root: HKLM; SubKey: SOFTWARE\ODBC\ODBCINST.INI\ezRETS ODBC Driver; ValueType: string; ValueName: ConnectFunctions; ValueData: YYN
Root: HKLM; SubKey: SOFTWARE\ODBC\ODBCINST.INI\ezRETS ODBC Driver; ValueType: string; ValueName: ; ValueData: 
Root: HKLM; SubKey: SOFTWARE\ODBC\ODBCINST.INI\ezRETS ODBC Driver; ValueType: string; ValueName: CPTimeout; ValueData: <not pooled>
Root: HKLM; SubKey: SOFTWARE\ODBC\ODBCINST.INI\ezRETS ODBC Driver; ValueType: string; ValueName: Driver; ValueData: {sys}\ezrets.dll
Root: HKLM; SubKey: SOFTWARE\ODBC\ODBCINST.INI\ezRETS ODBC Driver; ValueType: string; ValueName: Setup; ValueData: {sys}\ezretss.dll
Root: HKLM; Subkey: SOFTWARE\ODBC\ODBCINST.INI\ODBC Drivers; ValueType: string; ValueName: ezRETS ODBC Driver; ValueData: Installed; Flags: createvalueifdoesntexist uninsdeletevalue

[Dirs]
Name: {app}\ExcelTutorial
Name: {app}\InstallTutorial
