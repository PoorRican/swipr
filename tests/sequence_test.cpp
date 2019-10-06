//
// Created by Josue David Figueroa on 10/5/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "../core/data.h"
#include "../preprocessing/sequence.h"


const char* g_data_storage_fn = "tests/sequence_test.yaml";


int main( int argc, char** argv ){
    readData();
    labelSequencesFromVideo( argv[1], argv[1], 0 );
    return 0;
}
