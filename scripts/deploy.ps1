# 将 Qt 运行时与 QML 依赖复制到 exe 同目录（解决「找不到 Qt6Widgets.dll」）
param(
    [string]$Config = "Release",
    [string]$QtBin = "C:\Qt\6.8.0\msvc2022_64\bin"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if (-not (Test-Path (Join-Path $root "CMakeLists.txt"))) {
    Write-Error "项目根目录无效: $root"
}

$exe = Join-Path $root "public\$($Config.ToLower())\CleanerQt.exe"
if (-not (Test-Path $exe)) {
    $exe = Join-Path $root "public\$Config\CleanerQt.exe"
}
if (-not (Test-Path $exe)) {
    Write-Error "找不到 CleanerQt.exe，请先构建: cmake --build build --config $Config"
}

$windeployqt = Join-Path $QtBin "windeployqt.exe"
if (-not (Test-Path $windeployqt)) {
    $found = Get-Command windeployqt -ErrorAction SilentlyContinue
    if ($found) { $windeployqt = $found.Source }
    else { Write-Error "找不到 windeployqt，请设置 -QtBin 或把 Qt bin 加入 PATH" }
}

Write-Host "Deploying to: $(Split-Path $exe)"
& $windeployqt --qmldir (Join-Path $root "qml") --release $exe
Write-Host "完成。可直接运行: $exe"
