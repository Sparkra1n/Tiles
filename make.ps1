param (
    [string]$action
)

switch ($action) {
    "clean" {
        Remove-Item -Recurse -Force ./build
        New-Item -ItemType Directory -Path ./build
        Write-Host "Build directory cleaned and recreated."
    }
    "build" {
        cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
        cmake --build build
        Write-Host "Build completed."
    }
    default {
        Write-Host "Usage: .\make.ps1 {clean|build}"
    }
}
