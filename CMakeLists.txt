include(CTest)
enable_testing()

# Include header files
include_directories(${CMAKE_SOURCE_DIR}/glew-2.1.0/include)
include_directories(${CMAKE_SOURCE_DIR}/glew-2.1.0/include/GL)
include_directories(${CMAKE_SOURCE_DIR}/freeglut/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Manually set the paths to the libraries if find_package is not suitable
link_directories(${CMAKE_SOURCE_DIR}/glew-2.1.0/lib)
link_directories(${CMAKE_SOURCE_DIR}/freeglut/lib)

# Automatically add all source files from the src directory
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(HodgkinHuxley ${SOURCES})

# Set architecture to 32-bit
# set_target_properties(HodgkinHuxley PROPERTIES COMPILE_OPTIONS "-m32" LINK_FLAGS "-m32")

# Or set architecture to 64-bit
set_target_properties(HodgkinHuxley PROPERTIES COMPILE_OPTIONS "-m64" LINK_FLAGS "-m64")


# Link the executable with the libraries
# target_link_libraries(HodgkinHuxley libGLEW.so libGLU.so libGL.so libglut.so)

if(WIN32)
    target_link_libraries(HodgkinHuxley glew32 glu32 opengl32 freeglut)
elseif(UNIX)
    target_link_libraries(HodgkinHuxley GLEW GLU GL glut)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
