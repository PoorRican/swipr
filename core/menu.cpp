//
// Created by Josue David Figueroa on 9/28/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//

#include "menu.h"
#include "data.h"
#include "../utils/file.h"
#include "../preprocessing/calibrate.h"

#include <cstdlib>
#include <iostream>
#include <string>


using namespace std;

static void clear(){
    printf( "\033c" );
}

void main_menu(){
    char input;
    while( menuMode != EXIT ){
        clear();
        cout << "*******************" << endl;
        cout << "*    MAIN MENU    *" << endl;
        cout << "*******************" << endl << endl;

        // TODO: iterate through an array of function pointers
        cout << "0: Video Sources" << endl;
        cout << "Q: Quit" << endl;
        cout << " >>> ";
        cin >> input;
        switch( input ) {
            case '0': {
                menuMode = SOURCES;
                sources_menu();
            }
                break;

            case 'Q': {
                menuMode = EXIT;
            }
                break;

            default: {
                cout << endl << "Unknown option '" << input << "'" << endl;
            }
        }
    }
}


// Video Sources Menus

void sources_menu(){
    clear();
    char input;
    while( menuMode == SOURCES ){
        cout << "****************************" << endl;
        cout << "*    VIDEO SOURCES MENU    *" << endl;
        cout << "****************************" << endl << endl;

        // TODO: iterate through an array of function pointers
        cout << "0: New Video Source" << endl;
        cout << "1: Calibrate Existing Source" << endl;
        cout << "Q: Go back to Main Menu" << endl;

        cout << " >>> ";
        cin >> input;
        switch( input ) {
            case '0': {
                menuMode = CALIBRATE;
                new_region();
            }
                break;

            case '1': {
                menuMode = CALIBRATE;
                existing_region();
            }
                break;

            case 'Q': {
                menuMode = MAIN;
            }
                break;

            default: {
                cout << "Unknown option" << endl;
            }


        }
    }
}


void new_region(){
    string source_id, fn;
    auto* new_source = new source_filter_t;

    // get unique source_id
    for( ;; ){
        cout << "Enter Source ID (this can be any string): ";
        cin >> source_id;

        if( get_source_filter( source_id, &new_source ) ){
            break;
        } else{
            cout << "A source with that name already exists" << endl;
        }
    }

    // get path to video file
    cout << "Calibrating New Frame..." << endl;
    for( ;; ){
        cout << "Enter path to video file: ";
        cin >> fn;

        if( file_exists( fn ) ){
            break;
        } else{
            cout << "File does not exist." << endl;
        }
    }

    calibrateFromVideo( fn, &new_source );

    // return to sources menu
    menuMode = SOURCES;
}

void existing_region(){
    size_t source_indx;
    string fn;
    vector<string> existing;
    auto* existing_source = new source_filter_t;

    size_t count = 0;
    for( auto it = g_filters.begin(); it != g_filters.end(); it++, count++ ){
        existing.push_back( (*it).source );
        cout << count << ": " << (*it).source << endl;
    }

    // get unique source_id
    for( ;; ){
        cout << "Enter Source Index: ";
        cin >> source_indx;
        if( source_indx < existing.size() ){
            get_source_filter( existing[source_indx], &existing_source );
            break;
        } else{
            cout << "Invalid number..." << endl;
        }
    }

    // get path to video file
    cout << "Calibrating New Frame..." << endl;
    for( ;; ){
        cout << "Enter path to video file: ";
        cin >> fn;

        if( file_exists( fn ) ){
            break;
        } else{
            cout << "File does not exist." << endl;
        }
    }

    calibrateFromVideo( fn, &existing_source );

    // return to sources menu
    menuMode = SOURCES;
}
