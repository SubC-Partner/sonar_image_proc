// Copyright 2021 University of Washington Applied Physics Laboratory
//
// This file contains the class-based API (which is more efficient)
// as it can store and reuse intermediate results.
//
// See "DrawSonar.h" for the function-based API

#pragma once

#include <memory>

#include <opencv2/core/core.hpp>

#include "sonar_image_proc/ColorMaps.h"
#include "sonar_image_proc/AbstractSonarInterface.h"

namespace sonar_image_proc {


class SonarDrawer {
 public:
    SonarDrawer();

    // Calls drawRectSonarImage followed by remapRectSonarImage inline
    // The intermediate rectangular image is not returned, if required,
    // use the two functions individually...
    cv::Mat drawSonar(const sonar_image_proc::AbstractSonarInterface &ping,
                        const SonarColorMap &colorMap = InfernoColorMap(),
                        const cv::Mat &image = cv::Mat(0, 0, 
                        CV_8UC3),
                        bool addOverlay = false);

    // Maps the sonar ping to an RGB image.
    // rectImage is reshaped to be numRanges rows x numBearings columns
    //
    // If rectImage is either 8UC3 or 32FC3, it retains that type, otherwise
    // rectImage is converted to 8UC3
    //
    // Cell (0,0) is the color mapping of the data with the smallest range and
    // smallest (typically, most negative) bearing in the ping.
    //
    // Cell (nRange,0) is the data at the max range, most negative bearing
    //
    // Cell (nRange,nBearing) is the data at the max range, most positive
    // bearing
    //
    cv::Mat drawRectSonarImage(const sonar_image_proc::AbstractSonarInterface &ping,
                        const SonarColorMap &colorMap = InfernoColorMap(),
                        const cv::Mat &rectImage = cv::Mat(0,0,CV_8UC3));

    cv::Mat remapRectSonarImage(const sonar_image_proc::AbstractSonarInterface &ping,
                                const cv::Mat &rectImage);

    cv::Mat drawOverlay(const sonar_image_proc::AbstractSonarInterface &ping,
                                const cv::Mat &sonarImage);


 private:
   // Utility class which can generate and store the two cv::Mats 
   // required for the cv::remap() function
   //
   // Also stores meta-information to determine if the map is
   // invalid and needs to be regenerated.
    struct CachedMap {
     public:
        CachedMap()
            {;}

        typedef std::pair<cv::Mat, cv::Mat> MapPair;

        MapPair operator()(const sonar_image_proc::AbstractSonarInterface &ping);

     private:
        void create(const sonar_image_proc::AbstractSonarInterface &ping);
        bool isValid(const sonar_image_proc::AbstractSonarInterface &ping) const;

        cv::Mat _scMap1, _scMap2;

        // Meta-information to validate map
        std::pair<float, float> _rangeBounds, _azimuthBounds;
        int _numRanges, _numAzimuth;
    } _map;
};

}  // namespace sonar_image_proc
