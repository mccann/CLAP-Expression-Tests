cmake_minimum_required(VERSION 3.20.2)

set(PARENT_PROJECT_NAME "${PROJECT_NAME}")

project(note-logger)

add_library(${PROJECT_NAME} MODULE
        ${PROJECT_NAME}.cpp
        ${PROJECT_NAME}-gui.cpp
    )
target_link_libraries(${PROJECT_NAME} clap fmt imgui-clap-support)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            BUNDLE True
            BUNDLE_EXTENSION clap
            MACOSX_BUNDLE_GUI_IDENTIFIER org.${PARENT_PROJECT_NAME}.${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
            MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.plist.in
            )
    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_MAC=1)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_LINUX=1)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".clap" PREFIX "")
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_WIN=1)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".clap" PREFIX "")
endif()
