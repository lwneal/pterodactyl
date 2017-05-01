#!/bin/bash
set -e

#
./pterodactyl -analyze_segments

# Compute features from each segment
./pterodactyl -histogram_of_segments
