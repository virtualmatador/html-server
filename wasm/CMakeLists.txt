cmake_minimum_required(VERSION 3.13)
project("wasm")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB widgets "${parent_source_dir}/widgets/*.cpp")
foreach (widget ${widgets})
    get_filename_component(base "${widget}" NAME_WE)
    list(APPEND include_list "${base}")
endforeach()
add_custom_command(OUTPUT "bind.cpp"
    COMMAND ${CMAKE_COMMAND} -E echo "#include <emscripten/bind.h>" > "bind.cpp"
    COMMAND ${CMAKE_COMMAND} -E echo ${include_list} | xargs -n 1 echo | xargs -i echo "#include \"widgets/{}.h\""  >> "bind.cpp"
    COMMAND ${CMAKE_COMMAND} -E echo "EMSCRIPTEN_BINDINGS(my_module)" >> "bind.cpp"
    COMMAND ${CMAKE_COMMAND} -E echo "{" >> "bind.cpp"
    COMMAND ${CMAKE_COMMAND} -E echo ${include_list} | xargs -n 1 echo | xargs -i echo "    emscripten::function(\"{}_render\", &{}::template render<false>);" >> "bind.cpp"
    COMMAND ${CMAKE_COMMAND} -E echo "}" >> "bind.cpp"
    DEPENDS ${widgets}
    VERBATIM
)
add_executable("${CMAKE_PROJECT_NAME}"
    ${widgets} "${CMAKE_CURRENT_BINARY_DIR}/bind.cpp"
)
set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 17)
set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "wasm"
    LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/pre.js"
)
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE
    "${parent_source_dir}"
    "${parent_current_source_dir}/src"
    "${parent_current_binary_dir}/src"
)
target_link_options("${CMAKE_PROJECT_NAME}" PRIVATE
    --pre-js "${CMAKE_CURRENT_SOURCE_DIR}/pre.js"
    --bind
)
target_compile_definitions("${CMAKE_PROJECT_NAME}" PRIVATE
    CLIENT_SIDE
)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/wasm" DESTINATION ".")
