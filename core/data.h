//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


const extern char* global_data_storage_fn;

/**
 * @brief stores all regions of interest
 *
 * Regions are defined by two coordinates and shall be ordered [X1, Y1, X2, Y2]
 *
 * @remark It is preferred that coordinates be top-left and bottom-right but that is not required.
 */
extern std::vector<cv::Vec4i> registerROI;
// TODO: there should be the ability to have regions for more than one data source


// Data Persistence Functions

bool readData();

void writeData();
