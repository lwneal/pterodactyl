#!/bin/bash

which gm || sudo apt-get install graphicsmagick

pushd bin
./train.sh
./classify.sh
popd
