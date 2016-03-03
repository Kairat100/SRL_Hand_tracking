#ifndef HANDDETECTOR_HPP_
#define HANDDETECTOR_HPP_

#include <stdio.h>
#include <boost/filesystem.hpp>
#include "PartsBasedDetector.hpp"
#include "Candidate.hpp"
#include "FileStorageModel.hpp"
#include "MatlabIOModel.hpp"
#include "Visualize.hpp"
#include "types.hpp"
#include "nms.hpp"
#include "Rect3.hpp"
#include "DistanceTransform.hpp"

class HandDetector 
{
private:
	PartsBasedDetector<float> pbd;
	boost::scoped_ptr<Model> model;	

public:
	HandDetector(char* model_path) 
	{
		//printf("model path - %s\n", model_path);

		// determine the type of model to read
		string ext = boost::filesystem::path(model_path).extension().string();
		if (ext.compare(".xml") == 0 || ext.compare(".yaml") == 0) 
		{
			model.reset(new FileStorageModel);
		}
		else if (ext.compare(".mat") == 0) 
		{
			model.reset(new MatlabIOModel);
		}
		else 
		{
			printf("Unsupported model format: %s\n", ext.c_str());
			exit(-2);
		}

		bool ok = model->deserialize(model_path);
		if (!ok) 
		{
			printf("Error deserializing file\n");
			exit(-3);
		}

		// create the PartsBasedDetector and distribute the model parameters
		
		pbd.distributeModel(*model);

	}

	void detectPerson(Mat frame)
	{
		// load the image from file
		Mat_<float> depth;
	
        if (frame.empty()) 
        {
            printf("Frame has invalid image format\n");
            exit(-4);
        }
		
		// detect potential candidates in the image
		vector<Candidate> candidates;
		pbd.detect(frame, depth, candidates);
		printf("Number of candidates: %ld\n", candidates.size());

		// display the best candidates
		Visualize visualize(model->name());
		SearchSpacePruning<float> ssp;
	    Mat canvas;
		
		if (candidates.size() > 0) 
		{
		    Candidate::sort(candidates);
		    //Candidate::nonMaximaSuppression(frame, candidates, 0.2);
		    //visualize.candidates(frame, candidates, canvas, true);
		    visualize.candidates(frame, candidates, 1, canvas, true);
	        visualize.image(canvas);
		    waitKey();
		}
	}
};

#endif /* HANDDETECTOR_HPP_ */