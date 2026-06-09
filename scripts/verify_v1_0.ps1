$ErrorActionPreference = 'Stop'
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
if (-not $env:OUT_DIR) { $env:OUT_DIR = 'build/v1_0_verify' }
& (Join-Path $scriptDir 'verify_v0_1.ps1')
