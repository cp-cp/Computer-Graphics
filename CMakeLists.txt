cmake_minimum_required(VERSION 3.0.0)
project(HelloGL VERSION 0.1.0)

# 使用 C++ 17 标准
set(CMAKE_CXX_STANDARD 17)

# 设置代码文件
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)

# 添加头文件
set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include/)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)
include_directories(${HEADER_DIR} ${LIB_DIR})

# 查找 GLFW
find_package(glfw3 REQUIRED)

# 查找 GLM
find_package(glm REQUIRED)

# 查找 Assimp
find_package(ASSIMP REQUIRED)

# 执行编译命令
set(SOURCES ${SRC_DIR}glad.c ${SRC_DIR}main.cpp)
add_executable(HelloGL ${SOURCES})

# 链接系统的 OpenGL 框架
if (APPLE)
    target_link_libraries(HelloGL "-framework OpenGL")
endif()

# 链接 GLFW, GLM 和 Assimp
target_link_libraries(HelloGL glfw ${GLM_LIBRARIES} ${ASSIMP_LIBRARIES})

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

