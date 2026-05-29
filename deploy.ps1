# Qt6 依赖部署脚本
# 此脚本会自动收集并复制所有必需的 Qt6 DLL 和插件到可执行文件目录

param(
    [string]$ExePath = "",
    [string]$QtPath = "",
    [string]$OutputDir = "",
    [string]$BuildType = ""
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Qt6 依赖部署脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# 确定可执行文件路径和构建类型
if ($ExePath -eq "") {
    # 按优先级查找 exe
    $PublicDebugExe = Join-Path $PSScriptRoot "public\Debug\CleanerQt.exe"
    $PublicReleaseExe = Join-Path $PSScriptRoot "public\Release\CleanerQt.exe"
    $PublicExe = Join-Path $PSScriptRoot "public\CleanerQt.exe"
    $BuildDebugExe = Join-Path $PSScriptRoot "build\bin\Debug\CleanerQt.exe"
    $BuildReleaseExe = Join-Path $PSScriptRoot "build\bin\Release\CleanerQt.exe"
    
    if (Test-Path $PublicDebugExe) {
        $ExePath = $PublicDebugExe
        if ($BuildType -eq "") { $BuildType = "Debug" }
    } elseif (Test-Path $PublicReleaseExe) {
        $ExePath = $PublicReleaseExe
        if ($BuildType -eq "") { $BuildType = "Release" }
    } elseif (Test-Path $PublicExe) {
        $ExePath = $PublicExe
    } elseif (Test-Path $BuildDebugExe) {
        $ExePath = $BuildDebugExe
        if ($BuildType -eq "") { $BuildType = "Debug" }
    } elseif (Test-Path $BuildReleaseExe) {
        $ExePath = $BuildReleaseExe
        if ($BuildType -eq "") { $BuildType = "Release" }
    } else {
        Write-Host "错误: 找不到可执行文件" -ForegroundColor Red
        Write-Host "请先编译项目，或指定正确的可执行文件路径" -ForegroundColor Yellow
        exit 1
    }
}

# 检查可执行文件是否存在
if (-not (Test-Path $ExePath)) {
    Write-Host "错误: 找不到可执行文件: $ExePath" -ForegroundColor Red
    Write-Host "请先编译项目，或指定正确的可执行文件路径" -ForegroundColor Yellow
    exit 1
}

# 获取可执行文件的绝对路径和目录
$ExeFullPath = Resolve-Path $ExePath
$ExeDir = Split-Path -Parent $ExeFullPath

# 自动检测构建类型（如果未指定）
if ($BuildType -eq "") {
    if ($ExeFullPath -match "\\Debug\\") {
        $BuildType = "Debug"
    } elseif ($ExeFullPath -match "\\Release\\") {
        $BuildType = "Release"
    } else {
        # 默认使用 Release
        $BuildType = "Release"
        Write-Host "警告: 无法自动检测构建类型，默认使用 Release" -ForegroundColor Yellow
    }
}

# 确定部署目录
if ($OutputDir -ne "") {
    $DeployDir = Join-Path $PSScriptRoot $OutputDir
} else {
    # 默认部署到 public/Debug 或 public/Release
    $DeployDir = Join-Path $PSScriptRoot "public\$BuildType"
}

if (-not (Test-Path $DeployDir)) {
    $DeployDir = New-Item -ItemType Directory -Path $DeployDir -Force
} else {
    $DeployDir = Resolve-Path $DeployDir
}

Write-Host "可执行文件: $ExeFullPath" -ForegroundColor Green
Write-Host "构建类型: $BuildType" -ForegroundColor Green
Write-Host "部署目录: $DeployDir" -ForegroundColor Green

# 查找 Qt6 安装路径
if ($QtPath -eq "") {
    # 尝试从环境变量获取
    if ($env:CMAKE_PREFIX_PATH) {
        $QtPath = $env:CMAKE_PREFIX_PATH
        Write-Host "从 CMAKE_PREFIX_PATH 环境变量获取 Qt 路径: $QtPath" -ForegroundColor Yellow
    } elseif ($env:Qt6_DIR) {
        $QtPath = $env:Qt6_DIR
        Write-Host "从 Qt6_DIR 环境变量获取 Qt 路径: $QtPath" -ForegroundColor Yellow
    } else {
        # 尝试自动查找
        $QtBaseDir = "C:\Qt"
        if (Test-Path $QtBaseDir) {
            Write-Host "在 $QtBaseDir 中搜索 Qt6..." -ForegroundColor Yellow
            
            # 查找所有 Qt6 版本目录
            $QtVersions = Get-ChildItem -Path $QtBaseDir -Directory | Where-Object { $_.Name -match "^6\." } | Sort-Object Name -Descending
            
            foreach ($VersionDir in $QtVersions) {
                # 查找编译器目录
                $CompilerDirs = @("msvc2022_64", "msvc2019_64", "msvc2017_64", "mingw_64", "mingw_32")
                foreach ($CompilerDir in $CompilerDirs) {
                    $TestPath = Join-Path $VersionDir.FullName $CompilerDir
                    $Qt6Config = Join-Path $TestPath "lib\cmake\Qt6\Qt6Config.cmake"
                    if (Test-Path $Qt6Config) {
                        $QtPath = $TestPath
                        Write-Host "找到 Qt6: $QtPath" -ForegroundColor Green
                        break
                    }
                }
                if ($QtPath -ne "") { break }
            }
        }
    }
}

if ($QtPath -eq "" -or -not (Test-Path $QtPath)) {
    Write-Host "错误: 无法找到 Qt6 安装路径" -ForegroundColor Red
    Write-Host "请使用 -QtPath 参数指定 Qt6 安装路径，例如:" -ForegroundColor Yellow
    Write-Host "  .\deploy.ps1 -QtPath 'C:\Qt\6.9.0\msvc2022_64'" -ForegroundColor Yellow
    exit 1
}

$QtBinDir = Join-Path $QtPath "bin"
if (-not (Test-Path $QtBinDir)) {
    Write-Host "错误: Qt bin 目录不存在: $QtBinDir" -ForegroundColor Red
    exit 1
}

Write-Host "Qt6 路径: $QtPath" -ForegroundColor Green
Write-Host "Qt6 bin 目录: $QtBinDir" -ForegroundColor Green

# 查找 windeployqt
$Windeployqt = Join-Path $QtBinDir "windeployqt.exe"
if (-not (Test-Path $Windeployqt)) {
    Write-Host "错误: 找不到 windeployqt.exe: $Windeployqt" -ForegroundColor Red
    exit 1
}

Write-Host "`n开始部署 Qt 依赖..." -ForegroundColor Cyan

# 复制可执行文件到部署目录（如果不同）
if ($DeployDir -ne $ExeDir) {
    Write-Host "复制可执行文件到部署目录..." -ForegroundColor Yellow
    Copy-Item $ExeFullPath $DeployDir -Force
    $DeployExe = Join-Path $DeployDir (Split-Path -Leaf $ExeFullPath)
} else {
    $DeployExe = $ExeFullPath
}

# 运行 windeployqt（根据构建类型选择参数）
$QmlDir = Join-Path $PSScriptRoot "qml"
$DeployArgs = @()
if (Test-Path $QmlDir) {
    Write-Host "使用 QML 目录: $QmlDir" -ForegroundColor Yellow
    $DeployArgs += "--qmldir", $QmlDir
}

if ($BuildType -eq "Debug") {
    $DeployArgs += "--debug"
    Write-Host "使用 Debug 模式部署" -ForegroundColor Yellow
} else {
    $DeployArgs += "--release"
    Write-Host "使用 Release 模式部署" -ForegroundColor Yellow
}

$DeployArgs += $DeployExe

& $Windeployqt $DeployArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n========================================" -ForegroundColor Green
    Write-Host "部署完成！" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "所有 Qt 依赖已复制到: $DeployDir" -ForegroundColor Green
    Write-Host "`n现在可以运行 $DeployExe" -ForegroundColor Cyan
} else {
    Write-Host "`n错误: windeployqt 执行失败" -ForegroundColor Red
    exit 1
}

