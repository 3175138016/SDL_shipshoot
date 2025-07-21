cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  


project(shotter VERSION 0.1.0 LANGUAGES C CXX)
message("==================================")

message(STATUS "编译数据库状态: ${CMAKE_EXPORT_COMPILE_COMMANDS}")


# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)



# 设置C++编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()



#设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})


set(SOURCES
        "src/main.cpp"
        "src/Game.cpp"
        "src/SceneTitle.cpp"
        "src/Scene.cpp"
        "src/SceneEnd.cpp"
        "src/SceneMain.cpp"

)

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})


# 查找并载入Cmake预设
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

# 添加可执行文件
if(WIN32)
    list(APPEND SOURCES resources.rc)
endif()

add_executable(${TARGET} ${SOURCES})



# 链接库
target_link_libraries(${TARGET}
                        ${SDL2_LIBRARIES}
                        SDL2_image::SDL2_image
                        SDL2_mixer::SDL2_mixer
                        SDL2_ttf::SDL2_ttf
                        
                        )


# 不弹出控制台窗口
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
endif()

