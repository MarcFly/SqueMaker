cmake_minimum_required(VERSION 3.1...3.18) #require a minimum cmake version to build
# If the version is less than tested, make a warning about it, else try to use 3.18
if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#------------------------------------------------------------------------------------------------
# ADD SUBPROJECTS # Add where SqueLib is
#------------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../SqueLib ${CMAKE_CURRENT_BINARY_DIR}/SqueLib)
# Dear ImGui
set(imgui_sque_include ${SqueLib_extra}/imgui_squelib)
set(imgui_include ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(imgui_src 
    ${imgui_include}/imgui_demo.cpp
    ${imgui_include}/imgui_draw.cpp
    ${imgui_include}/imgui_widgets.cpp
    ${imgui_include}/imgui_tables.cpp
    ${imgui_include}/imgui.cpp
    ${imgui_sque_include}/imgui_impl_squelib.cpp)

# ImNodes - ImGui Node Editor
set(imnodes_include ${CMAKE_CURRENT_SOURCE_DIR}/external/imnodes)
set(imnodes_src
    ${imnodes_include}/imnodes.cpp
)

# TinyGLTF - GLTF/Json/Texture loader
set(tinygltf_include ${CMAKE_CURRENT_SOURCE_DIR}/external/tinygltf)
set(tinygltf_src
    ${tinygltf_include}/json.hpp
    ${tinygltf_include}/stb_image.h
    ${tinygltf_include}/stb_image_write.h
    ${tinygltf_include}/tiny_gltf.h)

#------------------------------------------------------------------------------------------------
# SETUP CURRENT PROJECT
#------------------------------------------------------------------------------------------------
project(SqueMaker       VERSION 2020.1
                        DESCRIPTION "Multiplatform C++ Engine"
                        LANGUAGES C CXX)
# C/CXX Requirements
set(CMAKE_CXX_STANDARD 17) # Currently using <filesystem> for easy output and folder creation
# Probably I can downgrade to C++11 because most new things I don't really use with thought into it
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON) # Android Turns it off (i still don't know why but it should)
#------------------------------------------------------------------------------------------------
# SETUP BUILD PATHS
#------------------------------------------------------------------------------------------------
execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/builds)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/builds)
#------------------------------------------------------------------------------------------------
# REQUIRED PLATFORM SPECIFICS
#------------------------------------------------------------------------------------------------

# Get all code
file(GLOB_RECURSE SqueMaker_src  ${CMAKE_CURRENT_SOURCE_DIR}/Code/*.cpp)
file(GLOB_RECURSE SqueMaker_head ${CMAKE_CURRENT_SOURCE_DIR}/Code/*.h)
SqueLib_PrepareBuild(SqueMaker MarcTorresJimenez "${SqueMaker_src};${SqueMaker_head}" "${CMAKE_CURRENT_SOURCE_DIR}/Code")
SqueLib_Package(${CMAKE_CURRENT_SOURCE_DIR}/Assets "")

#------------------------------------------------------------------------------------------------
# SETTING FINAL PROPERTIES FOR THE PROJECT
#------------------------------------------------------------------------------------------------
target_include_directories(SqueMaker 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Code
        ${SqueLib_include}
        ${imgui_sque_include}
        ${imgui_include}
        ${imnodes_include}
        ${tinyglf_include}
)


target_sources(SqueMaker PUBLIC
    "${imgui_src}"
    "${SqueMaker_head}"
    "${imnodes_src}"
    "${tinygltf_src}"
)

# Group Sources as Intended
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Code" 
    PREFIX "Engine" 
    FILES ${SqueMaker_src} ${SqueMaker_head}
)
source_group(libs/imgui FILES ${imgui_src})
source_group(libs/tinygltf FILES ${tinygltf_src})