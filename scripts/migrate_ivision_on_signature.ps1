# Migration script: migrate IVisionEventHandler On* signatures to by-value and update call sites.
# Usage: powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\migrate_ivision_on_signature.ps1

$root = Get-Location
$timestamp = Get-Date -Format "yyyyMMddHHmmss"
$report = "migrate_report_$timestamp.txt"
Write-Output "Migration started at $(Get-Date)" | Tee-Object $report

# File extensions to process
$exts = @('*.h','*.hpp','*.hh','*.c','*.cpp','*.cc')

# Patterns
$onNames = 'SetCok|InspReady|Measure|DeviceCheck|Light'
$patternDecl = "void\s+On($onNames)\s*\(\s*const\s+ByteArray\s*&\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)"
$patternDef = "void\s+([A-Za-z0-9_:]+)::On($onNames)\s*\(\s*const\s+ByteArray\s*&\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)"

# Gather files
$files = Get-ChildItem -Recurse -Include $exts -ErrorAction SilentlyContinue | Where-Object { -not $_.PSIsContainer }
Write-Output "Found $($files.Count) source files." | Tee-Object $report -Append

foreach ($f in $files) {
    $path = $f.FullName
    $content = Get-Content -Raw -ErrorAction SilentlyContinue -Path $path
    if ($null -eq $content) { continue }
    $orig = $content
    $modified = $false

    # Backup original once per file
    $bakPath = "$path.bak_$timestamp"
    if (!(Test-Path $bakPath)) { Copy-Item -Path $path -Destination $bakPath -Force }

    # 1) Replace declarations: void OnX(const ByteArray& name) -> void OnX(ByteArray name)
    $new = [regex]::Replace($content, $patternDecl, 'void On$1(ByteArray $2)')
    if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Updated declarations in: $path" | Tee-Object $report -Append }

    # 2) Replace qualified defs: void Class::OnX(const ByteArray& name) -> void Class::OnX(ByteArray name)
    $new = [regex]::Replace($content, $patternDef, 'void $1::On$2(ByteArray $3)')
    if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Updated definitions in: $path" | Tee-Object $report -Append }

    # 3) Find rvalue-ref parameter names (&& name) in this file
    $rvarMatches = [regex]::Matches($content, '&&\s*([A-Za-z_][A-Za-z0-9_]*)')
    $rvars = @()
    foreach ($m in $rvarMatches) { $name = $m.Groups[1].Value; if ($rvars -notcontains $name) { $rvars += $name } }

    if ($rvars.Count -gt 0) {
        # For each rvalue var name, replace OnX(var) -> OnX(std::move(var))
        foreach ($v in $rvars) {
            # build pattern to match OnX(var) with optional this-> prefix
            $pat = "(?<prefix>\b(?:this->)? )?On($onNames)\s*\(\s*" + [regex]::Escape($v) + "\s*\)"
            # But regex with optional this-> spacing is tricky; simpler two replacements:
            $new = [regex]::Replace($content, "this->On($onNames)\s*\(\s*" + [regex]::Escape($v) + "\s*\)", 'this->On$1(std::move(' + $v + '))')
            if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Replaced this->OnX($v) -> move in $path" | Tee-Object $report -Append }

            $new = [regex]::Replace($content, "\bOn($onNames)\s*\(\s*" + [regex]::Escape($v) + "\s*\)", 'On$1(std::move(' + $v + '))')
            if ($new -ne $content) { $content = $new; $modified = $true; Write-Output "Replaced OnX($v) -> move in $path" | Tee-Object $report -Append }
        }
    }

    # If changed, write back
    if ($modified) {
        Set-Content -Path $path -Value $content -Encoding UTF8
    }
}

Write-Output "Migration finished at $(Get-Date). See report: $report" | Tee-Object $report -Append
exit 0
