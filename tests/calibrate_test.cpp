//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "../core/data.h"
#include "../preprocessing/calibrate.h"


const char* global_data_storage_fn = "calibrate_test.yaml";


int main( int argc, char** argv ){
    readData();
    calibrateFromVideo( argv[1], argv[2], "/tmp/static_roi/", true );
    return 0;
}

