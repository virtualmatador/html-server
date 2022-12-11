cmake_minimum_required(VERSION 3.18)
enable_testing()
add_custom_target("name_hpp" DEPENDS "name.hpp")
file(GLOB widgets "${CMAKE_SOURCE_DIR}/widgets/*.cpp")
foreach (widget ${widgets})
    get_filename_component(base "${widget}" NAME_WE)
    list(APPEND include_list "${base}")
endforeach()
add_custom_command(OUTPUT "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo "// auto generated" > "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo ${include_list} | xargs -n 1 echo | xargs -i echo "class {};" >> "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo "template <class T> std::string get_name()" >> "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo "{" >> "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo "    if constexpr(std::is_same_v<T, void>) return \"\";" >> "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo ${include_list} | xargs -n 1 echo | xargs -i echo "    else if constexpr(std::is_same_v<T, {}>) return \"{}\";" >> "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo "    else static_assert(!sizeof(T));" >> "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo "    return \"\";" >> "name.hpp"
    COMMAND ${CMAKE_COMMAND} -E echo "}" >> "name.hpp"
    DEPENDS ${widgets}
    VERBATIM
)
add_library("${CMAKE_PROJECT_NAME}-lib" STATIC
    ${widgets} "server.cpp" "session.cpp" "${CMAKE_SOURCE_DIR}/index.cpp")
set_property(TARGET "${CMAKE_PROJECT_NAME}-lib" PROPERTY CXX_STANDARD 17)
target_include_directories("${CMAKE_PROJECT_NAME}-lib" INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_include_directories("${CMAKE_PROJECT_NAME}-lib" PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries("${CMAKE_PROJECT_NAME}-lib" PUBLIC
    "z"
    "ssl"
    "crypto"
    "pthread"
    "cli"
    "config"
    "compose"
    "jsonio"
)
add_dependencies("${CMAKE_PROJECT_NAME}-lib" "name_hpp")
