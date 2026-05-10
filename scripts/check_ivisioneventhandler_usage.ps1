# Script: check_ivisioneventhandler_usage.ps1
# Purpose: scan the repository for usages/implementations of IVisionEventHandler On* methods
# Outputs matches to stdout and to scripts/ivision_report.txt

$reportPath = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Definition) "ivision_report.txt"
if (Test-Path $reportPath) { Remove-Item $reportPath -Force }
Write-Output "Scanning repository for IVisionEventHandler On* usages..." | Tee-Object -FilePath $reportPath -Append

$patterns = @(
    'OnSetCok(',
    'OnInspReady(',
    'OnMeasure(',
    'OnDeviceCheck(',
    'OnLight('
)

# Prefer using git grep if available for speed and correctness; fall back to Select-String
$gitExists = (Get-Command git -ErrorAction SilentlyContinue) -ne $null

foreach ($p in $patterns) {
    Write-Output "\n--- Pattern: $p ---" | Tee-Object -FilePath $reportPath -Append
    if ($gitExists) {
        $res = git grep -n -- "$p" 2>$null
        if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($res)) {
            Write-Output "(no matches)" | Tee-Object -FilePath $reportPath -Append
        } else {
            $res | Tee-Object -FilePath $reportPath -Append
        }
    } else {
        # Use Select-String recursively for common source extensions
        $matches = Get-ChildItem -Recurse -Include *.h,*.hpp,*.cpp,*.c,*.cc -ErrorAction SilentlyContinue | Select-String -Pattern $p -SimpleMatch
        if ($matches.Count -eq 0) {
            Write-Output "(no matches)" | Tee-Object -FilePath $reportPath -Append
        } else {
            foreach ($m in $matches) {
                $line = "{0}:{1}: {2}" -f $m.Path, $m.LineNumber, $m.Line.Trim()
                Write-Output $line | Tee-Object -FilePath $reportPath -Append
            }
        }
    }
}

Write-Output "\nScan complete. Report saved to: $reportPath" | Tee-Object -FilePath $reportPath -Append

# Exit code 0
exit 0
