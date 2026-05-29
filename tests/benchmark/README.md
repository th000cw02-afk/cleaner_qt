# Benchmark 套件

对比 WinDirStat MFT 模式与 CleanerQt 扫描耗时（同机 C: SSD）。

```powershell
Measure-Command { .\public\Release\CleanerQt.exe --scan C:\ --format csv --output scan.csv --mft }
```

| 场景 | 目标 |
|------|------|
| 100 万文件索引 | All Files 虚拟列表可滚动 |
| MFT 扫描 | 接近 WinDirStat 数量级 |
| 快照 round-trip | `.cqtscan` 保存/加载一致 |
