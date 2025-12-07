#!/bin/sh
# Start the simulated IVI and cluster programs (build required)
echo "Starting simulated IVI and Cluster (run in separate terminals for stdout)."
./linux-domain/ivi_main
./qnx-domain/cluster_main
