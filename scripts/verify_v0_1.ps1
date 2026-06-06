$ErrorActionPreference = 'Stop'

$outDir = if ($env:OUT_DIR) { $env:OUT_DIR } else { 'build/v0_1_verify' }
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$commonSources = @(
    'core/events/event_bus.cpp',
    'core/logging/simulation_logger.cpp',
    'core/state/simulation_state.cpp',
    'core/state/state_manager.cpp',
    'core/config/config_loader.cpp',
    'core/registry/system_registry.cpp',
    'core/tick/tick_scheduler.cpp',
    'systems/energy/energy_pool.cpp',
    'systems/energy/energy_system.cpp',
    'systems/information/state_memory.cpp',
    'systems/information/information_system.cpp',
    'systems/entity/entity.cpp',
    'systems/entity/behavior_model.cpp',
    'systems/entity/entity_system.cpp',
    'systems/environment/world_grid.cpp',
    'systems/environment/environment_system.cpp',
    'systems/interaction/interaction_event.cpp',
    'systems/interaction/interaction_system.cpp'
)

$appSources = @(
    'core/app/main.cpp',
    'core/app/application.cpp',
    'core/simulation/simulation_controller.cpp'
) + $commonSources

$unitSources = @('tests/unit/core_unit_tests.cpp') + $commonSources

$mvbTestSources = @(
    'tests/integration/mvb_integration_test.cpp',
    'core/simulation/simulation_controller.cpp'
) + $commonSources

$smokeSources = @(
    'tests/smoke_tests.cpp',
    'core/tick_system/tick_manager.cpp',
    'systems/energy/energy_core.cpp',
    'systems/energy/energy_flow_solver.cpp',
    'systems/information/information_core.cpp',
    'systems/entity/entity_core.cpp',
    'systems/environment/environment_core.cpp',
    'systems/interaction/interaction_core.cpp'
)

function Invoke-Step($Name, [scriptblock]$Body) {
    Write-Host "[verify] $Name"
    & $Body
    if ($LASTEXITCODE -ne 0) {
        throw "Step failed: $Name"
    }
}

function Find-VsDevCmd {
    $candidates = @(
        'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat',
        'C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat',
        'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat',
        'C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
    )

    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate) { return $candidate }
    }

    return $null
}

$explicitCxx = [bool]$env:CXX
$cxx = if ($env:CXX) { $env:CXX } else { 'g++' }
$useGcc = [bool](Get-Command $cxx -ErrorAction SilentlyContinue)
$vsDevCmd = $null

if (-not $useGcc -and -not $explicitCxx) {
    $vsDevCmd = Find-VsDevCmd
    if ($vsDevCmd) {
        Write-Host '[verify] g++ not found; using Visual Studio cl.exe via VsDevCmd.bat'
    }
}

if (-not $useGcc -and -not $vsDevCmd) {
    throw 'No supported C++17 compiler found. Install g++ or Visual Studio C++ Build Tools, or set CXX.'
}

function Invoke-Compile($Name, $Sources, $Output) {
    if ($useGcc) {
        $std = if ($env:STD) { $env:STD } else { '-std=c++17' }
        Invoke-Step $Name { & $cxx $std '-I.' @Sources '-o' $Output }
        return
    }

    $clArgs = @('/nologo', '/std:c++17', '/EHsc', '/I.') + $Sources + @("/Fe:$Output")
    $quotedArgs = ($clArgs | ForEach-Object {
        if ($_ -match '[\s&()^]') { '"' + ($_ -replace '"', '\"') + '"' } else { $_ }
    }) -join ' '
    $cmd = 'call "' + $vsDevCmd + '" -arch=x64 >nul && cl ' + $quotedArgs
    Invoke-Step $Name { cmd /c $cmd }
}

$appExe = Join-Path $outDir 'ex_origine.exe'
$unitExe = Join-Path $outDir 'core_unit_tests.exe'
$mvbExe = Join-Path $outDir 'mvb_integration_test.exe'
$smokeExe = Join-Path $outDir 'smoke_tests.exe'

Invoke-Compile 'Building MVB executable' $appSources $appExe
Invoke-Step 'Running default 10-tick MVB' { & $appExe '--ticks' '10' }
Invoke-Step 'Running 100-tick MVB closeout loop' { & $appExe '--ticks' '100' }

Write-Host '[verify] Checking missing config fails'
& $appExe '--config' 'data/configs/no_such_config.json' '--ticks' '1'
if ($LASTEXITCODE -eq 0) {
    throw 'Expected missing config to fail'
}

Invoke-Compile 'Building unit tests' $unitSources $unitExe
Invoke-Step 'Running unit tests' { & $unitExe }

Invoke-Compile 'Building MVB integration test' $mvbTestSources $mvbExe
Invoke-Step 'Running MVB integration test' { & $mvbExe }

Invoke-Compile 'Building legacy smoke tests' $smokeSources $smokeExe
Invoke-Step 'Running legacy smoke tests' { & $smokeExe }

Write-Host '[verify] v0.1 closeout checks passed'
