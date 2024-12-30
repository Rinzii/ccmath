@echo off
set SCRIPTS_DIR=%~dp0
cd /d "%SCRIPTS_DIR%.."
set ROOT_DIR=%CD%

rem Check that root directory is set as ccmath
for %%I in ("%ROOT_DIR%") do set BASENAME=%%~nI
if not "%BASENAME%"=="ccmath" (
    echo Please run this script from the scripts folder inside of the ccmath root directory
    exit /b 1
)

echo Preparing ccmath for benchmarking and plot generation...

rem if out-plot directory exists, remove it
if exist "%ROOT_DIR%\out-plot" (
    echo Removing existing out-plot directory...
    rmdir /s /q "%ROOT_DIR%\out-plot"
)

cd /d "%ROOT_DIR%" || exit /b 1
cmake -S . --preset=default -B out-plot -DCCMATH_BUILD_BENCHMARKS:BOOL=ON -DCCMATH_BUILD_TESTS:BOOL=OFF -DCCMATH_BUILD_EXAMPLES:BOOL=OFF
cmake --build out-plot --config Release

rem Store the generated benchmark directory
set "BENCHMARK_EXE=%ROOT_DIR%\out-plot\benchmark\Release\ccmath-benchmark"

rem Move into the benchmark build directory
cd /d "%ROOT_DIR%\out-plot\benchmark\Release" || exit /b 1

rem Generate the benchmark csv file
%BENCHMARK_EXE% --benchmark_format=csv > benchmark.csv

echo Creating graph now!
rem Pass the csv file to the plot script to generate our graph
python "%SCRIPTS_DIR%\plot.py" -f benchmark.csv
