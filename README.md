# cleaner_qt — 磁盘空间分析 v2.0

Windows 专精 Qt 6 磁盘空间分析工具，对齐 QDirStat / WinDirStat / DiskPilot / Scour 开源能力全集。

## 功能

- **MFT 快速扫描**（NTFS + 管理员）与并行目录扫描自动回退
- 目录树 + Squarified Treemap（扩展名着色、文件级块、钻取）
- **全部文件** 扁平视图（`FileIndexModel` 虚拟滚动）
- Logical / Allocated 大小、硬链接 registry
- 扫描快照 `.cqtscan` 保存/加载
- 重复文件专页、正则搜索、多选批量删除
- **CleanupHub**：12 类 Scour 风格清理扫描器
- 深色/浅色主题、设置对话框（排除规则、MFT、便携模式）
- 扫描速率 GB/s 显示、UAC 提升重启
- CLI：`CleanerQt.exe --scan C:\ --format csv --output r.csv --mft`
- 目录监控（FileWatcher 占位）、Sunburst 备选 Tab

## 竞品参考

| 项目 | 借鉴点 |
|------|--------|
| [QDirStat](https://github.com/shundhammer/qdirstat) | 树 + Treemap、扩展名图例、自定义清理命令 |
| [WinDirStat](https://github.com/windirstat/windirstat) | 多视图、快照、硬链接、CLI |
| [DiskPilot](https://github.com/mhkasif/DiskPilot) | 虚拟滚动、allocated/logical、列持久化 |
| [Scour](https://github.com/SysAdminDoc/Scour) | 12 类清理扫描器 |
| [DiskClarity](https://github.com/Ezeny1337/DiskClarity) | MFT 性能线 |

## 环境要求

- Windows 10/11
- Qt 6.5+（Core, Quick, Qml, Widgets, Concurrent, Network）
- CMake 3.16+
- MSVC 2022 或 MinGW 64-bit

## 构建

```powershell
cd build
cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2022_64"
cmake --build . --config Release
cmake --build . --target deploy-release --config Release
```

或一键脚本：

```powershell
.\scripts\deploy.ps1
```

`windeployqt` 会把 `Qt6Widgets.dll`、`Qt6Quick.dll`、`platforms\` 等复制到 exe 同目录。**不要只拷贝 exe**，整个 `public/release` 文件夹需一起分发。

输出：`public/Release/CleanerQt.exe`（与 DLL 同目录）

## CLI

```powershell
CleanerQt.exe --scan D:\ --output scan.csv --format csv --mft
CleanerQt.exe --import-csv scan.csv
```

## 快捷键

| 键 | 动作 |
|----|------|
| F5 | 开始扫描 |
| Ctrl+S | 保存扫描快照 |
| Ctrl+F | 聚焦搜索 |

## 日志

基于 [spdlog](https://github.com/gabime/spdlog)（可选）：将源码置于 `third_party/spdlog`，或配置时加 `-DCLEANER_QT_FETCH_SPDLOG=ON` 自动下载。未找到 spdlog 时使用内置文件日志。输出：控制台 + 滚动文件（默认 `logs/CleanerQt.log`，单文件 5MB，保留 3 个）。

- GUI 与 QML 警告经 Qt 消息处理器写入同一日志
- CLI：`CleanerQt.exe --scan D:\ --verbose` 开启 debug 级别
- 设置项（`CleanerQt.ini` / 注册表）：`logLevel`（`trace|debug|info|warn|error`）、`logToFile`（默认 true）

## 测试

单元测试与集成测试（Qt Test + CTest），覆盖目录树、Treemap、TopFiles、ThemeManager、ScanModuleRegistry、日志、FormatUtils、硬链接、快照、导出、ScanWorker、CLI 等（约 20 个用例）。

一键运行：

```powershell
.\scripts\test.ps1
```

或手动：

```powershell
$env:PATH = "C:\Qt\6.8.0\msvc2022_64\bin;" + $env:PATH
cmake --build build --config Release
cd build
ctest -C Release --output-on-failure
```

关闭测试构建：`cmake -DCLEANER_QT_BUILD_TESTS=OFF ..`

Benchmark 说明见 `tests/benchmark/README.md`。

## 许可

GPL-3.0 — 见 [LICENSE](LICENSE)
