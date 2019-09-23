//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "data.h"
#include "debug.h"


using namespace cv;


std::vector<Vec4i> registerROI;


bool readData(){
    DBG( "Reading data...\n" );
    FileStorage f( global_data_storage_fn, FileStorage::READ );
    f["registerROI"] >> registerROI;
    DBG( "Data read!\n\n" );
    return true;
}

void writeData(){
    DBG( "Saving data...\n" );
    FileStorage f( global_data_storage_fn, FileStorage::WRITE );
    f << "registerROI" << registerROI;
    f.release();
    DBG( "Data saved\n\n" );
}
