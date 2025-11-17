cmake_minimum_required (VERSION 3.10)

# For better VSCode IntelliSense (ignored by Visual Studio)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add static runtimes (only when using MSVC)
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("STDISCM_P3")

# Change output directory of the exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set base path for 3rd party reference projects 
include(FetchContent)

# Set directory of third party stuff
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE PATH "Base Directory of Libs" FORCE)

# set directory of assets
set(RESOURCES_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Media CACHE PATH "Base Directory of resources" FORCE)

# set the source path for the project
set(PROJ_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src CACHE PATH "Project SRC" FORCE)

# add everything in the src folder
file(GLOB_RECURSE PROJ_SRC CONFIGURE_DEPENDS "${PROJ_SRC_PATH}/*.[ch]pp")

FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.x
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(SFML)

add_executable (P3 "${PROJ_SRC}")

# Change Target C++ compile to ver 20
target_compile_features(P3 PRIVATE cxx_std_20)

# Link the SFML graphics lib
target_link_libraries(P3 PRIVATE 
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

add_custom_command(
  TARGET P3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Media
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Media
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET P3 PROPERTY CXX_STANDARD 20)
endif()
