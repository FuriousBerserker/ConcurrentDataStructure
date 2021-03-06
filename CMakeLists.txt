cmake_minimum_required(VERSION 2.8.7)

macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

project(ConcurrentDataStructure)
enable_testing()
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})
find_package(Threads REQUIRED)
use_cxx11()
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/*.cpp)
foreach(TEST_SRC ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${TEST_SRC} NAME_WE)

        #Add compile target
        add_executable(${testName} ${TEST_SRC})
        list(APPEND testNames ${testName})
        #link to Boost libraries AND your targets and dependencies
        #target_link_libraries(${testName} ${Boost_LIBRARIES} [YOUR TARGETS/LIBRARIES])
        target_link_libraries(${testName} ${Boost_LIBRARIES})
        target_link_libraries(${testName} Threads::Threads)
        #I like to move testing binaries into a testBin directory
        set_target_properties(${testName} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)
        target_compile_options(${testName} PRIVATE "-D __USE_GNU_CAS")
        #Finally add it to test execution - 
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName} 
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin 
                 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/${testName}
        )
endforeach()
add_custom_target(unittests ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
    ${CMAKE_CTEST_COMMAND}
    DEPENDS ${testNames}
    WORKING_DIRECTORY ${CMAKE_BINAEY_DIR})


