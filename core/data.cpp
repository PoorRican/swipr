//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "data.h"
#include "debug.h"


using namespace cv;


std::vector<Vec4i> registerROI;


bool readData( const std::string& f_name ){
    DBG( "Reading data...\n" );
    FileStorage f( f_name, FileStorage::READ );
    f["registerROI"] >> registerROI;
    DBG( "Data read!\n\n" );
    return true;
}

void writeData( const std::string& f_name ){
    DBG( "Saving data...\n" );
    FileStorage f( f_name, FileStorage::WRITE );
    f << "registerROI" << registerROI;
    f.release();
    DBG( "Data saved\n\n" );
}
