# SPDX-FileCopyrightText: 2026 KritaFluent Contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Define the KritaFluent UI library
add_library(kritafluentui SHARED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_target_properties(kritafluentui PROPERTIES AUTOMOC ON)

target_sources(kritafluentui PRIVATE
    KisFluentController.cpp
    KisFluentInputEngine.cpp
    KisFluentCanvasOverlay.cpp
    widgets/KisFluentTopBar.cpp
    widgets/KisFluentSliders.cpp
)

# Define the export macro when building this library
target_compile_definitions(kritafluentui PRIVATE KRITA_FLUENT_UI_BUILDING)

target_include_directories(kritafluentui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/krita/fluent>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

target_link_libraries(kritafluentui
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        kritaui
        kritaimage
        kritaglobal
        kritawidgets
)

# Install headers
install(FILES
    KisFluentController.h
    KisFluentInputEngine.h
    KisFluentCanvasOverlay.h
    KisFluentUiExport.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/krita/fluent
)

install(FILES
    widgets/KisFluentTopBar.h
    widgets/KisFluentSliders.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/krita/fluent/widgets
)

# Install the library
install(TARGETS kritafluentui
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
