#[[
  MIT License

  Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
]]

function(setup_compile_params arg_target)
    target_compile_definitions(${arg_target} PRIVATE
        QT_NO_CAST_TO_ASCII
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_FROM_BYTEARRAY
        QT_NO_URL_CAST_FROM_STRING
        QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
        QT_NO_FOREACH
        #QT_TYPESAFE_FLAGS # QtQuick private headers prevent us from enabling this flag.
        QT_USE_QSTRINGBUILDER
        QT_USE_FAST_OPERATOR_PLUS
        QT_DEPRECATED_WARNINGS # Have no effect since 6.0
        QT_DEPRECATED_WARNINGS_SINCE=0x070000
        QT_WARN_DEPRECATED_UP_TO=0x070000 # Since 6.5
        QT_DISABLE_DEPRECATED_BEFORE=0x070000
        QT_DISABLE_DEPRECATED_UP_TO=0x070000 # Since 6.5
    )
    if(NOT (DEFINED __DONT_DISABLE_QT_KEYWORDS AND __DONT_DISABLE_QT_KEYWORDS))
        target_compile_definitions(${arg_target} PRIVATE
            QT_NO_KEYWORDS # Some QtQuick private headers still use the traditional Qt keywords.
        )
    endif()
    if(WIN32) # Needed by both MSVC and MinGW, otherwise some APIs we need will be invisible.
        set(_WIN32_WINNT_WIN10 0x0A00)
        set(NTDDI_WIN10_NI 0x0A00000C)
        target_compile_definitions(${arg_target} PRIVATE
            WINVER=${_WIN32_WINNT_WIN10} _WIN32_WINNT=${_WIN32_WINNT_WIN10}
            _WIN32_IE=${_WIN32_WINNT_WIN10} NTDDI_VERSION=${NTDDI_WIN10_NI}
        )
    else()
        target_compile_definitions(${arg_target} PRIVATE
            QT_STRICT_ITERATORS # On Windows we need to re-compile Qt with this flag enabled, so only enable it on non-Windows platforms.
        )
    endif()
    if(MSVC)
        target_compile_definitions(${arg_target} PRIVATE
            _CRT_NON_CONFORMING_SWPRINTFS _CRT_SECURE_NO_WARNINGS
            _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS
            _CRT_NONSTDC_NO_DEPRECATE _SCL_SECURE_NO_WARNINGS
            _SCL_SECURE_NO_DEPRECATE _ENABLE_EXTENDED_ALIGNED_STORAGE
            _USE_MATH_DEFINES NOMINMAX UNICODE _UNICODE
            WIN32_LEAN_AND_MEAN WINRT_LEAN_AND_MEAN
        )
        target_compile_options(${arg_target} PRIVATE
            /bigobj /utf-8 $<$<NOT:$<CONFIG:Debug>>:/fp:fast /GT /Gw /Gy /guard:cf /Zc:inline>
            /Zc:auto /Zc:forScope /Zc:implicitNoexcept /Zc:noexceptTypes /Zc:referenceBinding
            /Zc:rvalueCast /Zc:sizedDealloc /Zc:strictStrings /Zc:throwingNew /Zc:trigraphs
            /Zc:wchar_t
        )
        target_link_options(${arg_target} PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:/OPT:REF /OPT:ICF /OPT:LBR /GUARD:CF>
            /DYNAMICBASE /NXCOMPAT /LARGEADDRESSAWARE /WX
        )
        set(__target_type "UNKNOWN")
        get_target_property(__target_type ${arg_target} TYPE)
        if(__target_type STREQUAL "EXECUTABLE")
            target_compile_options(${arg_target} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/GA>)
            target_link_options(${arg_target} PRIVATE /TSAWARE)
        endif()
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            target_link_options(${arg_target} PRIVATE /SAFESEH)
        endif()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            target_link_options(${arg_target} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/HIGHENTROPYVA>)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1900) # Visual Studio 2015
            target_compile_options(${arg_target} PRIVATE /Zc:threadSafeInit)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1910) # Visual Studio 2017 version 15.0
            target_compile_options(${arg_target} PRIVATE /permissive- /Zc:ternary)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1912) # Visual Studio 2017 version 15.5
            target_compile_options(${arg_target} PRIVATE /Zc:alignedNew)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1913) # Visual Studio 2017 version 15.6
            target_compile_options(${arg_target} PRIVATE /Zc:externConstexpr)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1914) # Visual Studio 2017 version 15.7
            target_compile_options(${arg_target} PRIVATE /Zc:__cplusplus)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1915) # Visual Studio 2017 version 15.8
            target_compile_options(${arg_target} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/JMC>)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1920) # Visual Studio 2019 version 16.0
            target_link_options(${arg_target} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/CETCOMPAT>)
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                target_compile_options(${arg_target} PRIVATE /d2FH4)
            endif()
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1921) # Visual Studio 2019 version 16.1
            target_compile_options(${arg_target} PRIVATE /Zc:char8_t)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1923) # Visual Studio 2019 version 16.3
            target_compile_options(${arg_target} PRIVATE /Zc:externC)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1924) # Visual Studio 2019 version 16.4
            target_compile_options(${arg_target} PRIVATE /Zc:hiddenFriend)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1925) # Visual Studio 2019 version 16.5
            target_compile_options(${arg_target} PRIVATE
                /Zc:preprocessor /Zc:tlsGuards $<$<NOT:$<CONFIG:Debug>>:/QIntel-jcc-erratum> # /Qspectre-load
            )
        #elseif(MSVC_VERSION GREATER_EQUAL 1912) # Visual Studio 2017 version 15.5
        #    target_compile_options(${arg_target} PRIVATE /Qspectre)
        endif()
        #[[if((MSVC_VERSION GREATER_EQUAL 1927) AND (CMAKE_SIZEOF_VOID_P EQUAL 8)) # Visual Studio 2019 version 16.7
            target_compile_options(${arg_target} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/guard:ehcont>)
            target_link_options(${arg_target} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/guard:ehcont>)
        endif()]]
        if(MSVC_VERSION GREATER_EQUAL 1928) # Visual Studio 2019 version 16.8 & 16.9
            target_compile_options(${arg_target} PRIVATE /Zc:lambda /Zc:zeroSizeArrayNew)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1929) # Visual Studio 2019 version 16.10
            target_compile_options(${arg_target} PRIVATE /await:strict)
        elseif(MSVC_VERSION GREATER_EQUAL 1900) # Visual Studio 2015
            target_compile_options(${arg_target} PRIVATE /await)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1930) # Visual Studio 2022 version 17.0
            target_compile_options(${arg_target} PRIVATE /options:strict)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1931) # Visual Studio 2022 version 17.1
            target_compile_options(${arg_target} PRIVATE /Zc:static_assert)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1932) # Visual Studio 2022 version 17.2
            target_compile_options(${arg_target} PRIVATE /Zc:__STDC__)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1934) # Visual Studio 2022 version 17.4
            target_compile_options(${arg_target} PRIVATE /Zc:enumTypes /Zc:gotoScope /Zc:nrvo)
        endif()
        if(MSVC_VERSION GREATER_EQUAL 1935) # Visual Studio 2022 version 17.5
            target_compile_options(${arg_target} PRIVATE /Zc:templateScope /Zc:checkGwOdr)
        endif()
    else()
        target_compile_options(${arg_target} PRIVATE
            -Wall -Wextra -Werror
            $<$<NOT:$<CONFIG:Debug>>:-ffunction-sections -fdata-sections> # -fcf-protection=full? -Wa,-mno-branches-within-32B-boundaries?
        )
        if(APPLE)
            target_link_options(${arg_target} PRIVATE
                $<$<NOT:$<CONFIG:Debug>>:-Wl,-dead_strip>
            )
        else()
            target_link_options(${arg_target} PRIVATE
                $<$<NOT:$<CONFIG:Debug>>:-Wl,--gc-sections>
            )
        endif()
    endif()
endfunction()

function(setup_gui_app arg_target)
    set_target_properties(${arg_target} PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER org.wangwenx190.${arg_target}
        MACOSX_BUNDLE_BUNDLE_VERSION 1.0.0.0
        MACOSX_BUNDLE_SHORT_VERSION_STRING 1.0
    )
endfunction()

function(setup_package_export arg_target arg_path arg_public arg_alias arg_private)
    include(GNUInstallDirs)
    set(__targets ${arg_target})
    if(TARGET ${arg_target}_resources_1)
        list(APPEND __targets ${arg_target}_resources_1) # Ugly hack to workaround a CMake configure error.
    endif()
    install(TARGETS ${__targets}
        EXPORT ${arg_target}Targets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${arg_path}"
    )
    export(EXPORT ${arg_target}Targets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${arg_target}Targets.cmake"
        NAMESPACE ${PROJECT_NAME}::
    )
    install(FILES ${arg_public} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${arg_path}")
    install(FILES ${arg_alias} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${arg_path}")
    install(FILES ${arg_private} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${arg_path}/private")
    install(EXPORT ${arg_target}Targets
        FILE ${arg_target}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
endfunction()

function(deploy_qt_runtime arg_target)
    if(NOT (DEFINED Qt5_FOUND OR DEFINED Qt6_FOUND))
        find_package(QT NAMES Qt6 Qt5 QUIET COMPONENTS Core)
    endif()
    if(NOT (Qt5_FOUND OR Qt6_FOUND))
        message("You need to install the QtCore module first to be able to deploy the Qt libraries.")
        return()
    endif()
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
    if(NOT DEFINED QT_QMAKE_EXECUTABLE) # QT_QMAKE_EXECUTABLE is usually defined by QtCreator.
        get_target_property(QT_QMAKE_EXECUTABLE Qt::qmake IMPORTED_LOCATION)
    endif()
    if(NOT EXISTS "${QT_QMAKE_EXECUTABLE}")
        message("Cannot find the QMake executable.")
        return()
    endif()
    get_filename_component(QT_BIN_DIRECTORY "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(QT_DEPLOY_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${QT_BIN_DIRECTORY}")
    if(NOT EXISTS "${QT_DEPLOY_EXECUTABLE}")
        message("Cannot find the deployqt tool.")
        return()
    endif()
    set(__is_quick_app FALSE)
    cmake_parse_arguments(DEPLOY_QT_RUNTIME_ARGS "" "QML_SOURCE_DIR;QML_IMPORT_DIR" "" ${ARGN})
    if(WIN32)
        set(__old_deploy_params)
        if(${QT_VERSION_MAJOR} LESS 6)
            set(__old_deploy_params --no-webkit2 --no-angle)
        endif()
        set(__quick_deploy_params)
        if(DEFINED DEPLOY_QT_RUNTIME_ARGS_QML_SOURCE_DIR)
            set(__is_quick_app TRUE)
            set(__quick_deploy_params
                --dir "$<TARGET_FILE_DIR:${arg_target}>/qml"
                --qmldir "${DEPLOY_QT_RUNTIME_ARGS_QML_SOURCE_DIR}"
            )
        endif()
        if(DEFINED DEPLOY_QT_RUNTIME_ARGS_QML_IMPORT_DIR)
            set(__is_quick_app TRUE)
            set(__quick_deploy_params
                ${__quick_deploy_params}
                --qmlimport "${DEPLOY_QT_RUNTIME_ARGS_QML_IMPORT_DIR}"
            )
        endif()
        add_custom_command(TARGET ${arg_target} POST_BUILD COMMAND
            "${QT_DEPLOY_EXECUTABLE}"
            $<$<CONFIG:Debug>:--debug>
            $<$<OR:$<CONFIG:MinSizeRel>,$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:--release>
            --libdir "$<TARGET_FILE_DIR:${arg_target}>"
            --plugindir "$<TARGET_FILE_DIR:${arg_target}>/plugins"
            --no-translations
            --no-system-d3d-compiler
            --no-virtualkeyboard
            --no-compiler-runtime
            --no-opengl-sw
            --force
            --verbose 0
            ${__quick_deploy_params}
            ${__old_deploy_params}
            "$<TARGET_FILE:${arg_target}>"
        )
    elseif(APPLE)
        set(__quick_deploy_params)
        if(DEFINED DEPLOY_QT_RUNTIME_ARGS_QML_SOURCE_DIR)
            set(__is_quick_app TRUE)
            set(__quick_deploy_params
                -qmldir="${DEPLOY_QT_RUNTIME_ARGS_QML_SOURCE_DIR}"
                -qmlimport="${PROJECT_BINARY_DIR}/qml"
            )
        endif()
        if(DEFINED DEPLOY_QT_RUNTIME_ARGS_QML_IMPORT_DIR)
            set(__is_quick_app TRUE)
            set(__quick_deploy_params
                ${__quick_deploy_params}
                -qmlimport="${DEPLOY_QT_RUNTIME_ARGS_QML_IMPORT_DIR}"
            )
        endif()
        add_custom_command(TARGET ${arg_target} POST_BUILD COMMAND
            "${QT_DEPLOY_EXECUTABLE}"
            "$<TARGET_BUNDLE_DIR:${arg_target}>"
            -verbose=0
            ${__quick_deploy_params}
        )
    elseif(UNIX)
        # TODO
    endif()
    include(GNUInstallDirs)
    install(TARGETS ${arg_target}
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
    if(QT_VERSION VERSION_GREATER_EQUAL "6.3")
        set(__deploy_script)
        if(${__is_quick_app})
            qt_generate_deploy_qml_app_script(
                TARGET ${arg_target}
                FILENAME_VARIABLE __deploy_script
                #MACOS_BUNDLE_POST_BUILD
                NO_UNSUPPORTED_PLATFORM_ERROR
                DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
            )
        else()
            qt_generate_deploy_app_script(
                TARGET ${arg_target}
                FILENAME_VARIABLE __deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
            )
        endif()
        install(SCRIPT "${__deploy_script}")
    endif()
endfunction()

function(setup_translations arg_target)
    # Qt5's CMake functions to create translations lack many features
    # we need and what's worse, they also have a severe bug which will
    # wipe out our .ts files' contents every time we call them, so we
    # really can't use them until Qt6 (the functions have been completely
    # re-written in Qt6 and according to my experiments they work reliably
    # now finally).
    if(NOT DEFINED Qt6LinguistTools_FOUND)
        find_package(Qt6 QUIET COMPONENTS LinguistTools)
    endif()
    if(NOT Qt6LinguistTools_FOUND)
        message("You need to install the Qt Linguist Tools first to be able to create translations.")
        return()
    endif()
    cmake_parse_arguments(TRANSLATION_ARGS "" "TS_DIR;QM_DIR;INSTALL_DIR" "LOCALES" ${ARGN})
    if(NOT DEFINED TRANSLATION_ARGS_LOCALES)
        message("You need to specify at least one locale.")
        return()
    endif()
    set(__ts_dir translations)
    if(DEFINED TRANSLATION_ARGS_TS_DIR)
        set(__ts_dir "${TRANSLATION_ARGS_TS_DIR}")
    endif()
    set(__qm_dir "${PROJECT_BINARY_DIR}/translations")
    if(DEFINED TRANSLATION_ARGS_QM_DIR)
        set(__qm_dir "${TRANSLATION_ARGS_QM_DIR}")
    endif()
    set(__inst_dir translations)
    if(DEFINED TRANSLATION_ARGS_INSTALL_DIR)
        set(__inst_dir "${TRANSLATION_ARGS_INSTALL_DIR}")
    endif()
    set(__ts_files)
    foreach(__locale ${TRANSLATION_ARGS_LOCALES})
        list(APPEND __ts_files "${__ts_dir}/${arg_target}_${__locale}.ts")
    endforeach()
    set_source_files_properties(${__ts_files} PROPERTIES
        OUTPUT_LOCATION "${__qm_dir}"
    )
    set(__qm_files)
    qt_add_translations(${arg_target}
        TS_FILES ${__ts_files}
        QM_FILES_OUTPUT_VARIABLE __qm_files
        LUPDATE_OPTIONS
            -no-obsolete # Don't keep vanished translation contexts.
        LRELEASE_OPTIONS
            -compress # Compress the QM file if the file size can be decreased siginificantly.
            -nounfinished # Don't include unfinished translations (to save file size).
            -removeidentical # Don't include translations that are the same with their original texts (to save file size).
    )
    install(FILES ${__qm_files} DESTINATION "${__inst_dir}")
endfunction()
