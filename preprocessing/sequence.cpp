//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//

#include "sequence.h"
#include "../core/data.h"
#include "../core/keycodes.h"
#include "../utils/debug.h"
#include "../utils/file.h"
#include "../utils/roi.h"

#include <iostream>


using namespace std;

void get_sequence( const std::string& video_fn, const action_sequence_t& sequence, std::vector<cv::Mat>& frames ){
    if( !file_exists( video_fn ) ){
        cout << "Invalid Video Path/Filename" << endl;
        exit( 5 );
    }
    cv::Mat frame;
    cv::VideoCapture vid( video_fn );

    vid.set( CV_CAP_PROP_POS_FRAMES, sequence.start_frame );
    frames.clear();

    for( int i = 0; i < (sequence.end_frame - sequence.start_frame); ++i ){
        vid >> frame;
        frames.push_back( frame );
    }
}


void labelSequencesFromVideo( const string& video_fn, const string& source_id, const int& region_id,
                              const double& start_frame ){
    auto* filter = new source_filter_t;
    bool existing = !get_source_filter( source_id, &filter );
    if( !existing ){
        cout << "Invalid Source ID" << endl;
        exit( 5 );
    }
    if( !file_exists( video_fn ) ){
        cout << "Invalid Video Path/Filename" << endl;
        exit( 5 );
    }

    sequenceUI_state uiState = SEEKING;

    cv::Mat img, tmp;
    cv::VideoCapture vid( video_fn );
    auto* pSequence = new action_sequence_t;
    double frame = start_frame;

    // open video file
    if( start_frame != 0 ){
        vid.set( CV_CAP_PROP_POS_FRAMES, start_frame );
    }
    vid >> tmp;

    // display video
    const char* w_name = "Calibrate Sequence";
    cv::namedWindow( w_name, 0 );

    while( uiState != DONE ){
        extract_frame( tmp, img, (filter->regions)[region_id] );
        cv::imshow( w_name, img );

        // handle keyboard input
        char input = (char) cv::waitKey( 0 );
        switch( input ) {
            case ESC_KEY: {
                if( uiState == SEEKING ){
                    cout << "Done extracting sequences" << endl;
                    uiState = DONE;
                }
                if( uiState == INSIDE ){
                    cout << "Canceled sequence" << endl;
                    // cancel current action_sequence_t
                    uiState = SEEKING;
                }
            }
                break;

            case SPACE_KEY: {
                if( uiState == INSIDE &&
                    pSequence->start_frame != frame ){      // there can't be a sequence of 0 frames
                    // Save sequence
                    cout << "Created new sequence ending at " << frame << endl;

                    pSequence->end_frame = frame;
                    pSequence->region_id = region_id;
                    pSequence->fn = video_fn;
                    pSequence->source = source_id;
                    pSequence->action = SCAN;

                    g_sequences.push_back( *pSequence );
                    pSequence = new action_sequence_t;

                    uiState = SEEKING;
                } else if( uiState == SEEKING ){
                    // Start sequence
                    cout << "Began Sequence at " << frame << endl;

                    pSequence->start_frame = frame;
                    uiState = INSIDE;
                }
            }
                break;

            case RIGHT_KEY: {
                frame++;

                DBG( "right: frame now at " );
                DBG( frame );
                DBG( '\n' );

                vid >> tmp;
                if( tmp.empty() ){
                    cout << "No more frames..." << endl;
                    uiState = DONE;
                }
            }
                break;

            case LEFT_KEY: {
                if( frame > 0 ){
                    frame--;

                    DBG( "left: frame now at " );
                    DBG( frame );
                    DBG( '\n' );

                    vid.set( CV_CAP_PROP_POS_FRAMES, frame );
                    vid >> tmp;
                } else{
                    DBG( "frame at 0\n" );
                }
            }
                break;

            case -1: {

                // this is what is normally returned by `waitKey` w/ no input
            }
                break;

            default: {
                DBG( (int) input );
                DBG( '\n' );
            }
                break;
        }
    }
    cv::destroyWindow( w_name );
    writeData();
}
