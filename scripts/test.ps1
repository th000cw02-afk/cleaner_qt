# Build and run all CTest targets for CleanerQt
param(
    [string]$BuildDir = "build",
    [string]$Config = "Release",
    [string]$QtBin = ""
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot

if (-not $QtBin) {
    $candidates = @(
        "C:\Qt\6.8.0\msvc2022_64\bin",
        "C:\Qt\6.10.2\msvc2022_64\bin"
    )
    foreach ($c in $candidates) {
        if (Test-Path $c) { $QtBin = $c; break }
    }
}
if ($QtBin) {
    $env:PATH = "$QtBin;$env:PATH"
}

$BuildPath = Join-Path $Root $BuildDir
if (-not (Test-Path $BuildPath)) {
    Write-Host "Configuring CMake..."
    cmake -S $Root -B $BuildPath -DCMAKE_BUILD_TYPE=$Config -DCLEANER_QT_BUILD_TESTS=ON
}

Write-Host "Building project and tests..."
cmake --build $BuildPath --config $Config

Write-Host "Running ctest..."
Push-Location $BuildPath
ctest -C $Config --output-on-failure
$code = $LASTEXITCODE
Pop-Location
exit $code
