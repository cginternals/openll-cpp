
#
# External dependencies
#

find_package(cpplocate REQUIRED)
find_package(cppassist REQUIRED)
find_package(glm REQUIRED)
find_package(glbinding REQUIRED)
find_package(globjects REQUIRED)
find_package(glfw3 REQUIRED)


# 
# Executable name and options
# 

# Target name
set(target point-based-layouting)

# Exit here if required dependencies are not met
message(STATUS "Example ${target}")


#
# Sources
#

set(sources

    main.cpp
    datapath.inl
    PointDrawable.cpp
    PointDrawable.h
    RectangleDrawable.cpp
    RectangleDrawable.h
    benchmark.cpp
    benchmark.h
    GeoData.cpp
    GeoData.h
    ScreenAlignedQuad.cpp
    ScreenAlignedQuad.h
)


# 
# Create executable
# 

# Build executable
add_executable(${target}
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})


# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source/include
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    glfw
    cpplocate::cpplocate
    cppassist::cppassist
    glbinding::glbinding
    globjects::globjects
    ${META_PROJECT_NAME}::openll
)


# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
    GLFW_INCLUDE_NONE
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)


# 
# Deployment
# 

# Executable
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_EXAMPLES} COMPONENT examples
    BUNDLE  DESTINATION ${INSTALL_EXAMPLES} COMPONENT examples
)
