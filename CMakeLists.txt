cmake_minimum_required(VERSION 3.31)
project(early_retirement)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(fetch_github_content GIT_REPOSITORY https://github.com/CesarBerriot/fetch_github_content.git GIT_TAG 1.0.0)
FetchContent_MakeAvailable(fetch_github_content)
include(${fetch_github_content_SOURCE_DIR}/fetch_github_content.cmake)

fetch_github_content(
    discover_sources CesarBerriot 1.0.0 INCLUDE_DEFAULT
    dial CesarBerriot 1.0.2
)

add_executable(early_retirement WIN32)
dial_link_target(early_retirement)
discover_sources(early_retirement)
target_compile_definitions(early_retirement PRIVATE MODULE_NAME="Early Retirement")