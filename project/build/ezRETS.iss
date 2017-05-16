[Setup]
AppName=ezRETS
AppVerName=ezRETS {#VERSION}
#if TARGET_CPU == "x64"
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
#endif
DefaultDirName={pf}\ezRETS
DefaultGroupName=ezRETS
DisableDirPage=yes
SourceDir=..\..
OutputDir=build
OutputBaseFilename=ezRETS-{#TARGET_CPU}-{#VERSION}
UninstallFilesDir={app}\uninstall
VersionInfoVersion={#VERSION}
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
Source: doc\SQL_Reference.html; DestDir: {app}
Source: project\installer\src\release\RegisterDriver.exe; DestDir: {app}; Flags: replacesameversion

[Icons]
Name: {commonprograms}\ezRETS\Readme; Filename: {app}\README.txt
Name: {commonprograms}\ezRETS\License; Filename: {app}\LICENSE.txt
Name: {commonprograms}\ezRETS\News; Filename: {app}\NEWS.txt
Name: {commonprograms}\ezRETS\Uninstall ezRETS; Filename: {uninstallexe}
Name: {commonprograms}\ezRETS\Excel Tutorial; Filename: {app}\ExcelTutorial\ExcelTutorial.html
Name: {commonprograms}\ezRETS\Install Tutorial; Filename: {app}\InstallTutorial\ezRETSSetup.html
Name: {commonprograms}\ezRETS\SQL Reference; Filename: {app}\SQL_Reference.html

[Dirs]
Name: {app}\ExcelTutorial
Name: {app}\InstallTutorial

[Run]
Filename: {app}\RegisterDriver.exe; Parameters: -i; StatusMsg: Registering ezRETS Driver; Flags: runhidden

[UninstallRun]
Filename: {app}\RegisterDriver.exe; Parameters: -r -d; Flags: runhidden
