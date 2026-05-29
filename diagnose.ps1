# 诊断脚本 - 检查 CleanerQt.exe 无法运行的原因

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CleanerQt 诊断工具" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查可执行文件
$ExePaths = @(
    "public\Debug\CleanerQt.exe",
    "public\Release\CleanerQt.exe",
    "public\CleanerQt.exe",
    "build\bin\Debug\CleanerQt.exe",
    "build\bin\Release\CleanerQt.exe"
)

$ExePath = $null
foreach ($path in $ExePaths) {
    if (Test-Path $path) {
        $ExePath = $path
        Write-Host "找到可执行文件: $path" -ForegroundColor Green
        break
    }
}

if (-not $ExePath) {
    Write-Host "错误: 找不到 CleanerQt.exe" -ForegroundColor Red
    Write-Host "请先编译项目" -ForegroundColor Yellow
    exit 1
}

$ExeDir = Split-Path -Parent (Resolve-Path $ExePath)

Write-Host "`n检查 Qt DLL 依赖..." -ForegroundColor Cyan

# 检查必需的 Qt DLL
$RequiredDlls = @(
    "Qt6Core.dll",
    "Qt6Gui.dll",
    "Qt6Qml.dll",
    "Qt6Quick.dll",
    "Qt6QuickControls2.dll",
    "Qt6Network.dll"
)

$MissingDlls = @()
foreach ($dll in $RequiredDlls) {
    $dllPath = Join-Path $ExeDir $dll
    if (Test-Path $dllPath) {
        Write-Host "  ✓ $dll" -ForegroundColor Green
    } else {
        Write-Host "  ✗ $dll (缺失)" -ForegroundColor Red
        $MissingDlls += $dll
    }
}

# 检查 platforms 插件
Write-Host "`n检查 Qt 插件..." -ForegroundColor Cyan
$PlatformsDir = Join-Path $ExeDir "platforms"
if (Test-Path $PlatformsDir) {
    $QWindowsDll = Join-Path $PlatformsDir "qwindows.dll"
    if (Test-Path $QWindowsDll) {
        Write-Host "  ✓ platforms/qwindows.dll" -ForegroundColor Green
    } else {
        Write-Host "  ✗ platforms/qwindows.dll (缺失)" -ForegroundColor Red
        $MissingDlls += "platforms/qwindows.dll"
    }
} else {
    Write-Host "  ✗ platforms 目录不存在" -ForegroundColor Red
    $MissingDlls += "platforms/"
}

# 检查 QML 模块
Write-Host "`n检查 QML 模块..." -ForegroundColor Cyan
$QmlDir = Join-Path $ExeDir "qml"
if (Test-Path $QmlDir) {
    Write-Host "  ✓ qml 目录存在" -ForegroundColor Green
    
    $RequiredQmlModules = @(
        "QtQuick",
        "QtQuick.Controls",
        "QtQuick.Layouts"
    )
    
    foreach ($module in $RequiredQmlModules) {
        $modulePath = Join-Path $QmlDir $module
        if (Test-Path $modulePath) {
            Write-Host "    ✓ $module" -ForegroundColor Green
        } else {
            Write-Host "    ✗ $module (缺失)" -ForegroundColor Yellow
        }
    }
} else {
    Write-Host "  ✗ qml 目录不存在" -ForegroundColor Yellow
    Write-Host "    注意: QML 文件可能已编译到资源中" -ForegroundColor Yellow
}

# 检查 Visual C++ 运行时
Write-Host "`n检查 Visual C++ 运行时..." -ForegroundColor Cyan
$VCRedist = @(
    "vcruntime140.dll",
    "msvcp140.dll"
)

foreach ($dll in $VCRedist) {
    $dllPath = Join-Path $ExeDir $dll
    if (Test-Path $dllPath) {
        Write-Host "  ✓ $dll" -ForegroundColor Green
    } else {
        Write-Host "  ⚠ $dll (可能由系统提供)" -ForegroundColor Yellow
    }
}

# 总结
Write-Host "`n========================================" -ForegroundColor Cyan
if ($MissingDlls.Count -eq 0) {
    Write-Host "所有必需的依赖都已找到！" -ForegroundColor Green
    Write-Host "`n如果程序仍然无法运行，请尝试：" -ForegroundColor Yellow
    Write-Host "1. 在命令行中运行程序查看错误信息" -ForegroundColor Yellow
    Write-Host "2. 检查事件查看器中的应用程序错误" -ForegroundColor Yellow
    Write-Host "3. 使用 Dependency Walker 检查依赖" -ForegroundColor Yellow
} else {
    Write-Host "发现缺失的依赖！" -ForegroundColor Red
    Write-Host "`n缺失的文件：" -ForegroundColor Red
    foreach ($dll in $MissingDlls) {
        Write-Host "  - $dll" -ForegroundColor Red
    }
    Write-Host "`n请运行部署脚本：" -ForegroundColor Yellow
    Write-Host "  .\deploy.ps1" -ForegroundColor Cyan
    Write-Host "  或" -ForegroundColor Yellow
    Write-Host "  deploy.bat" -ForegroundColor Cyan
}

Write-Host "`n可执行文件位置: $ExePath" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

