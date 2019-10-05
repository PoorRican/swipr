//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "data.h"
#include "debug.h"


using namespace cv;


std::vector<source_filter_t> g_filters;


bool get_source_filter( const std::string& source_id, source_filter_t** filter_ptr ){
    for( auto it = g_filters.begin(); it != g_filters.end(); it++ ){
        if( (*it).source == source_id ){
            *filter_ptr = &(*it);                           // filter_ptr points to vector memory
            return false;                                   // source_filter_t was not created
        }
    }
    // source_filter_t is being created
    (**filter_ptr).source = source_id;                      // update new source_filter_t
    g_filters.push_back( **filter_ptr );                      // add source_filter_t to vector memory
    *filter_ptr = &(g_filters[g_filters.size() - 1]);        // filter_ptr should point to vector memory
    return true;                                            // source_filter_t was created
}


bool readData(){
    DBG( "Reading data...\n" );
    FileStorage fs( g_data_storage_fn, FileStorage::READ );

    if( !fs.isOpened() )
        return false;

    int n_filters = 0;
    fs["n_filters"] >> n_filters;
    g_filters.resize( n_filters );
    FileNode fn = fs["g_filters"];
    int id = 0;
    for( FileNodeIterator it = fn.begin(); it != fn.end(); it++, id++ ){
        FileNode item = *it;

        std::string source;
        vector<cv::Vec4i> regions;
        item["source"] >> source;
        item["regions"] >> regions;

        g_filters[id] = {source, regions};
    }

    DBG( "Data read!\n\n" );
    return true;
}

void writeData(){
    DBG( "Saving data...\n" );
    FileStorage fs( g_data_storage_fn, FileStorage::WRITE );
    fs << "n_filters" << (int) g_filters.size();
    fs << "g_filters" << "[";
    for( int i = 0; i < (int) g_filters.size(); i++ ){
        source_filter_t& s = g_filters[i];
        fs << "{:" << "source" << s.source << "regions" << s.regions << "}";
    }
    fs << "]";
    fs.release();
    DBG( "Data saved\n\n" );
}
