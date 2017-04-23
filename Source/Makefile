FLAGS = -lm -std=c++11
HEADERS = wav_def.h \
          pStringUtils.h \
          pFileUtils.h \
          pTextFile.h \
          pVectorUtils.h \
          pImageBytes.h \
          pImageUtils.h \
          pFastGaussianBlur.h \
          pDrawPrimitives.h \
          pWavData.h \
          pSpectrogram.h \
          pNoiseReducer.h \
          pExample.h\
          pRandomForest_pthread.h\
          pKMeansCluster.h\
          pFarthestFirstTraversal.h \
          pMetaData_Rescale.h \
          pExample_MultiLabel.h \
          pMultiLabelErrorMeasure.h \
          pBinaryRelevance.h \
          pMultiLabelEnsembleOfClassifierChains.h \
          pRandomForest_OutOfBag.h \
          p2DRFSegmentation.h \
          p2DSegmentExtraction.h \
          pMetaData.h \
          pMultiLabelClassificationExperiment.h \
          pDatasetSpecific_HJAndrews.h \
          pDatasetSpecific_MLSPCompetition.h \
          pMIMLClassDiscoveryExperiment.h \
          pParameters.o \
          pTimer.o \
          pFileIO_BMP.o \
          p2DSegmentFeatures.o \
          FFT.o \
          wav_out.o \
          wav_in.o \
          pMFCCs.o \
          pHardCodedFont.o

default: $(HEADERS) main.cpp
	g++ -pthread main.cpp -o pterodactyl pParameters.o pTimer.o pFileIO_BMP.o p2DSegmentationFeatures.o FFT.o wav_out.o wav_in.o pMFCCs.o pHardCodedFont.o $(FLAGS)

FFT.o: FFT.h FFT.cpp
	g++ -c FFT.cpp  $(FLAGS)

p2DSegmentFeatures.o: p2DSegmentFeatures.h p2DSegmentationFeatures.cpp pImageBytes.h pImageUtils.h pVectorND.h pFastGaussianBlur.h pVectorUtils.h pRectangle.h
	g++  -c p2DSegmentationFeatures.cpp $(FLAGS)

pParameters.o: pParameters.cpp pParameters.h
	g++ -c pParameters.cpp $(FLAGS)

pFileIO_BMP.o: pFileIO_BMP.h pColor.h pFileIO_BMP.cpp
	g++ -c pFileIO_BMP.cpp $(FLAGS)

pTimer.o: pTimer.h pTimer.cpp
	g++ -c Utils/pTimer.cpp $(FLAGS)

pHardCodedFont.o: pHardCodedFont.h pHardCodedFont.cpp
	g++ -c pHardCodedFont.cpp $(FLAGS)

pMFCCs.o: pMFCCs.h pMFCCs.cpp
	g++ -c pMFCCs.cpp $(FLAGS)

wav_in.o: f_err.h f_ptch.h wav_def.h wav_in.h wav_in.cpp
	g++ -c wav_in.cpp $(FLAGS)

wav_out.o: wav_out.h wav_out.cpp wav_in.o
	g++ -c wav_out.cpp $(FLAGS)

clean:
	rm -f *.o
