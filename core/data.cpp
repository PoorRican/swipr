//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "data.h"
#include "../utils/debug.h"


using namespace cv;


std::vector<source_filter_t> g_filters;


std::vector<action_sequence_t> g_sequences;


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
    int id;
    FileNode fn;

    // ROI Filter Data
    int n_filters = 0;
    fs["n_filters"] >> n_filters;
    g_filters.resize( n_filters );
    fn = fs["g_filters"];
    id = 0;
    for( FileNodeIterator it = fn.begin(); it != fn.end(); it++, id++ ){
        FileNode item = *it;

        std::string source;
        vector<cv::Vec4i> regions;
        item["source"] >> source;
        item["regions"] >> regions;

        g_filters[id] = {source, regions};
    }

    // Sequence Data
    int n_sequences = 0;
    fs["n_sequences"] >> n_sequences;
    g_sequences.resize( n_sequences );
    fn = fs["g_sequences"];
    id = 0;
    for( FileNodeIterator it = fn.begin(); it != fn.end(); it++, id++ ){
        FileNode item = *it;

        double start_frame, end_frame;
        std::string source, file;
        int region_id;
        int action;
        item["start_frame"] >> start_frame;
        item["end_frame"] >> end_frame;
        item["source"] >> source;
        item["fn"] >> file;
        item["region_id"] >> region_id;
        item["action"] >> action;

        g_sequences[id] = {start_frame, end_frame, source, file, region_id, (action_t) action};
    }

    DBG( "Data read!\n\n" );
    return true;
}

void writeData(){
    DBG( "Saving data...\n" );
    FileStorage fs( g_data_storage_fn, FileStorage::WRITE );


    // ROI Filter Data
    fs << "n_filters" << (int) g_filters.size();
    fs << "g_filters" << "[";
    for( size_t i = 0; i < g_filters.size(); i++ ){
        source_filter_t& s = g_filters[i];
        fs << "{" << "source" << s.source << "regions" << s.regions << "}";
    }
    fs << "]";


    // Sequence Data
    fs << "n_sequences" << (int) g_sequences.size();
    fs << "g_sequences" << "[";
    for( size_t i = 0; i < g_sequences.size(); ++i ){
        action_sequence_t& a = g_sequences[i];
        fs << "{";
        fs << "start_frame" << a.start_frame;
        fs << "end_frame" << a.end_frame;
        fs << "source" << a.source;
        fs << "fn" << a.fn;
        fs << "region_id" << a.region_id;
        fs << "action" << (int) a.action;
        fs << "}";
    }
    fs << "]";

    fs.release();
    DBG( "Data saved\n\n" );
}
