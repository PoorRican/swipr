//
// Created by Josue David Figueroa on 10/5/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//

#ifndef SWIPR_ROI_H
#define SWIPR_ROI_H

#include "opencv2/opencv.hpp"


/**
 * @brief Extracts a region from given frame
 * @param frame [in] Input frame to extract from
 * @param extracted [out] Output to put extracted frame
 * @param v [in] Region of Points to extract (tl_x, tl_y, br_x, br_y)
 */
inline void extract_frame( const cv::Mat& frame, cv::Mat& extracted, const cv::Vec4i& v ){
    extracted = frame( cv::Rect( cv::Point2i( v[0], v[1] ),
                                 cv::Point2i( v[2], v[3] ) ) );
}

#endif //SWIPR_ROI_H
