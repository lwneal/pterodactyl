#!/bin/bash
set -e

# Assume the RF classifier has already been trained

# Clean out all directories
./clean.sh

# Generate spectrograms
./pterodactyl -spectrogram+noisefilter_multithread src_wavs spectrograms filtered_spectrograms

# Segment spectrograms
./pterodactyl -2drfseg_run_multithread filtered_spectrograms

# Compute features from each segment
./pterodactyl -histogram_of_segments

# Train and test a classifier with 2-fold cross-validation and report results
./pterodactyl -export_mlc_experiment mlc_experiments/hja20092010histosegments_mlc.txt 2
