//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


#ifndef SWIPR_DATA_H
#define SWIPR_DATA_H


const extern char* g_data_storage_fn;       // filename for global data


// Regions of Interest

// Region of Interest Functionality
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


/**
 * @brief Function to fetch or create a new source filter in `g_filters`
 * @param source_id [in] string for `source_filter_t.source`
 * @param filter_ptr [out] pointer to a pointer containing `source_filter_t`.
 *          This is either set to point to the existing struct in g_filters,
 *          or is populated with new `source_id`.
 *          Pointer arguments are copied in C/C++.
 * @return true if `source_id` does not correspond to an existing source_filter_t in `g_filters`
 *
 * @remark The expression `auto* source = new source_filter_t` should always be used ahead of this function
 *          call to initialize new memory.
 */
bool get_source_filter( const std::string& source_id, source_filter_t** filter_ptr );


// Data Persistence Functions


bool readData();

void writeData();

#endif //SWIPR_DATA_H
