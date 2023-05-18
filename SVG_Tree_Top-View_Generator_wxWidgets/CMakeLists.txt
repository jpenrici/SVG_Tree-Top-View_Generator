cmake_minimum_required(VERSION 3.5)

project(SVG_TreeGenerator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    app.h app.cpp
    drawingArea.h drawingArea.cpp
    svg.h
)

set(RESOURCE_FILES
    images/icon_line0.png
    images/icon_line1.png
    images/icon_line2.png
    images/icon_line3.png
    images/icon_line4.png
    images/icon_line5.png
    images/icon_line6.png
    images/icon_line7.png
    images/icon_line8.png
    images/icon_line9.png
    images/icon_line10.png
)

file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/Resources)

# WXWIN : Environment variable configured in Windows for the wxWidgets library.
if (WIN32)
    set(wxWidgets_ROOT_DIR $ENV{WXWIN})
    set(wxWidgets_LIB_DIR  $ENV{WXWIN}/lib/vc_x64_lib)
    set(wxWidgets_USE_LIBS ON)
endif()

find_package(wxWidgets REQUIRED COMPONENTS net core base)

if (wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})

    message("S.O. : " ${CMAKE_SYSTEM_NAME})
    if (WIN32)
        message("Root Dir: " ${wxWidgets_ROOT_DIR} )
        message("Lib dir : " ${wxWidgets_LIB_DIR} )
    else(LINUX)
        message("wxWidgets package found!")
    endif()

    add_executable(${PROJECT_NAME} ${SOURCES})

    target_link_libraries (${PROJECT_NAME} PUBLIC ${wxWidgets_LIBRARIES})

    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    message("Cmake completed successfully!")
else()
    message(FATAL_ERROR "wxWidgets not found!")
endif()
