# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(p7zip)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

# 编译 NAPI 入口动态库
add_library(entry SHARED napi_init.cpp)
target_link_libraries(entry PUBLIC libace_napi.z.so)

# 添加 7-Zip 子目录（里面会生成动态库 lib7za.so）
add_subdirectory(7z2501-src/CPP/7zip/CMAKE/7za)

# 注意：不再需要 target_compile_options(sevenzip_static ...) 这行，
# 因为我们现在使用动态库，编译选项已在子目录中定义。