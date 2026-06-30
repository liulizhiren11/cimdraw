# cimdraw

`cimdraw` 是一个基于 Qt6 / C++17 的电力一次接线图绘制与分析工程，当前窗口标题为 `CIMDraw`。

仓库当前只保留电力工程相关内容，历史上无关的子目录和占位功能已经移除。

## 当前能力

1. 基于 `QGraphicsView` / `QGraphicsScene` 的画布与图元编辑。
2. 电力一次系统图元与工具箱。
3. 拓扑节点、连接关系与场景级拓扑索引。
4. 开关状态参与的导通、供电路径、接地路径分析。
5. 基于 Qt Property Browser 的属性编辑。
6. `lz_data.json` 运行态数据源接入能力。

## 目录结构

| 路径 | 说明 |
|------|------|
| `CMakeLists.txt` | CMake 构建入口 |
| `CMakePresets.json` | CMake 预设 |
| `src/` | 主源码目录 |
| `src/wiring/base/` | 接线图公共基础能力 |
| `src/wiring/power/` | 电力工程图元与工具 |
| `src/topology/` | 拓扑相关实现 |
| `src/datamodule/` | 数据源与运行态数据接入 |
| `src/qtpropertybrowser/` | Qt Property Browser 源码 |
| `config/lz_data.example.json` | 数据源配置示例 |
| `tests/` | QTest 测试 |
| `docs/` | 电力工程文档 |

## 构建

环境要求：

1. CMake 3.16 或更高
2. C++17 编译器
3. Qt 6
4. Windows 下推荐 MSVC 2019 x64

示例命令：

```powershell
cmake -S . -B out/build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/Qt/Qt6.5.3/6.5.3/msvc2019_64
cmake --build out/build/debug --target cimdraw
```

也可以使用预设：

```powershell
cmake --preset Qt-Debug
cmake --build out/build/debug --target cimdraw
```

## 运行

构建产物默认输出到上级 `exe` 目录，例如：

```powershell
..\exe\bin64\cimdraw.exe
```

如需启用运行态数据源，可参考：

```text
config/lz_data.example.json
```

## 测试

默认不构建测试。启用方式：

```powershell
cmake -S . -B out/build/debug-tests -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/Qt/Qt6.5.3/6.5.3/msvc2019_64 -DBUILD_CIMDRAW_TESTS=ON
cmake --build out/build/debug-tests --target cimdraw_topology_tests
.\out\build\debug-tests\tests_bin\cimdraw_topology_tests.exe
```

## 文档入口

1. [docs/README.md](docs/README.md)
2. [docs/当前状态.md](docs/当前状态.md)
3. [docs/design/README.md](docs/design/README.md)
