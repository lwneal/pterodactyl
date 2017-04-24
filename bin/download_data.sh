#!/bin/bash
set -e

# Download and extract the data
wget -nc lwneal.com/hja_sarah_664.tar.gz
tar xzvf hja_sarah_664.tar.gz --skip-old-files
#
wget -nc lwneal.com/hja_lawrence_625.tar.gz
tar xzvf hja_lawrence_625.tar.gz -C hja_lawrence_625 --skip-old-files


# Generate spectrograms
cp hja_lawrence_625/setA/wavs/*.wav src_wavs
cp hja_lawrence_625/setB/wavs/*.wav src_wavs
./pterodactyl -spectrogram+noisefilter_multithread src_wavs spectrograms filtered_spectrograms


# Convert labels to a format acceptable by Pterodactyl
for dir in setA setB; do
    echo "Converting BMP labels from directory $dir"
    for fn in `ls hja_lawrence_625/$dir/labels/`; do
        convert hja_lawrence_625/$dir/labels/$fn -depth 24 -resize 1871x256! BMP3:segmentation_examples/$fn
    done
done

# Train the RF classifier
./pterodactyl -2drfseg_train filtered_spectrograms segmentation_examples
