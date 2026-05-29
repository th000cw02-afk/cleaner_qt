# Run after `gh auth login` to update GitHub repository metadata for discoverability.
$ErrorActionPreference = "Stop"

gh repo edit th000cw02-afk/cleaner_qt `
  --description "Fast Windows disk analyzer — NTFS MFT scan, Qt 6 treemap UI, cleanup hub & CLI" `
  --homepage "https://github.com/th000cw02-afk/cleaner_qt#readme"

gh repo edit th000cw02-afk/cleaner_qt `
  --add-topic disk-usage `
  --add-topic disk-analyzer `
  --add-topic windirstat `
  --add-topic qt6 `
  --add-topic qml `
  --add-topic windows `
  --add-topic ntfs `
  --add-topic mft `
  --add-topic treemap `
  --add-topic disk-cleanup `
  --add-topic cpp

Write-Host "Repository description and topics updated."
