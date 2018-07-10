
# Check if examples are enabled
if(NOT OPTION_BUILD_EXAMPLES)
    return()
endif()

# Example applications
add_subdirectory(openll-example)
add_subdirectory(labeling-at-point)
add_subdirectory(point-based-layouting)
