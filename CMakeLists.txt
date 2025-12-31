cmake_minimum_required(VERSION 3.16)

project(CleanerQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt6 组件
# 如果找不到 Qt6，请设置 CMAKE_PREFIX_PATH 环境变量或通过 -DCMAKE_PREFIX_PATH 参数指定 Qt6 安装路径
# 例如: cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2022_64" ...
find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found. Please set CMAKE_PREFIX_PATH to your Qt6 installation directory.")
endif()

# 启用 Qt6 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 源文件
set(SOURCES
    main.cpp
    src/DiskScanner.cpp
    src/CleanerController.cpp
)

set(HEADERS
    src/DiskScanner.h
    src/FileInfo.h
    src/CleanerController.h
)

# QML 资源文件
set(QML_FILES
    qml/main.qml
    qml/components/DiskSelector.qml
    qml/components/ScanProgress.qml
    qml/components/FileListView.qml
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} qml/resources.qrc)

# 链接 Qt6 库
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
)

# 设置 Qt6 导入路径
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE FALSE
)

# 设置 QML 导入路径（用于开发时）
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_QML_DEBUG
)

