//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "data.h"
#include "debug.h"


using namespace cv;


std::vector<source_filter_t> g_filters;


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
