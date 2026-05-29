@echo off
REM Qt6 依赖部署批处理脚本
REM 使用方法: deploy.bat [Qt路径] [输出目录]

setlocal enabledelayedexpansion

set "EXE_PATH="
set "QT_PATH=%~1"
set "OUTPUT_DIR=%~2"
set "BUILD_TYPE="

echo ========================================
echo Qt6 依赖部署脚本
echo ========================================
echo.

REM 确定可执行文件路径和构建类型
if "%EXE_PATH%"=="" (
    if exist "public\Debug\CleanerQt.exe" (
        set "EXE_PATH=public\Debug\CleanerQt.exe"
        set "BUILD_TYPE=Debug"
    ) else if exist "public\Release\CleanerQt.exe" (
        set "EXE_PATH=public\Release\CleanerQt.exe"
        set "BUILD_TYPE=Release"
    ) else if exist "public\CleanerQt.exe" (
        set "EXE_PATH=public\CleanerQt.exe"
    ) else if exist "build\bin\Debug\CleanerQt.exe" (
        set "EXE_PATH=build\bin\Debug\CleanerQt.exe"
        set "BUILD_TYPE=Debug"
    ) else if exist "build\bin\Release\CleanerQt.exe" (
        set "EXE_PATH=build\bin\Release\CleanerQt.exe"
        set "BUILD_TYPE=Release"
    ) else (
        echo 错误: 找不到可执行文件
        echo 请先编译项目
        exit /b 1
    )
)

REM 检查可执行文件
if not exist "%EXE_PATH%" (
    echo 错误: 找不到可执行文件: %EXE_PATH%
    echo 请先编译项目
    exit /b 1
)

echo 可执行文件: %EXE_PATH%

REM 自动检测构建类型（如果未指定）
if "%BUILD_TYPE%"=="" (
    echo %EXE_PATH% | findstr /i "\\Debug\\" >nul
    if %ERRORLEVEL% equ 0 (
        set "BUILD_TYPE=Debug"
    ) else (
        echo %EXE_PATH% | findstr /i "\\Release\\" >nul
        if %ERRORLEVEL% equ 0 (
            set "BUILD_TYPE=Release"
        ) else (
            set "BUILD_TYPE=Release"
            echo 警告: 无法自动检测构建类型，默认使用 Release
        )
    )
)

echo 构建类型: %BUILD_TYPE%

REM 如果未指定 Qt 路径，尝试自动查找
if "%QT_PATH%"=="" (
    if defined CMAKE_PREFIX_PATH (
        set "QT_PATH=%CMAKE_PREFIX_PATH%"
        echo 从 CMAKE_PREFIX_PATH 环境变量获取 Qt 路径: %QT_PATH%
    ) else if defined Qt6_DIR (
        set "QT_PATH=%Qt6_DIR%"
        echo 从 Qt6_DIR 环境变量获取 Qt 路径: %QT_PATH%
    ) else (
        REM 尝试在 C:\Qt 中查找
        if exist "C:\Qt" (
            echo 在 C:\Qt 中搜索 Qt6...
            for /d %%v in ("C:\Qt\6.*") do (
                for %%c in (msvc2022_64 msvc2019_64 msvc2017_64 mingw_64 mingw_32) do (
                    if exist "%%v\%%c\lib\cmake\Qt6\Qt6Config.cmake" (
                        set "QT_PATH=%%v\%%c"
                        echo 找到 Qt6: !QT_PATH!
                        goto :found_qt
                    )
                )
            )
        )
    )
)

:found_qt
if "%QT_PATH%"=="" (
    echo 错误: 无法找到 Qt6 安装路径
    echo 请指定 Qt6 路径，例如:
    echo   deploy.bat "C:\Qt\6.9.0\msvc2022_64"
    exit /b 1
)

set "QT_BIN=%QT_PATH%\bin"
if not exist "%QT_BIN%" (
    echo 错误: Qt bin 目录不存在: %QT_BIN%
    exit /b 1
)

set "WINDEPLOYQT=%QT_BIN%\windeployqt.exe"
if not exist "%WINDEPLOYQT%" (
    echo 错误: 找不到 windeployqt.exe: %WINDEPLOYQT%
    exit /b 1
)

echo Qt6 路径: %QT_PATH%
echo Qt6 bin 目录: %QT_BIN%
echo.

REM 确定部署目录
if "%OUTPUT_DIR%"=="" (
    set "DEPLOY_DIR=%~dp0public\%BUILD_TYPE%"
) else (
    set "DEPLOY_DIR=%~dp0%OUTPUT_DIR%"
)
if not exist "%DEPLOY_DIR%" mkdir "%DEPLOY_DIR%"

REM 如果可执行文件不在部署目录，先复制过去
for %%F in ("%EXE_PATH%") do set "EXE_NAME=%%~nxF"
if /i not "%EXE_PATH%"=="%DEPLOY_DIR%\%EXE_NAME%" (
    echo 复制可执行文件到部署目录...
    copy /Y "%EXE_PATH%" "%DEPLOY_DIR%\" >nul
    set "EXE_PATH=%DEPLOY_DIR%\CleanerQt.exe"
)

echo 开始部署 Qt 依赖...
echo.

REM 检查 QML 目录并运行 windeployqt
set "QML_DIR=%~dp0qml"
if "%BUILD_TYPE%"=="Debug" (
    echo 使用 Debug 模式部署
    if exist "%QML_DIR%" (
        echo 使用 QML 目录: %QML_DIR%
        "%WINDEPLOYQT%" --qmldir "%QML_DIR%" --debug "%EXE_PATH%"
    ) else (
        echo 警告: QML 目录不存在，跳过 QML 部署
        "%WINDEPLOYQT%" --debug "%EXE_PATH%"
    )
) else (
    echo 使用 Release 模式部署
    if exist "%QML_DIR%" (
        echo 使用 QML 目录: %QML_DIR%
        "%WINDEPLOYQT%" --qmldir "%QML_DIR%" --release "%EXE_PATH%"
    ) else (
        echo 警告: QML 目录不存在，跳过 QML 部署
        "%WINDEPLOYQT%" --release "%EXE_PATH%"
    )
)

if %ERRORLEVEL% equ 0 (
    echo.
    echo ========================================
    echo 部署完成！
    echo ========================================
    echo 所有 Qt 依赖已复制到可执行文件目录
    echo.
    echo 现在可以运行 %DEPLOY_DIR%\CleanerQt.exe
) else (
    echo.
    echo 错误: windeployqt 执行失败
    exit /b 1
)

endlocal

