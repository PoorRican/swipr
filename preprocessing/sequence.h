//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#ifndef SWIPR_SEQUENCE_H
#define SWIPR_SEQUENCE_H

#include "opencv2/opencv.hpp"
#include "string"
#include "vector"

#include "../core/data.h"


enum sequenceUI_state {
    SEEKING,    /// when seeking for start or end frames
    INSIDE,     /// when start frame has been found, but still seeking for end frame
    DONE        /// when there are no more sequences to extract
};


/**
 * @brief Populates a vector of video frames from a given action_sequence_t
 *
 * @param video_fn [in] video file to open
 * @param sequence [in] action_sequence_t to fetch
 * @param frames [out] vector of cv::Mat to populate
 */
void get_sequence( const std::string& video_fn, const action_sequence_t& sequence, std::vector<cv::Mat>& frames );


/**
 * @brief Manually extract `action_sequence_t` for a given video file and source_id
 *
 * @param video_fn [in] video file to open
 * @param source_id [in] source id string of `source_filter_t`
 * @param region_id [in] region index in `source_filter_t` to extract sequences from
 * @param start_frame [in] frame to start at. This can be the end of the last sequence of the same source_id and fn.
 */
void labelSequencesFromVideo( const std::string& video_fn, const std::string& source_id, const int& region_id,
                              const double& start_frame = 0 );

#endif //SWIPR_SEQUENCE_H
