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

# 配置 Qt 查找路径，确保新构建目录也能直接完成 find_package
set(CMAKE_PREFIX_PATH "C:/Qt/Qt6.5.3/6.5.3/msvc2019_64")
set(Qt6_DIR "C:/Qt/Qt6.5.3/6.5.3/msvc2019_64/lib/cmake/Qt6")

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

# 只保留当前工程入口，避免子工程混入其他 main
list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*/src/main\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/main.cpp)

# 显式加入算法源文件，避免 GLOB 未刷新导致 LNK2019（CimdrawVisibilityGraph 等）
list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimdrawVisibilityGraph\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/algorithm/CimdrawVisibilityGraph.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*TopologyGraphIndex\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/topology/TopologyGraphIndex.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimdrawWiringSymbolStyle\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/wiring/base/CimdrawWiringSymbolStyle.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimdrawAppConfig\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/CimdrawAppConfig.cpp)

# 组合所有源文件
list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimConnectivityNodeView\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/semantic/CimConnectivityNodeView.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimTransformerEndView\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/semantic/CimTransformerEndView.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimDiagramObjectView\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/semantic/CimDiagramObjectView.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimDiagramObjectPointView\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/semantic/CimDiagramObjectPointView.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimPowerTransformerView\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/semantic/CimPowerTransformerView.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimShuntCompensatorView\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/semantic/CimShuntCompensatorView.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimSeriesCompensatorView\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/semantic/CimSeriesCompensatorView.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*PowerTopologyProjection\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/topology/PowerTopologyProjection.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimGraphicSummaryText\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimGraphicSummaryText.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimGraphicRenderStateQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimGraphicRenderStateQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimGraphicBehaviorQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimGraphicBehaviorQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimBehaviorRuntimeIngest\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/behavior/CimBehaviorRuntimeIngest.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimModelDiff\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/diff/CimModelDiff.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimModelSubsetExporter\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/exporter/CimModelSubsetExporter.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimModelWriteback\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/writeback/CimModelWriteback.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimGeneratedTopologyGraphQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimGeneratedTopologyGraphQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimPowerTopologyRuntimeQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimPowerTopologyRuntimeQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimPowerTopologyPreviewQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimPowerTopologyPreviewQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimTopologyCompatQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimTopologyCompatQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimTopologySummaryQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimTopologySummaryQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimGraphicObjectSummaryQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimGraphicObjectSummaryQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimGraphicVisualSummaryQuery\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/query/CimGraphicVisualSummaryQuery.cpp)

list(FILTER PROJECT_SRC_FILES EXCLUDE REGEX ".*CimGeneratedTopologySceneBuilder\\.cpp$")
list(APPEND PROJECT_SRC_FILES ${CMAKE_SOURCE_DIR}/src/cim/ui/CimGeneratedTopologySceneBuilder.cpp)

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

option(BUILD_CIMDRAW_TESTS "Build Cimdraw QTest executables" OFF)
if(BUILD_CIMDRAW_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)

    set(CIMDRAW_TEST_SOURCES ${ALL_SRC_FILES})
    list(FILTER CIMDRAW_TEST_SOURCES EXCLUDE REGEX ".*/src/main\\.cpp$")

    set(CIMDRAW_TEST_QT_LIBS
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Xml
        Qt6::PrintSupport
        Qt6::Network
        Qt6::Test
    )

    function(add_cimdraw_qtest TARGET_NAME)
        add_executable(${TARGET_NAME}
            ${ARGN}
            ${CIMDRAW_TEST_SOURCES}
            ${HEADER_FILES}
            ${UI_FILES}
            ${RESOURCE_FILES}
        )
        set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests_bin")
        target_link_libraries(${TARGET_NAME} PRIVATE ${CIMDRAW_TEST_QT_LIBS})
        if(Qt6Core5Compat_FOUND)
            target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Core5Compat)
        endif()
        if(WIN32)
            target_compile_definitions(${TARGET_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
        endif()
    endfunction()

    add_cimdraw_qtest(cimdraw_topology_tests
        tests/test_cimdraw_topology_main.cpp
        tests/test_cimdraw_topology.cpp
        tests/test_cim_model.cpp
        tests/test_cim_importer.cpp
        tests/test_cim_ui.cpp
    )

    add_cimdraw_qtest(cimdraw_cim_model_tests
        tests/test_cim_model_main.cpp
        tests/test_cim_model.cpp
    )

    add_cimdraw_qtest(cimdraw_cimdraw_topology_tests
        tests/test_cimdraw_topology_only_main.cpp
        tests/test_cimdraw_topology.cpp
    )

    add_cimdraw_qtest(cimdraw_cim_importer_tests
        tests/test_cim_importer_main.cpp
        tests/test_cim_importer.cpp
    )

    add_cimdraw_qtest(cimdraw_cim_graphics_tests
        tests/test_cim_graphics_main.cpp
        tests/test_cim_graphics.cpp
    )

    add_cimdraw_qtest(cimdraw_cim_ui_tests
        tests/test_cim_ui_main.cpp
        tests/test_cim_ui.cpp
    )
endif()
