// Copyright (C) 2013 Forrest Briggs
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// many of the tools in this package write meta-data such as segment features, rectangles in a spectrogram, labels, etc.,
// to text files. this file provides functions for parsing those files to re-load the saved meta-data.
 
#pragma once

#include "pTextFile.h"
#include "pSignalUtils.h"
#include "pRectangle.h"
#include <string>
#include <vector>
using namespace std;

class pSegment_MetaData
{
public:
	int bag_id_;
	int segment_id_;
	
	int clusterAssignment_; //  the index of the cluster this segment belongs to, in some clustering of the data. this may be loaded from segment2cluster.txt (which is generated by -cluster_segments).
	
	pRectangle rect_;
	vector<float> featureVector_;
	
	pSegment_MetaData(int bag_id, int segment_id, pRectangle rect, vector<float>& fv) : bag_id_(bag_id), segment_id_(segment_id), rect_(rect), featureVector_(fv) {}
};

class pAudioFile_MetaData
{
public:
	int bag_id_;
	int crossValidationFold_;				// for k-fold cross-validation, this is in 0, 1, ..., k-1, and indexes which fold the bag/recording is in
	string filename_;
	vector<pSegment_MetaData> segments_;	// this will be filled in after the object is created
	vector<float> bagLevelFeatureVector_;	// in some case, an audio file (rather than a segment) is described by a feature vector
	vector<int> speciesList_;				// list of all species present in the recording (or other classes)
	float rainProbability_;					// probability of rain estimated by the rain classifier
	
	pAudioFile_MetaData(int bag_id, string filename) : bag_id_(bag_id), filename_(filename) {}
};

class pMetaData_Parser
{
public:
	// loads a value for each instance clusterAssignment_ from a file such as segment2cluster.txt
	static void parseInstanceClusterAssignment(string filename, vector<pAudioFile_MetaData>& _afmd)
	{
		cout << "parsing cluster assignments from " << filename << endl;
		
		pTextFile f(filename, PFILE_READ);
		assert(f.readLine() == "bag_id,segment_id,cluster_assignment");
		
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::split(f.readLine(), ",");
			assert(parts.size() == 3);
			
			int bag_id = pStringUtils::stringToInt(parts[0]);
			int segment_id = pStringUtils::stringToInt(parts[1]);
			assert( bag_id >= 0 && bag_id < _afmd.size() ); 
			
			_afmd[bag_id].segments_[segment_id].clusterAssignment_ = pStringUtils::stringToInt(parts[2]);
		}

		
		f.close();
	}

	// parses species_list.txt to get 4-letter species codes and species names
	static void parseSpeciesList(string filename, vector<string>& _speciesCodes, vector<string>& _speciesNames)
	{
		cout << "parsing species list from " << filename << endl;
		
		pTextFile f(filename, PFILE_READ);
		assert(f.readLine() == "class_id,code,species");
		
		int i = 0;
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::split(f.readLine(), ",");
			assert(parts.size() == 3);
			
			int bag_id = pStringUtils::stringToInt(parts[0]);
			assert(bag_id == i && "species codes must be listed in ascending order, starting with 0");
			
			string& code = parts[1];
			string& species = parts[2];
			
			_speciesCodes.push_back(code);
			_speciesNames.push_back(species);
			
			++i;
		}
		
		f.close();
	}

	// this function is itended to read the file bag_id2filename.txt which is generated by processSegments(),
	// but might be used for a similarly structured file with a different name. results are written to _afmd.
	// this function should be called before any other parsers to setup meta-data datastructures.
	static void parseBags(string filename, vector<pAudioFile_MetaData>& _afmd)
	{
		cout << "parsing bags from " << filename << endl;
		
		pTextFile f(filename, PFILE_READ);
		assert(f.readLine() == "bag_id,filename");
		
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::split(f.readLine(), ",");
			assert(parts.size() == 2);
			int bag_id = pStringUtils::stringToInt(parts[0]);
			string filename = parts[1];
			
			_afmd.push_back(pAudioFile_MetaData(bag_id, filename));
			
			assert(_afmd[_afmd.size()-1].bag_id_ == _afmd.size()-1); // verify that all the bag-meta data is in the correct order
		}
		
		f.close();
	}  
	
	// read the file rain_probabilities.txt generated by the rain classifier
	static void parseRainProbabilities(vector<pAudioFile_MetaData>& _afmd)
	{
		cout << "parsing rain probabilities" << endl;
		
		pTextFile f("rain_probabilities.txt", PFILE_READ);
		assert(f.readLine() == "bag_id,rain_probability");
		
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::split(f.readLine(), ",");
			assert(parts.size() == 2);
			int bag_id = pStringUtils::stringToInt(parts[0]);
			assert(bag_id >= 0 && bag_id < _afmd.size());
			_afmd[bag_id].rainProbability_ = pStringUtils::stringToFloat(parts[1]);;
		}
		
		f.close();
	}
	
	// parses label sets for recordings/bags, e.g., bag_labels.txt
	static void parseBagLabelSets(string filename, vector<pAudioFile_MetaData>& _afmd)
	{
		cout << "parsing bag label sets from " << filename << endl;
		
		pTextFile f(filename, PFILE_READ);
		assert(f.readLine() == "bag_id,[labels]");
		
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::splitNonEmpty(f.readLine(), ",");
                           if (parts.size() == 0) {
                               break;
                           }

			assert(parts.size() >= 1); // smallest case- no labels for bag -> 1 part for the bag_id
			
			int bag_id = pStringUtils::stringToInt(parts[0]);
                           cout << " Parsing bag id " << bag_id << " out of " << _afmd.size() << endl;
			assert(bag_id >= 0 && bag_id < _afmd.size());
			
			vector<int> labelSet;
			for(int i = 1; i < parts.size(); ++i)
				labelSet.push_back(pStringUtils::stringToInt(parts[i]));
			
			_afmd[bag_id].speciesList_ = labelSet;
		}
		
		f.close();
	}
	
	// this function is indended to read histogram_of_segments.txt or a similar file which defines a feature vector for each bag
	static void parseBagLevelFeatures(string filename, vector<pAudioFile_MetaData>& _afmd)
	{
		cout << "parsing bag-level feature vectors from " << filename << endl;
		
		pTextFile f(filename, PFILE_READ);
		string header = f.readLine();
		// warning: header is not checked for exact match against histogram_of_segments.txt header so that this function can be used more generically
		
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::split(f.readLine(), ",");
			assert(parts.size() >= 2);
			
			int bag_id = pStringUtils::stringToInt(parts[0]);
			assert(bag_id >= 0 && bag_id < _afmd.size());
			
			vector<float> fv;
			for(int i = 1; i < parts.size(); ++i)
				fv.push_back(pStringUtils::stringToFloat(parts[i]));
			
			_afmd[bag_id].bagLevelFeatureVector_ = fv;
		}
		
		f.close();
	}
	
	// this function is intended to read the file segment_features.txt, and to be called after parseBags
	static void parseSegmentFeatures(string filename, vector<pAudioFile_MetaData>& _afmd)
	{
		cout << "parsing segment features from " << filename << endl;
		
		pTextFile f(filename, PFILE_READ);
		assert(f.readLine() == "bag_id,segment_id,[feature vector]");
		
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::split(f.readLine(), ",");
			assert(parts.size() >= 2);
			
			int bag_id = pStringUtils::stringToInt(parts[0]);
			int segment_id = pStringUtils::stringToInt(parts[1]);
			assert( bag_id >= 0 && bag_id < _afmd.size() ); 
			
			vector<float> fv;
			for(int i = 2; i < parts.size(); ++i)
				fv.push_back(pStringUtils::stringToFloat(parts[i]));
				
			_afmd[bag_id].segments_.push_back(pSegment_MetaData(bag_id, segment_id, pRectangle(), fv)); // for now we don't have the rectangle. that can be parsed later
		}

		f.close();
	}
	
	// returns the segment feature vector dimension, given the full dataset meta-data.
	// this is more complicated than it might sound, because the first file might not have any segments, 
	// so we need to look for a file that has segments and inspect those. this is slow. don't call it lots of times.
	// instead cache the result.
	static int getSegmentFeatureDim(vector<pAudioFile_MetaData>& afmd)
	{
		for(int i = 0; i < afmd.size(); ++i)
		{
			if( afmd[i].segments_.size() > 0 )
				return afmd[i].segments_[0].featureVector_.size();
		}
		
		assert(false && "tried to do something requiring segments, but there are none");
	}
	
	// this function parses a file which assigns each bag/recording to a fold for k-fold cross-validation
	static void parseCrossValidationFolds(string filename, vector<pAudioFile_MetaData>& _afmd)
	{
		pTextFile f(filename, PFILE_READ);
                  cout << "parsing cross-validation folds from " << filename << endl;
		assert(f.readLine() == "bag_id,fold");
		
		while(!f.eof())
		{
			vector<string> parts = pStringUtils::split(f.readLine(), ",");
			assert(parts.size() == 2);
			
			int bag_id = pStringUtils::stringToInt(parts[0]);
			assert(bag_id >= 0 && bag_id < _afmd.size());

			_afmd[bag_id].crossValidationFold_ = pStringUtils::stringToInt(parts[1]);
		}

		f.close();
	}
};
