find_package(Qt5Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

function(add_exec_test test_name test_file)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE fbksd::core Qt5::Test ${ARGN} )
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction(add_exec_test)


add_subdirectory(mock)

add_exec_test(TestSharedMemory core/TestSharedMemory.cpp)
add_exec_test(TestSceneInfo core/TestSceneInfo.cpp)

add_exec_test(TestBenchmarkClient libclient/TestBenchmarkClient.cpp
    fbksd::client fbksd::libbenchmark
)
target_compile_definitions(TestBenchmarkClient
    PRIVATE
        -DRENDERER_FILE="$<TARGET_FILE:mockrenderer>"
)
add_dependencies(TestBenchmarkClient mockrenderer)

add_exec_test(TestBenchmarkManager libbenchmark/TestBenchmarkManager.cpp
    fbksd::libbenchmark
)
target_compile_definitions(TestBenchmarkManager
    PRIVATE
        -DRENDERER_FILE="$<TARGET_FILE:mockrenderer>"
        -DCLIENT_FILE="$<TARGET_FILE:mockclient>"
)
add_dependencies(TestBenchmarkClient mockrenderer mockrenderer)

add_exec_test(TestIqa libiqa/TestIqa.cpp fbksd::iqa)
add_exec_test(TestImg libiqa/TestImg.cpp fbksd::iqa)
