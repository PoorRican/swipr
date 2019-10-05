//
// Created by Josue David Figueroa on 9/29/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//
#include "core/data.h"
#include "core/menu.h"


const char* g_data_storage_fn = "swipr.yaml";


int main( int argc, char** argv ){
    readData();
    main_menu();
    return 0;
}

