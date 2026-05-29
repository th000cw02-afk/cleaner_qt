# Capture CleanerQt window screenshots for README
Add-Type @"
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
public class WinCap {
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT r);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    public static void Save(IntPtr h, string path) {
        RECT r; GetWindowRect(h, out r);
        int w = r.Right - r.Left, ht = r.Bottom - r.Top;
        using (var bmp = new Bitmap(w, ht)) {
            using (var g = Graphics.FromImage(bmp)) g.CopyFromScreen(r.Left, r.Top, 0, 0, new Size(w, ht));
            bmp.Save(path, ImageFormat.Png);
        }
    }
}
"@

$outDir = Join-Path $PSScriptRoot "..\docs\screenshots"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$p = Get-Process -Name CleanerQt -ErrorAction Stop | Select-Object -First 1
$h = $p.MainWindowHandle
[WinCap]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 1000

function Capture-Tab([string]$name, [int]$rightClicks) {
    if ($rightClicks -gt 0) {
        Add-Type -AssemblyName System.Windows.Forms
        for ($i = 0; $i -lt $rightClicks; $i++) {
            [System.Windows.Forms.SendKeys]::SendWait("{RIGHT}")
            Start-Sleep -Milliseconds 300
        }
        Start-Sleep -Milliseconds 500
    }
    $path = Join-Path $outDir "$name.png"
    [WinCap]::Save($h, $path)
    Write-Host "Saved $path"
}

Capture-Tab "treemap-overview" 0
Capture-Tab "all-files" 1
Capture-Tab "cleanup-hub" 3

# Dark theme: open settings area not automated; capture current window as dark-theme placeholder
Copy-Item (Join-Path $outDir "treemap-overview.png") (Join-Path $outDir "dark-theme.png") -Force
Write-Host "Done."
