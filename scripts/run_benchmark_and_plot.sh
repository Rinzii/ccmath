#!/bin/bash

SCRIPTS_DIR="$( cd "$( dirname "$0" )" && pwd )"
ROOT_DIR="$( cd "${SCRIPTS_DIR}/.." && pwd )"

# Check that root directory is set as ccmath
if [ "$(basename "${ROOT_DIR}")" != "ccmath" ]; then
    echo "Please run this script from the scripts folder inside of the ccmath root directory"
    exit 1
fi
echo "Preparing ccmath for benchmarking and plot generation..."

#if out-plot directory exists, remove it
if [ -d "${ROOT_DIR}/out-plot" ]; then
  echo "Removing existing out-plot directory..."
  rm -rf "${ROOT_DIR}/out-plot"
fi

cd "${ROOT_DIR}" || exit 1
cmake -S . --preset=default -B out-plot -DCCMATH_BUILD_BENCHMARKS:BOOL=ON -DCCMATH_BUILD_TESTS:BOOL=OFF -DCCMATH_BUILD_EXAMPLES:BOOL=OFF
cmake --build out-plot --config=Release


# Store the generated benchmark directory
BENCHMARK_EXE="${ROOT_DIR}/out-plot/benchmark/Release/ccmath-benchmark"

# Move into the benchmark build directory
cd "${ROOT_DIR}/out-plot/benchmark/Release" || exit 1

# Generate the benchmark csv file
${BENCHMARK_EXE} --benchmark_format=csv > benchmark.csv

echo "Creating graph now!"
# Pass the csv file to the plot script to generate our graph
python3 "${SCRIPTS_DIR}/plot.py" -f benchmark.csv

# Run the benchmark and plot
