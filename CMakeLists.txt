cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME FaceMaskDetection)
project(${PROJECT_NAME})

add_definitions(-std=c++11)
add_definitions("-Wall")

set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencv-3.4.10")
set(Ncnn_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ncnn")
set(RepidJson_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson")
set(OpenCV_LIBS opencv_core 
                opencv_highgui 
                opencv_imgproc 
                opencv_imgcodecs 
                opencv_videoio)

if(True)
    find_package(OpenMP)
    if(NOT TARGET OpenMP::OpenMP_CXX AND (OpenMP_CXX_FOUND OR OPENMP_FOUND))
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

include_directories(${OpenCV_DIR}/include
                    ${Ncnn_DIR}/include/ncnn
                    ${RepidJson_DIR}/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${OpenCV_DIR}/lib/
                 ${Ncnn_DIR}/lib/
                 ${CMAKE_CURRENT_SOURCE_DIR}/build/)

FILE(GLOB_RECURSE SRC "include/*.h" "src/*.cpp")

add_executable(example "example/main.cpp")
# target_link_libraries(example ncnn ${OpenCV_LIBS})

# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(example FaceMaskDetection ${OpenCV_LIBS})

# add_definitions(-fopenmp)
add_definitions(-fPIC)
link_libraries(${PROJECT_NAME} ncnn ${OpenCV_LIBS})
add_library(${PROJECT_NAME} SHARED ${SRC})
if(OpenMP_CXX_FOUND)
    if(NCNN_CMAKE_VERBOSE)
        message("Building with OpenMP")
    endif()
    target_link_libraries(example PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()



function(copy_changed_file filename destination)
    set(_copy 1)
    set(_src_name ${filename})
    get_filename_component(_name ${_src_name} NAME)
    set(_dst_name ${destination}/${_name})

    # lock a file to ensure that no two cmake processes
    # try to copy the same file at the same time in parallel
    # builds

    # string(SHA1 _hash ${_dst_name})
    # set(_lock_file ${CMAKE_BINARY_DIR}/${_hash}.lock)
    # file(LOCK ${_lock_file} GUARD FUNCTION)

    if(EXISTS ${_dst_name})
        file(TIMESTAMP ${_dst_name} _dst_time)
        file(TIMESTAMP ${_src_name} _src_time)
        if(${_dst_time} STREQUAL ${_src_time})
            # skip this library if the destination and source
            # have the same time stamp
            return()
        else()
            # file has changed remove
            file(REMOVE ${_dst_name})
        endif()
    endif()

    if(_copy)
        message(STATUS "Copying ${_name} to ${destination}")
        file(COPY ${_src_name} DESTINATION ${destination})
    endif()
endfunction()

include(GetPrerequisites)

file(GLOB FOUND_FILES "/home//autobuild/wangzhen/project_task/self-detection/graduate-face-hat-detection/build/example" )

if (FOUND_FILES)
    get_prerequisites(${FOUND_FILES} OUT 1 1 "" "$ENV{LD_LIBRARY_PATH}")
    if (OUT)
        foreach(_item ${OUT})
            gp_resolve_item("${FOUND_FILES}" "${_item}" "" "$ENV{LD_LIBRARY_PATH}" DEPENDENCY_PATH)
            get_filename_component(RESOLVED_DEPENDENCY_PATH "${DEPENDENCY_PATH}" REALPATH)
            foreach(_i ${DEPENDENCY_PATH})
                copy_changed_file("${_i}" "${CMAKE_INSTALL_PREFIX}/lib")
            endforeach(_i ${_item})  
            foreach(_i ${RESOLVED_DEPENDENCY_PATH})
                copy_changed_file("${_i}" "${CMAKE_INSTALL_PREFIX}/lib")
            endforeach(_i ${_item})  
        endforeach(_item)
    endif()
endif()
