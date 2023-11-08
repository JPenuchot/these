#! /bin/sh -e

WORKDIR=$(pwd)

mkdir -p "$WORKDIR/images"

# sudo pyperf system tune

echo "Running BF benchmarks"
cd "$WORKDIR/poacher/brainfog"
cmake --preset bench

cmake --build --preset bench -j1 --target bfbench-consecutive_loops
cp  "build/bench/bfbench-consecutive_loops/ExecuteCompiler.png" \
    "$WORKDIR/images/bf-consecutive_loops.png"

cmake --build --preset bench -j1 --target bfbench-imbricated_loops
cp  "build/bench/bfbench-imbricated_loops/ExecuteCompiler.png" \
    "$WORKDIR/images/bf-imbricated_loops.png"

echo "Running Shunting yard benchmarks"
cd "$WORKDIR/poacher/shunting-yard"
cmake --preset release

cmake --build --preset release -j1 --target shunting-yard.addition-series.graph
cp  "build/release/shunting-yard.addition-series.graph/ExecuteCompiler.png" \
    "$WORKDIR/images/shunting-yard.addition-series.graph.png"
