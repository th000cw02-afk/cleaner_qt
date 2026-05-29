get_target_property(_qt_qmake_location Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qt_qmake_location}" DIRECTORY)

function(link_test_spdlog_if_available target)
    if(CLEANER_QT_HAS_SPDLOG)
        target_link_libraries(${target} PRIVATE spdlog::spdlog)
        target_compile_definitions(${target} PRIVATE CLEANER_QT_HAS_SPDLOG)
    endif()
endfunction()

function(add_qt_test name)
    add_executable(${name} ${ARGN})
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${name} PRIVATE Qt6::Core Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES
        ENVIRONMENT "PATH=${_qt_bin_dir};$ENV{PATH}"
    )
endfunction()

# CLI smoke fixture
set(SMOKE_FIXTURE_DIR "${CMAKE_BINARY_DIR}/test_fixtures/smoke_dir")
file(MAKE_DIRECTORY "${SMOKE_FIXTURE_DIR}")
file(WRITE "${SMOKE_FIXTURE_DIR}/sample.txt" "smoke test data\n")

add_qt_test(test_folder_tree
    test_folder_tree.cpp
    ../src/models/FolderNode.cpp
    ../src/models/TopFilesTracker.cpp
    ../src/scan/ScanOptions.cpp
)

add_qt_test(test_treemap_layout
    test_treemap_layout.cpp
    ../src/models/FolderNode.cpp
    ../src/models/TreemapLayout.cpp
    ../src/models/ExtensionColorMap.cpp
)

add_qt_test(test_extension_stats
    test_extension_stats.cpp
    ../src/models/ExtensionStatsAggregator.cpp
    ../src/models/ExtensionColorMap.cpp
)

add_qt_test(test_hardlink_registry
    test_hardlink_registry.cpp
    ../src/models/HardlinkRegistry.cpp
)

add_qt_test(test_extension_color_map
    test_extension_color_map.cpp
    ../src/models/ExtensionColorMap.cpp
)

add_qt_test(test_file_index_builder
    test_file_index_builder.cpp
    ../src/models/FileIndexBuilder.cpp
    ../src/models/HardlinkRegistry.cpp
)

add_qt_test(test_report_exporter
    test_report_exporter.cpp
    ../src/scan/ReportExporter.cpp
    ../src/models/FolderNode.cpp
)

add_qt_test(test_scan_snapshot
    test_scan_snapshot.cpp
    ../src/models/ScanSnapshot.cpp
    ../src/models/FolderNode.cpp
)

add_qt_test(test_scan_options
    test_scan_options.cpp
    ../src/scan/ScanOptions.cpp
)

add_qt_test(test_file_index_model
    test_file_index_model.cpp
    ../src/models/FileIndexModel.cpp
)

add_qt_test(test_folder_tree_model
    test_folder_tree_model.cpp
    ../src/models/FolderTreeModel.cpp
    ../src/models/FolderNode.cpp
)

add_qt_test(test_cleanup_scanners
    test_cleanup_scanners.cpp
    ../src/scan/CleanupScanners.cpp
)

add_qt_test(test_app_settings
    test_app_settings.cpp
    ../src/settings/AppSettings.cpp
    ../src/scan/ScanOptions.cpp
    ../src/logging/AppLogger.cpp
    ../src/logging/QtMessageHandler.cpp
)
link_test_spdlog_if_available(test_app_settings)

add_qt_test(test_app_logger
    test_app_logger.cpp
    ../src/logging/AppLogger.cpp
    ../src/logging/QtMessageHandler.cpp
    ../src/settings/AppSettings.cpp
    ../src/scan/ScanOptions.cpp
)
link_test_spdlog_if_available(test_app_logger)

add_qt_test(test_top_files_tracker
    test_top_files_tracker.cpp
    ../src/models/TopFilesTracker.cpp
)

add_qt_test(test_theme_manager
    test_theme_manager.cpp
    ../src/ui/ThemeManager.cpp
)

add_qt_test(test_scan_module_registry
    test_scan_module_registry.cpp
    ../src/scan/modules/ScanModuleRegistry.cpp
    ../src/scan/CleanupScanners.cpp
)

add_qt_test(test_format_utils
    test_format_utils.cpp
    ../src/util/FormatUtils.cpp
)

if(WIN32)
    add_qt_test(test_scan_worker
        test_scan_worker.cpp
        ../src/scan/ScanWorker.cpp
        ../src/scan/ScanOptions.cpp
        ../src/logging/AppLogger.cpp
        ../src/logging/QtMessageHandler.cpp
        ../src/settings/AppSettings.cpp
        ../src/models/FolderNode.cpp
        ../src/models/TopFilesTracker.cpp
        ../src/models/FileIndexBuilder.cpp
        ../src/models/HardlinkRegistry.cpp
        ../src/models/ExtensionStatsAggregator.cpp
        ../src/models/ExtensionColorMap.cpp
        ../src/platform/FileIdHelper_win.cpp
    )
    link_test_spdlog_if_available(test_scan_worker)
    target_link_libraries(test_scan_worker PRIVATE advapi32)

    set(_cli_out_csv "${CMAKE_BINARY_DIR}/test_cli_out.csv")
    add_test(NAME test_cli_smoke
        COMMAND ${CMAKE_COMMAND} -E env
            "PATH=${_qt_bin_dir};$<TARGET_FILE_DIR:CleanerQt>;$ENV{PATH}"
            ${CMAKE_COMMAND}
            -DEXE=$<TARGET_FILE:CleanerQt>
            -DFIXTURE=${SMOKE_FIXTURE_DIR}
            -DOUT=${_cli_out_csv}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/run_cli_smoke.cmake
    )
endif()
