cmake_minimum_required(VERSION 3.16)

project(CleanerQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 Qt6 组件
# 如果找不到 Qt6，请设置 CMAKE_PREFIX_PATH 环境变量或通过 -DCMAKE_PREFIX_PATH 参数指定 Qt6 安装路径
# 例如: cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2022_64" ...

# 尝试从环境变量读取 Qt6 路径（如果 CMAKE_PREFIX_PATH 未设置）
if((NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL "") AND DEFINED ENV{Qt6_DIR})
    set(CMAKE_PREFIX_PATH $ENV{Qt6_DIR})
endif()

# 在 Windows 上尝试自动查找 Qt6 安装路径
if(WIN32 AND (NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL ""))
    set(QT_BASE_DIR "C:/Qt")
    
    if(EXISTS ${QT_BASE_DIR})
        message(STATUS "Searching for Qt6 in ${QT_BASE_DIR}...")
        
        # 常见的编译器目录名称
        set(QT_COMPILER_DIRS
            "msvc2022_64"
            "msvc2019_64"
            "msvc2017_64"
            "mingw_64"
            "mingw_32"
        )
        
        # 获取 C:/Qt 下的所有子目录
        file(GLOB QT_VERSION_DIRS "${QT_BASE_DIR}/*")
        
        # 按版本号降序排序，优先使用较新版本
        list(SORT QT_VERSION_DIRS)
        list(REVERSE QT_VERSION_DIRS)
        
        foreach(QT_VERSION_DIR ${QT_VERSION_DIRS})
            # 检查是否是目录且名称以 6. 开头（Qt6 版本）
            get_filename_component(VERSION_NAME ${QT_VERSION_DIR} NAME)
            if(IS_DIRECTORY ${QT_VERSION_DIR} AND VERSION_NAME MATCHES "^6\\.")
                # 在该版本目录下查找编译器目录
                foreach(COMPILER_DIR ${QT_COMPILER_DIRS})
                    set(QT_PATH "${QT_VERSION_DIR}/${COMPILER_DIR}")
                    if(EXISTS "${QT_PATH}/lib/cmake/Qt6/Qt6Config.cmake")
                        set(CMAKE_PREFIX_PATH ${QT_PATH})
                        message(STATUS "Found Qt6 at: ${QT_PATH}")
                        break()
                    endif()
                endforeach()
                
                # 如果找到了，退出外层循环
                if(DEFINED CMAKE_PREFIX_PATH)
                    break()
                endif()
            endif()
        endforeach()
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR 
        "Qt6 not found!\n"
        "Please set CMAKE_PREFIX_PATH to your Qt6 installation directory.\n"
        "Example: cmake -DCMAKE_PREFIX_PATH=\"C:\\Qt\\6.x.x\\msvc2022_64\" ..\n"
        "Or set the Qt6_DIR environment variable to the Qt6 installation directory."
    )
endif()

# 启用 Qt6 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置输出目录为 public，根据构建类型区分 Debug 和 Release
set(PUBLIC_DIR ${CMAKE_SOURCE_DIR}/public)
# 对于多配置生成器（如 Visual Studio），使用 CMAKE_CFG_INTDIR
# 对于单配置生成器，使用 CMAKE_BUILD_TYPE
if(CMAKE_CONFIGURATION_TYPES)
    # Visual Studio 等多配置生成器
    set(PUBLIC_OUTPUT_DIR ${PUBLIC_DIR}/$<CONFIG>)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PUBLIC_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PUBLIC_OUTPUT_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PUBLIC_OUTPUT_DIR})
else()
    # 单配置生成器（如 MinGW Makefiles）
    if(CMAKE_BUILD_TYPE)
        string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
        set(PUBLIC_OUTPUT_DIR ${PUBLIC_DIR}/${BUILD_TYPE_UPPER})
    else()
        set(PUBLIC_OUTPUT_DIR ${PUBLIC_DIR}/Release)
    endif()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PUBLIC_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PUBLIC_OUTPUT_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PUBLIC_OUTPUT_DIR})
endif()

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

# Windows 部署配置
if(WIN32)
    # 查找 windeployqt 工具
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    
    if(WINDEPLOYQT_EXECUTABLE)
        # 为 Debug 和 Release 分别创建部署目标
        add_custom_target(deploy
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt dependencies..."
            COMMENT "Deploy target - use 'cmake --build . --target deploy --config Debug/Release'"
        )
        
        # Debug 部署目标
        add_custom_target(deploy-debug
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt dependencies (Debug) to ${PUBLIC_DIR}/Debug..."
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                --qmldir ${CMAKE_SOURCE_DIR}/qml
                --debug
                $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Deploying Qt6 dependencies (Debug) for ${PROJECT_NAME}"
            DEPENDS ${PROJECT_NAME}
        )
        
        # Release 部署目标
        add_custom_target(deploy-release
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt dependencies (Release) to ${PUBLIC_DIR}/Release..."
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                --qmldir ${CMAKE_SOURCE_DIR}/qml
                --release
                $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Deploying Qt6 dependencies (Release) for ${PROJECT_NAME}"
            DEPENDS ${PROJECT_NAME}
        )
        
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
        message(STATUS "Use 'cmake --build . --target deploy-debug --config Debug' for Debug")
        message(STATUS "Use 'cmake --build . --target deploy-release --config Release' for Release")
    else()
        message(WARNING "windeployqt not found. Qt dependencies will not be automatically deployed.")
        message(WARNING "Please run deploy.ps1 script manually or set Qt6_DIR/bin to PATH")
    endif()
endif()

