//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include <opencv2/opencv.hpp>
#include <vector>


#ifndef SWIPR_ROI_H
#define SWIPR_ROI_H

#endif //SWIPR_ROI_H


// ROI memory

/**
 * @brief stores all regions of interest
 *
 * Regions are defined by two coordinates and shall be ordered [X1, Y1, X2, Y2]
 *
 * @remark It is preferred that coordinates be top-left and bottom-right but that is not required.
 */
std::vector<cv::Vec4i> registerROI;
