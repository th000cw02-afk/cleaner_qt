; Inno Setup script — build Release + deploy-release first
#define MyAppName "Disk Space Analyzer"
#define MyAppVersion "2.0.0"
#define MyAppPublisher "CleanerQt"
#define MyAppExeName "CleanerQt.exe"

[Setup]
AppId={{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=..\installer\output
OutputBaseFilename=CleanerQt-{#MyAppVersion}-setup
Compression=lzma
SolidCompression=yes

[Files]
Source: "..\public\Release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
