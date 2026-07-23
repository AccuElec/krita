# SPDX-FileCopyrightText: 2026 KritaFluent Contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Define the KritaFluent UI library
add_library(kritafluentui SHARED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_target_properties(kritafluentui PROPERTIES AUTOMOC ON)

target_sources(kritafluentui PRIVATE
    # Phase 1: Core Architecture
    KisFluentController.cpp
    KisFluentInputEngine.cpp
    KisFluentCanvasOverlay.cpp
    widgets/KisFluentTopBar.cpp
    widgets/KisFluentSliders.cpp

    # Phase 2: Panels & Workflows
    widgets/KisFluentQuickMenu.cpp
    panels/KisFluentBrushPanel.cpp
    panels/KisFluentBrushStudio.cpp
    panels/KisFluentColorPanel.cpp
    panels/KisFluentEyedropperOverlay.cpp
    panels/KisFluentLayerPanel.cpp

    # Phase 3: Tools, Gallery, Animation & Onboarding
    tools/KisFluentSelectionBar.cpp
    tools/KisFluentTransformOverlay.cpp
    gallery/KisFluentGalleryView.cpp
    gallery/KisFluentCanvasPresetPicker.cpp
    panels/KisFluentAnimationTimeline.cpp
    onboarding/KisFluentGestureGuide.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/panels
        ${CMAKE_CURRENT_SOURCE_DIR}/tools
        ${CMAKE_CURRENT_SOURCE_DIR}/gallery
        ${CMAKE_CURRENT_SOURCE_DIR}/onboarding
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
    widgets/KisFluentQuickMenu.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/krita/fluent/widgets
)

install(FILES
    panels/KisFluentBrushPanel.h
    panels/KisFluentBrushStudio.h
    panels/KisFluentColorPanel.h
    panels/KisFluentEyedropperOverlay.h
    panels/KisFluentLayerPanel.h
    panels/KisFluentAnimationTimeline.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/krita/fluent/panels
)

install(FILES
    tools/KisFluentSelectionBar.h
    tools/KisFluentTransformOverlay.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/krita/fluent/tools
)

install(FILES
    gallery/KisFluentGalleryView.h
    gallery/KisFluentCanvasPresetPicker.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/krita/fluent/gallery
)

install(FILES
    onboarding/KisFluentGestureGuide.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/krita/fluent/onboarding
)

# Install the library
install(TARGETS kritafluentui
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
