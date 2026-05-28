cmake_minimum_required(VERSION 3.16)



project(CleanerQt VERSION 2.0.0 LANGUAGES CXX)



set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)



if((NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL "") AND DEFINED ENV{Qt6_DIR})

    set(CMAKE_PREFIX_PATH $ENV{Qt6_DIR})

endif()



if(WIN32 AND (NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL ""))

    set(QT_BASE_DIR "C:/Qt")

    if(EXISTS ${QT_BASE_DIR})

        file(GLOB QT_VERSION_DIRS "${QT_BASE_DIR}/*")

        list(SORT QT_VERSION_DIRS)

        list(REVERSE QT_VERSION_DIRS)

        foreach(QT_VERSION_DIR ${QT_VERSION_DIRS})

            get_filename_component(VERSION_NAME ${QT_VERSION_DIR} NAME)

            if(IS_DIRECTORY ${QT_VERSION_DIR} AND VERSION_NAME MATCHES "^6\\.")

                foreach(COMPILER_DIR msvc2022_64 msvc2019_64 mingw_64)

                    set(QT_PATH "${QT_VERSION_DIR}/${COMPILER_DIR}")

                    if(EXISTS "${QT_PATH}/lib/cmake/Qt6/Qt6Config.cmake")

                        set(CMAKE_PREFIX_PATH ${QT_PATH})

                        break()

                    endif()

                endforeach()

                if(DEFINED CMAKE_PREFIX_PATH)

                    break()

                endif()

            endif()

        endforeach()

    endif()

endif()



find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Widgets Concurrent Test Network)

option(CLEANER_QT_FETCH_SPDLOG "Download spdlog via FetchContent when not vendored" OFF)
set(CLEANER_QT_HAS_SPDLOG OFF)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/spdlog/CMakeLists.txt")
    add_subdirectory(third_party/spdlog ${CMAKE_BINARY_DIR}/spdlog EXCLUDE_FROM_ALL)
    set(CLEANER_QT_HAS_SPDLOG ON)
else()
    find_package(spdlog CONFIG QUIET)
    if(spdlog_FOUND)
        set(CLEANER_QT_HAS_SPDLOG ON)
    elseif(CLEANER_QT_FETCH_SPDLOG)
        include(FetchContent)
        FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.14.1
        )
        FetchContent_MakeAvailable(spdlog)
        set(CLEANER_QT_HAS_SPDLOG ON)
    endif()
endif()
if(NOT CLEANER_QT_HAS_SPDLOG)
    message(STATUS "spdlog not found; using built-in file logger (vendor third_party/spdlog for full spdlog)")
endif()

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTORCC ON)



set(PUBLIC_DIR ${CMAKE_SOURCE_DIR}/public)

if(CMAKE_CONFIGURATION_TYPES)

    set(PUBLIC_OUTPUT_DIR ${PUBLIC_DIR}/$<CONFIG>)

else()

    if(CMAKE_BUILD_TYPE)

        string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)

        set(PUBLIC_OUTPUT_DIR ${PUBLIC_DIR}/${BUILD_TYPE_UPPER})

    else()

        set(PUBLIC_OUTPUT_DIR ${PUBLIC_DIR}/Release)

    endif()

endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PUBLIC_OUTPUT_DIR})



set(SOURCES

    main.cpp

    src/AnalyzerController.cpp

    src/DiskScanner.cpp

    src/models/FolderNode.cpp

    src/models/FolderTreeModel.cpp

    src/models/TopFilesTracker.cpp

    src/models/ExtensionStatsAggregator.cpp

    src/models/ExtensionColorMap.cpp

    src/models/HardlinkRegistry.cpp

    src/models/FileIndexBuilder.cpp

    src/models/FileIndexModel.cpp

    src/models/ScanSnapshot.cpp

    src/models/TreemapLayout.cpp

    src/ui/ThemeManager.cpp

    src/settings/AppSettings.cpp

    src/util/FormatUtils.cpp

    src/logging/AppLogger.cpp

    src/logging/QtMessageHandler.cpp

    src/scan/ScanOptions.cpp

    src/scan/ScanWorker.cpp

    src/scan/ParallelScanWorker.cpp

    src/scan/ReportExporter.cpp

    src/scan/CleanupScanners.cpp

    src/scan/modules/ScanModuleRegistry.cpp

    src/cli/CliRunner.cpp

    src/platform/FileDeleter_win.cpp

    src/platform/FileIdHelper_win.cpp

    src/platform/UacHelper_win.cpp

    src/platform/FileWatcherService_win.cpp

    src/platform/AutoUpdateChecker.cpp

)



set(HEADERS

    src/AnalyzerController.h

    src/DiskScanner.h

    src/FileInfo.h

    src/models/FolderNode.h

    src/models/FolderTreeModel.h

    src/models/TopFilesTracker.h

    src/models/ExtensionStatsAggregator.h

    src/models/ExtensionColorMap.h

    src/models/HardlinkRegistry.h

    src/models/FileIndexEntry.h

    src/models/FileIndexBuilder.h

    src/models/FileIndexModel.h

    src/models/ScanSnapshot.h

    src/models/TreemapLayout.h

    src/ui/ThemeManager.h

    src/settings/AppSettings.h

    src/logging/AppLogger.h

    src/scan/ScanOptions.h

    src/scan/ScanWorker.h

    src/scan/ParallelScanWorker.h

    src/scan/ReportExporter.h

    src/scan/CleanupScanners.h

    src/scan/modules/ScanModule.h

    src/cli/CliRunner.h

    src/platform/FileDeleter_win.h

    src/platform/FileIdHelper_win.h

    src/platform/UacHelper_win.h

    src/platform/FileWatcherService_win.h

    src/platform/AutoUpdateChecker.h

)



if(WIN32)

    list(APPEND SOURCES

        src/scan/NtfsMftScanner_win.cpp

        src/scan/NtfsMftScanWorker.cpp

        src/scan/NtfsMftDirectReader_win.cpp

    )

    list(APPEND HEADERS

        src/scan/NtfsMftScanner_win.h

        src/scan/NtfsMftScanWorker.h

        src/scan/NtfsMftDirectReader_win.h

    )

endif()



add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} qml/resources.qrc)



target_link_libraries(${PROJECT_NAME}

    Qt6::Core

    Qt6::Quick

    Qt6::Qml

    Qt6::Widgets

    Qt6::Concurrent

    Qt6::Network

)

if(CLEANER_QT_HAS_SPDLOG)
    target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
    target_compile_definitions(${PROJECT_NAME} PRIVATE CLEANER_QT_HAS_SPDLOG)
endif()



if(WIN32)

    target_link_libraries(${PROJECT_NAME} shell32)

    if(EXISTS "${CMAKE_SOURCE_DIR}/app/app.manifest")

        target_sources(${PROJECT_NAME} PRIVATE app/app.manifest)

    endif()

endif()



set_target_properties(${PROJECT_NAME} PROPERTIES

    WIN32_EXECUTABLE TRUE

    MACOSX_BUNDLE FALSE

)



target_compile_definitions(${PROJECT_NAME} PRIVATE QT_QML_DEBUG)



option(CLEANER_QT_BUILD_TESTS "Build unit and integration tests" ON)

if(CLEANER_QT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()



if(WIN32)

    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)

    if(WINDEPLOYQT_EXECUTABLE)

        add_custom_target(deploy-release

            COMMAND ${WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR}/qml --release $<TARGET_FILE:${PROJECT_NAME}>

            DEPENDS ${PROJECT_NAME}

        )

    endif()

endif()

