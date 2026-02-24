cmake_minimum_required(VERSION 3.10...3.34)

project(CSV_Median_Calculator VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(sources)

option(USE_SYSTEM_BOOST "Use preinstalled Boost if available" ON)

if(USE_SYSTEM_BOOST)
    find_package(Boost COMPONENTS filesystem program_options QUIET)
endif()


if(NOT Boost_FOUND)
    message(STATUS "System Boost not found, will download via FetchContent")
    include(FetchContent)
    FetchContent_Declare(Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.85.0
        GIT_SHALLOW TRUE
    )
    set(BOOST_ENABLE_CMAKE ON)
    set(BOOST_INCLUDE_LIBRARIES "filesystem;program_options")
    FetchContent_MakeAvailable(Boost)
    
include(FetchContent)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0           
)
FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1           
)

FetchContent_MakeAvailable(spdlog)

add_definitions(-DSPDLOG_HEADER_ONLY)

add_executable(CSV_Median_Calculator 
sources/main.cpp

sources/interfaces/IConfigLoader.hpp
sources/interfaces/IFileScanner.hpp
sources/interfaces/IRecordReader.hpp
sources/interfaces/IMedianCalculator.hpp
sources/interfaces/IOutputWriter.hpp
sources/core/TomlConfigLoader.h
sources/core/TomlConfigLoader.cpp
sources/core/FileScanner.h
sources/core/FileScanner.cpp
sources/core/CsvRecordReader.h
sources/core/CsvRecordReader.cpp
sources/core/MedianCalculator.h
sources/core/MedianCalculator.cpp
sources/core/CsvOutputWriter.h
sources/core/CsvOutputWriter.cpp
sources/core/Application.h
sources/core/Application.cpp
sources/multi_thread_module/NewThreadPool.cpp
sources/multi_thread_module/NewThreadPool.h
 )

target_link_libraries(CSV_Median_Calculator PRIVATE Boost::filesystem
                                              Boost::program_options
                                              tomlplusplus::tomlplusplus 
                                              spdlog::spdlog_header_only
)

    target_include_directories(CSV_Median_Calculator PRIVATE 
        ${boost_SOURCE_DIR}/libs/tokenizer/include
        ${boost_SOURCE_DIR}/libs/algorithm/include
        ${boost_SOURCE_DIR}/libs/accumulators/include
        ${boost_SOURCE_DIR}/libs/parameter/include
        ${boost_SOURCE_DIR}/libs/range/include
        ${boost_SOURCE_DIR}/libs/numeric/ublas/include
       ${boost_SOURCE_DIR}/libs/serialization/include
       ${boost_SOURCE_DIR}/libs/array/include
       ${boost_SOURCE_DIR}/libs/numeric/conversion/include

    )
else()
    message(STATUS "Found system Boost version ${Boost_VERSION}")
endif()

add_custom_command(TARGET CSV_Median_Calculator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:CSV_Median_Calculator>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:CSV_Median_Calculator>/resources
    COMMENT "Copying resources to output directory"
)

