cmake_minimum_required(VERSION 3.16)

project(cimdraw LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 Qt 自动处理机制
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 添加必要的宏定义
add_definitions(-DQT_DISABLE_DEPRECATED_UP_TO=0)
add_definitions(-DQT_SHARED)

if(MSVC)
    add_compile_options(/utf-8 /FS)
elseif(MINGW)
    add_compile_options("-fexec-charset=UTF-8")
endif()

# 区分 Debug / Release 输出路径（与 .pro 中一致）
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(EXEC_DIR ${CMAKE_SOURCE_DIR}/../exe/bin64)
    set(LIB_DIR ${CMAKE_SOURCE_DIR}/../exe/lib64)
else()
    set(EXEC_DIR ${CMAKE_SOURCE_DIR}/../exe/bin)
    set(LIB_DIR ${CMAKE_SOURCE_DIR}/../exe/lib)
endif()

# 设置可执行输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

# 设置 MOC、UI、RCC 输出目录（与 .pro 一致）
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/cimdraw/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/cimdraw/lib)
set(CMAKE_AUTOMOC_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/cimdraw/moc)
set(CMAKE_AUTOUIC_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/cimdraw/ui)
set(CMAKE_AUTORCC_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/cimdraw/rcc)

# 设置 include 路径
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/datamodule)
include_directories(${CMAKE_SOURCE_DIR}/src/wiring)
include_directories(${CMAKE_SOURCE_DIR}/src/wiring/base)
include_directories(${CMAKE_SOURCE_DIR}/src/wiring/power)
include_directories(${CMAKE_SOURCE_DIR}/src/qtpropertybrowser)

# 查找 Qt 模块
find_package(Qt6 COMPONENTS Core Gui Widgets Sql Xml PrintSupport Network REQUIRED)
# core5compat 只在 Qt6 使用
find_package(Qt6 COMPONENTS Core5Compat QUIET)

# 设置 qtpropertybrowser 源文件
set(QTPROPERTY_SOURCES
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qtbuttonpropertybrowser.cpp
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qtgroupboxpropertybrowser.cpp 
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qtpropertybrowser.cpp
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qtpropertymanager.cpp
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qteditorfactory.cpp
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qtvariantproperty.cpp
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qttreepropertybrowser.cpp
    ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser/qtpropertybrowserutils.cpp
)

# 获取项目其他源文件（CONFIGURE_DEPENDS：新增/删除 src 下 .cpp 时会触发重新配置，避免 LNK2019）
file(GLOB_RECURSE PROJECT_SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS src/*.h)

# 排除qtpropertybrowser的源文件，我们会单独添加
list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*/qtpropertybrowser/.*\\.cpp$")

# 只保留电力工程入口，避免子工程混入其他 main
list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*/src/main_power\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/main_power.cpp)

# 显式加入算法源文件，避免 GLOB 未刷新导致 LNK2019（LzVisibilityGraph 等）
list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*LzVisibilityGraph\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/algorithm/LzVisibilityGraph.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*TopologyGraphIndex\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/topology/TopologyGraphIndex.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*LzWiringSymbolStyle\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/wiring/base/LzWiringSymbolStyle.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*LzAppConfig\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/LzAppConfig.cpp)

# 组合所有源文件
set(ALL_SRC_FILES ${PROJECT_SRC_FILES} ${QTPROPERTY_SOURCES})

# 获取 UI 文件
file(GLOB UI_FILES *.ui)

# 添加资源
set(RESOURCE_FILES cimdraw.qrc)

# 最终构建目标
add_executable(cimdraw WIN32  # WIN32 指定 GUI 应用
    ${ALL_SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

# 链接 Qt 模块
target_link_libraries(cimdraw PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Xml
    Qt6::PrintSupport
    Qt6::Network
)

# 如果 Qt6::Core5Compat 可用，则添加
if(Qt6Core5Compat_FOUND)
    target_link_libraries(cimdraw PRIVATE Qt6::Core5Compat)
endif()

# 设置 Windows 平台特定选项
if(WIN32)
    target_compile_definitions(cimdraw PRIVATE WIN32_LEAN_AND_MEAN)
    if(MSVC)
        target_link_options(cimdraw PRIVATE /DEBUG)
    elseif(MINGW)
        target_link_options(cimdraw PRIVATE "-g")
    endif()
endif()

# 添加 lib 路径（按平台）
link_directories(${LIB_DIR})

# 添加构建类型符号（Release 带调试符号）
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")


set(CMAKE_PREFIX_PATH "C:/Qt/Qt6.5.3/6.5.3/msvc2019_64")
set(QT6_DIR "C:/Qt/Qt6.5.3/6.5.3/Src")

option(BUILD_CIMDRAW_TESTS "Build QTest executable cimdraw_topology_tests" OFF)
if(BUILD_CIMDRAW_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    set(LZ_TEST_SOURCES ${ALL_SRC_FILES})
    list(FILTER LZ_TEST_SOURCES EXCLUDE REGEX ".*/src/main_power\\.cpp$")
    add_executable(cimdraw_topology_tests
        tests/test_power_topology_main.cpp
        tests/test_power_topology.cpp
        tests/test_cim_model.cpp
        tests/test_cim_importer.cpp
        tests/test_cim_ui.cpp
        ${LZ_TEST_SOURCES}
        ${HEADER_FILES}
        ${UI_FILES}
        ${RESOURCE_FILES}
    )
    set_target_properties(cimdraw_topology_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests_bin")
    target_link_libraries(cimdraw_topology_tests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Xml
        Qt6::PrintSupport
        Qt6::Network
        Qt6::Test
    )
    if(Qt6Core5Compat_FOUND)
        target_link_libraries(cimdraw_topology_tests PRIVATE Qt6::Core5Compat)
    endif()
    if(WIN32)
        target_compile_definitions(cimdraw_topology_tests PRIVATE WIN32_LEAN_AND_MEAN)
        if(MSVC)
        elseif(MINGW)
        endif()
    endif()

    add_executable(cimdraw_cim_importer_tests
        tests/test_cim_importer_main.cpp
        tests/test_cim_importer.cpp
        ${LZ_TEST_SOURCES}
        ${HEADER_FILES}
        ${UI_FILES}
        ${RESOURCE_FILES}
    )
    set_target_properties(cimdraw_cim_importer_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests_bin")
    target_link_libraries(cimdraw_cim_importer_tests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Xml
        Qt6::PrintSupport
        Qt6::Network
        Qt6::Test
    )
    if(Qt6Core5Compat_FOUND)
        target_link_libraries(cimdraw_cim_importer_tests PRIVATE Qt6::Core5Compat)
    endif()
    if(WIN32)
        target_compile_definitions(cimdraw_cim_importer_tests PRIVATE WIN32_LEAN_AND_MEAN)
        if(MSVC)
        elseif(MINGW)
        endif()
    endif()

    add_executable(cimdraw_cim_graphics_tests
        tests/test_cim_graphics_main.cpp
        tests/test_cim_graphics.cpp
        ${LZ_TEST_SOURCES}
        ${HEADER_FILES}
        ${UI_FILES}
        ${RESOURCE_FILES}
    )
    set_target_properties(cimdraw_cim_graphics_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests_bin")
    target_link_libraries(cimdraw_cim_graphics_tests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Xml
        Qt6::PrintSupport
        Qt6::Network
        Qt6::Test
    )
    if(Qt6Core5Compat_FOUND)
        target_link_libraries(cimdraw_cim_graphics_tests PRIVATE Qt6::Core5Compat)
    endif()
    if(WIN32)
        target_compile_definitions(cimdraw_cim_graphics_tests PRIVATE WIN32_LEAN_AND_MEAN)
        if(MSVC)
        elseif(MINGW)
        endif()
    endif()

    add_executable(cimdraw_cim_ui_tests
        tests/test_cim_ui_main.cpp
        tests/test_cim_ui.cpp
        ${LZ_TEST_SOURCES}
        ${HEADER_FILES}
        ${UI_FILES}
        ${RESOURCE_FILES}
    )
    set_target_properties(cimdraw_cim_ui_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests_bin")
    target_link_libraries(cimdraw_cim_ui_tests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Xml
        Qt6::PrintSupport
        Qt6::Network
        Qt6::Test
    )
    if(Qt6Core5Compat_FOUND)
        target_link_libraries(cimdraw_cim_ui_tests PRIVATE Qt6::Core5Compat)
    endif()
    if(WIN32)
        target_compile_definitions(cimdraw_cim_ui_tests PRIVATE WIN32_LEAN_AND_MEAN)
        if(MSVC)
        elseif(MINGW)
        endif()
    endif()
endif()
