#!/bin/bash

# Assume the RF classifier has already been trained

# Generate spectrograms
./pterodactyl -spectrogram+noisefilter_multithread src_wavs spectrograms filtered_spectrograms >/dev/null

# Segment spectrograms
#./pterodactyl -2drfseg_run_multithread filtered_spectrograms

COUNT=`ls segmentation_mask/ | wc`
#echo "Produced $COUNT segmentation masks"

# TODO: Compute features with -analyze_segments

# TODO: Run a classifier on the features to output species labels

cat example_output.json
