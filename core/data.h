//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


const extern char* g_data_storage_fn;       // filename for global data


// Regions of Interest

/**
 * @brief Struct for storing regions of interest for a given video source (eg: camera stream)
 */
struct source_filter_t {
    // TODO: add date last updated
    /**
     * @brief stores the identifier of the source
     *
     * This is the id used when calibrating regions on new sources.
     * It is set by `calibrateFromVideo` but can be changed via the `source` argument.
     */
    std::string source = "";
    /**
     * @brief stores all regions of interest
     *
     * Regions are defined by two coordinates and shall be ordered [X1, Y1, X2, Y2]
     *
     * @remark It is preferred that coordinates be top-left and bottom-right but that is not required.
     */
    std::vector<cv::Vec4i> regions;
    // TODO: there needs to be a way to id these regions
};


extern std::vector<source_filter_t> g_filters;


// Data Persistence Functions


bool readData();

void writeData();
