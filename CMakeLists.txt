cmake_minimum_required(VERSION 3.18)
enable_testing()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Packaging
set(CPACK_PACKAGE_VENDOR "virtualmatador")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_BINARY_DIR}/daemon/postinst;${CMAKE_BINARY_DIR}/daemon/prerm")
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libssl1.1")

include(CPack)

# Sub Directories
add_subdirectory("extern/cli")
add_subdirectory("extern/config")
add_subdirectory("extern/compose")
add_subdirectory("extern/jsonio")
add_subdirectory("extern/daemon")
add_subdirectory("src")
add_subdirectory("test")

configure_file("${CMAKE_SOURCE_DIR}/config.json" "config.json")
add_executable("${CMAKE_PROJECT_NAME}" "main.cpp")
target_link_libraries("${CMAKE_PROJECT_NAME}" "${CMAKE_PROJECT_NAME}-lib")
target_compile_definitions("${CMAKE_PROJECT_NAME}" PRIVATE
    PROJECT_NAME="${CMAKE_PROJECT_NAME}"
    PROJECT_VERSION="${CMAKE_PROJECT_VERSION}"
    ROOT="html"
)
install(TARGETS "${CMAKE_PROJECT_NAME}"
    DESTINATION "${HOME_DIR}/${DOMAIN}/${SUBDOMAIN}"
)

include(ExternalProject)
ExternalProject_Add("wasm"
    CMAKE_ARGS
        -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -D parent_current_binary_dir=${CMAKE_CURRENT_BINARY_DIR}
        -D parent_current_source_dir=${CMAKE_CURRENT_SOURCE_DIR}
        -D parent_source_dir=${CMAKE_SOURCE_DIR}
        -D CMAKE_TOOLCHAIN_FILE=/usr/local/dev/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
        -D CMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/wasm"
    BUILD_ALWAYS On
)
add_custom_target("html" ALL
    COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/html"
    COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/html"
    COMMAND cp "${CMAKE_SOURCE_DIR}/widgets/*.js" "${CMAKE_CURRENT_BINARY_DIR}/html"
    COMMAND cp "${CMAKE_SOURCE_DIR}/widgets/*.css" "${CMAKE_CURRENT_BINARY_DIR}/html"
    COMMAND cp "${CMAKE_CURRENT_BINARY_DIR}/wasm/wasm.js" "${CMAKE_CURRENT_BINARY_DIR}/html"
    COMMAND cp "${CMAKE_CURRENT_BINARY_DIR}/wasm/wasm.wasm" "${CMAKE_CURRENT_BINARY_DIR}/html"
)
add_dependencies("html" "wasm")
add_dependencies("${CMAKE_PROJECT_NAME}" "html")
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}html" DESTINATION "${HOME_DIR}/${DOMAIN}/${SUBDOMAIN}")
