//
// Created by Josue David Figueroa on 9/9/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//

#include "sys/stat.h"       // for mkdir

#include "calibrate.h"
#include "../core/keycodes.h"
#include "../core/debug.h"
#include "../core/data.h"


using namespace cv;


vector<Vec4i>* registerROI = nullptr;


// Utility Functions

static void createTmpDirectories( const string& tmp_dir ){
    DBG( "Creating directories for each region of interest\n\n" );
    Mat extracted;
    if( tmp_dir.find( "/tmp/" ) ){        // ensure that `tmp_dir` is in '/tmp/' and delete any existing
        system( (string( "rm -rf " + tmp_dir ).c_str()) );
    }
    mkdir( tmp_dir.c_str(), 0777 );
    for( std::size_t i = 0; i < (*registerROI).size(); i++ ){
        string path = tmp_dir + "/" + std::to_string( i );
        mkdir( path.c_str(), 0777 );
    }
}

inline static void calibrateRegionHelp(){
    std::cout << "Waiting for user input...\n";
    std::cout << "\tPress 'i' for INPUT mode.\n";
    std::cout << "\tPress 'e' for EDIT mode.\n";
    std::cout << "Or press 'ESC' to continue.\n";
}

inline static void inputROIHelp(){
    std::cout << "INPUT mode:\n";
    std::cout << "\tClick a corner, then left click to draw the region of interest.\n\n";
    std::cout << "\tOr press 'u' to undo last region.\n";
    std::cout << "\tOr press 'e' for EDIT mode.\n";
    std::cout << "Press 'ESC' to leave INPUT mode.\n";
}

inline static void editROIHelp(){
    std::cout << "EDIT mode:\n";
    std::cout << "Hover over a corner and left click to select. drag to edit region boundaries.\n\n";
    std::cout << "Double click right mouse button to delete a selected region.\n\n";
    std::cout << "Or press 'i' for INPUT mode.\n";
    std::cout << "Press 'ESC' to leave EDIT mode.\n";
}


// Sub-routines

static void new_roi(){
    Vec4i v( -1, -1, 0, 0 );
    (*registerROI).push_back( v );
    ++roi_i;
}

static void undo_roi(){
    if( roi_i > -1 ){
        (*registerROI).pop_back();
        --roi_i;
    }
}

static void render_rect( Mat& img, const Vec4i& L ){
    Scalar color( 0x00, 0x00, 0xFF );
    if( uiMode == EDIT ){
        color = {0xFF, 0x99, 0x30};
    } else{
    }

    const Point2i A( L[0], L[1] ), B( L[2], L[3] );

    rectangle( img, A, B, color, 8 );
}

static void render_rect( Mat& img, const Vec4i& L, const Scalar color, const int radius = 7 ){
    const Point2i A( L[0], L[1] ), B( L[2], L[3] );

    rectangle( img, A, B, color, 12 );
    Scalar red( 0, 0, 255 );
    circle( img, A, radius, red, -1 );
    circle( img, B, radius, red, -1 );
}

static void render_all_roi( Mat& img ){
    for( std::size_t i = 0; i < (*registerROI).size(); i++ ){
        Vec4i& v = (*registerROI)[i];
        render_rect( img, v );
    }
}


// Callback Functions

static void editROI( int event, int x, int y, int flags, void* param ){
    Mat& image = *(Mat*) param;

    // stores whether selected coordinates is the first or second pair in Vec4i
    static auto first_coord = true;

    switch( event ) {
        case EVENT_MOUSEMOVE: {
            // select a mouse corner
            if( uiMode == EDIT ){
                // TODO: implement selecting a side
                const int padding = 25;
                for( std::size_t r = 0; r < (*registerROI).size(); ++r ){
                    if( (x >= (*registerROI)[r][0] - padding) && (x <= (*registerROI)[r][0] + padding) &&
                        (y >= (*registerROI)[r][1] - padding) && (y <= (*registerROI)[r][1] + padding) ){
                        first_coord = true;
                        roi_i = (int) r;
                        break;
                    } else if( (x >= (*registerROI)[r][2] - padding) && (x <= (*registerROI)[r][2] + padding) &&
                               (y >= (*registerROI)[r][3] - padding) && (y <= (*registerROI)[r][3] + padding) ){
                        first_coord = false;
                        roi_i = (int) r;
                        break;
                    } else{
                        roi_i = -1;
                    }
                }
                // NOTE: highlighting is handled in `calibrateRegions`
            }
                // drag selected corner
            else if( uiMode == DRAWING ){
                if( first_coord ){
                    (*registerROI)[roi_i][0] = x;
                    (*registerROI)[roi_i][1] = y;
                } else{
                    (*registerROI)[roi_i][2] = x;
                    (*registerROI)[roi_i][3] = y;
                }
            }
        }
            break;

        case EVENT_LBUTTONDOWN: {
            if( uiMode == EDIT && roi_i != -1 ){
                uiMode = DRAWING;       // begin to drag selected ROI
            }
        }
            break;

        case EVENT_LBUTTONUP: {
            if( uiMode == DRAWING ){
                DBG( "New ROI Coordinates: (" );
                DBG( (*registerROI)[roi_i][0] );
                DBG( ", " );
                DBG( (*registerROI)[roi_i][1] );
                DBG( ")\t(" );
                DBG( (*registerROI)[roi_i][2] );
                DBG( ", " );
                DBG( (*registerROI)[roi_i][3] );
                DBG( ")\n" );
                roi_i = -1;
                uiMode = EDIT;
            }
        }
            break;

        case EVENT_RBUTTONDBLCLK: {
            if( uiMode == EDIT && roi_i != -1 ){
                (*registerROI).erase( (*registerROI).begin() + roi_i );     // delete selected ROI
            }
        }

        default: {
            DBG( "Unknown UI event during inputROI" );
        }
            break;
    }
}

static void inputROI( int event, int x, int y, int flags, void* param ){
    Mat& image = *(Mat*) param;

    switch( event ) {
        case EVENT_MOUSEMOVE: {
            // check that UI click was within image
            if( uiMode == DRAWING && x >= 0 && x < image.cols && y >= 0 && y < image.rows ){
                (*registerROI)[roi_i][2] = x;
                (*registerROI)[roi_i][3] = y;
            }
        }
            break;

        case EVENT_LBUTTONDOWN: {
            uiMode = DRAWING;
            // check that UI click was within image
            if( x >= 0 && x < image.cols && y >= 0 && y < image.rows ){
                new_roi();            // create new Vec4i rect
                Vec4i v( x, y, x, y );
                (*registerROI)[roi_i] = v;
            }
        }
            break;

        case EVENT_LBUTTONUP: {
            if( uiMode == DRAWING ){
                uiMode = INPUT;
                DBG( "New ROI Coordinates: (" );
                DBG( (*registerROI)[roi_i][0] );
                DBG( ", " );
                DBG( (*registerROI)[roi_i][1] );
                DBG( ")\t(" );
                DBG( (*registerROI)[roi_i][2] );
                DBG( ", " );
                DBG( (*registerROI)[roi_i][3] );
                DBG( ")\n" );
            }
        }
            break;

        default: {
            DBG( "Unknown UI event during inputROI" );
        }
            break;
    }
}


// Main Functionality
void calibrateRegions( const Mat& img ){
    Mat drawn;
    img.copyTo( drawn );

    const char* w_name = "Calibrate ROI";       // window string
    namedWindow( w_name, 0 );
    imshow( w_name, drawn );

    calibrateRegionHelp();

    /* Main Function Loop. Here the working image is copied to the drawn image,
     * and if the user is drawing, then put the currently edited line onto that drawn image.
     * Display the drawn image, and wait 15ms for a keystroke, then repeat.
     */
    for( ;; ){
        // render and/or highlight ROIs
        if( (uiMode == DRAWING) ^ (uiMode == EDIT && roi_i != -1) ){
            img.copyTo( drawn );
            for( std::size_t r = 0; r < (*registerROI).size(); r++ ){
                if( r == roi_i ){
                    render_rect( drawn, (*registerROI)[r], Scalar( 0, 255, 255 ), 15 );
                } else{
                    render_rect( drawn, (*registerROI)[r] );
                }
            }
            imshow( w_name, drawn );
        } else{
            // NOTE: is there a way to optimize this so that window is not constantly refreshed?
            img.copyTo( drawn );
            render_all_roi( drawn );
            imshow( w_name, drawn );
        }

        // handle keyboard input
        char input = (char) waitKey( 15 );
        switch( input ) {
            case ESC_KEY: {
                if( uiMode == INPUT || uiMode == EDIT ){
                    calibrateRegionHelp();

                    uiMode = WAITING;
                    img.copyTo( drawn );
                    render_all_roi( drawn );
                    setMouseCallback( w_name, nullptr, nullptr );

                    DBG( "Now waiting for user to change modes\n" );
                } else if( uiMode == WAITING ){
                    DBG( "Got ESC. Moving on...\n" );
                    uiMode = DONE;
                }
            }
                break;

            case 'u': {
                if( uiMode == INPUT ){
                    undo_roi();
                    img.copyTo( drawn );
                    render_all_roi( drawn );
                    DBG( "Undid last action\n" );
                }
            }
                break;

            case 'e': {
                /* NOTE: we set the value of 'params' to be the image we are working with
                 * so that the callback will have the image to edit.
                 */
                inputROIHelp();

                uiMode = EDIT;
                roi_i = -1;
                setMouseCallback( w_name, editROI, (void*) &drawn );

                DBG( "Now in EDIT mode\n" );
            }
                break;

            case 'i': {
                /* NOTE: we set the value of 'params' to be the image we are working with
                 * so that the callback will have the image to edit.
                 */
                editROIHelp();

                uiMode = INPUT;
                if( !(*registerROI).empty() ){
                    roi_i = (int) ((*registerROI).size()) - 1;
                }
                setMouseCallback( w_name, inputROI, (void*) &drawn );

                DBG( "Now in INPUT mode\n" );
            }

            case -1: {
                // this is what is normally returned by `waitKey` w/ no input
            }
                break;

            default: {
                DBG( input );
                std::cout << (input == 27);
                DBG( "\n" );
            }
                break;
        }
        if( uiMode == DONE ){ break; }
    }

    destroyWindow( w_name );

    DBG( "Finished getting regions of interest\n\n" );
    // TODO: implement persistent data
}


void calibrateFromVideo( const string& input, const string& source_id, const string& prefix, const string& render_path,
                         bool render ){
    Mat frame;
    VideoCapture vid;
    vid.open( string( input ) );

    bool new_source = true;
    for( auto it = g_filters.begin(); it != g_filters.end(); it++ ){
        if( (*it).source == source_id ){
            new_source = false;
            registerROI = &((*it).regions);
        }
    }
    if( new_source ){
        source_filter_t source;
        source.source = source_id;
        g_filters.push_back( source );
        registerROI = &(g_filters[g_filters.size() - 1].regions);
    }

    // Grab first frame
    vid >> frame;
    DBG( "Video is: " );
    DBG( frame.cols );
    DBG( "x" );
    DBG( frame.rows );
    DBG( "\n\n" );
    DBG( "Awaiting User Input\n\n" );

    calibrateRegions( frame );
    writeData();

    if( render && !(*registerROI).empty() ){
        Mat extracted;
        const string output = string( prefix );
        createTmpDirectories( render_path );

        // Extract Frames
        DBG( "Extracting Frames:\n" );
        int count = (int) vid.get( CV_CAP_PROP_FRAME_COUNT );
        DBG( count );
        DBG( " frames detected\n" );
        int completed = -1;
        for( int i = 1; i <= count; i++ ){
            for( std::size_t r = 0; r < (*registerROI).size(); r++ ){
                string f_name =
                        string( render_path ) + "/" + std::to_string( r ) + "/" + output + "_" + std::to_string( i ) +
                        ".jpg";
                Vec4i& v = (*registerROI)[r];
                extracted = frame( Rect( Point2i( v[0], v[1] ),
                                         Point2i( v[2], v[3] ) ) );
                imwrite( f_name, extracted );
            }

            int percentage = (int) (((float) i / (float) count) * 100);
            if( percentage % 10 == 0 && completed != percentage ){
                completed = percentage;
                DBG( completed );
                DBG( "% complete...\n" );
            }
            vid >> frame;
            if( frame.empty() ){
                DBG( "\nNo more frames!\n\n" );
                break;
            }
        }

        // ffmpeg and playback
        vector<string> _ffmpeg_cmds, _xdg_cmds;
        for( std::size_t r = 0; r < (*registerROI).size(); r++ ){
            string path = string( render_path ) + std::to_string( r );
            // TODO: fetch framerate
            // ffmpeg command
            string cmd = "'ffmpeg -f image2 -framerate 30 -i " + path;
            cmd += "/";
            cmd += output + "_%d.jpg ";

            cmd += path + '/';
            cmd += output + ".avi ";
            cmd += "&> /dev/null' ";                    // ignore stdout

            _ffmpeg_cmds.push_back( cmd );

            // xdg command
            cmd = "'xdg-open " + path;
            cmd += "/";
            cmd += output + ".avi &> /dev/null' ";      // ignore stdout

            _xdg_cmds.push_back( cmd );
        }

        string ffmpeg_cmd = "parallel ::: ", xdg_cmd = "parallel ::: ";
        for( std::size_t r = 0; r < (*registerROI).size(); r++ ){
            ffmpeg_cmd.append( _ffmpeg_cmds[r] );
            xdg_cmd.append( _xdg_cmds[r] );
        }

        DBG( "Calling ffmpeg\n" );
        system( ffmpeg_cmd.c_str() );
        DBG( "Calling xdg-open\n" );
        system( xdg_cmd.c_str() );
    }

}
