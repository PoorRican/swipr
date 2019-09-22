#include "sys/stat.h"       // for mkdir
#include "cstring"
#include "roi.h"
#include "keycodes.h"


#define RENDER_PATH "/tmp/static_roi/"      // tmp


using namespace cv;


// Sub-routines
static void new_roi(){
    Vec4i v( -1, -1, 0, 0 );
    registerROI.push_back( v );
    ++roi_i;
}

static void undo_roi(){
    if( roi_i > -1 ){
        registerROI.pop_back();
        --roi_i;
    }
}

static void render_rect( Mat& img, Vec4i& L ){
    Scalar color( 0, 0, 255 );

    const Point2i A( L[0], L[1] ), B( L[2], L[3] );

    rectangle( img, A, B, color, 4 );
    //circle( img, A, 5, color, 4 );
    //circle( img, B, 5, color, 4 );
}

static void render_roi( Mat& img ){
    for( std::size_t i = 0; i < registerROI.size(); i++ ){
        Vec4i& v = registerROI[i];
        render_rect( img, v );
    }
}

static void inputROI( int event, int x, int y, int flags, void* param ){
    Mat& image = *(Mat*) param;

    switch( event ) {
        case EVENT_MOUSEMOVE: {
            if( drawing_box ){
                registerROI[roi_i][2] = x;
                registerROI[roi_i][3] = y;
            }
        }
            break;

        case EVENT_LBUTTONDOWN: {
            drawing_box = true;
            new_roi();            // create new Vec4i rect
            Vec4i v( x, y, x, y );
            registerROI[roi_i] = v;
        }
            break;

        case EVENT_LBUTTONUP: {
            drawing_box = false;
            render_rect( image, registerROI[roi_i] );
            DBG( "New ROI Coordinates: (" );
            DBG( registerROI[roi_i][0] );
            DBG( ", " );
            DBG( registerROI[roi_i][1] );
            DBG( ")\t(" );
            DBG( registerROI[roi_i][2] );
            DBG( ", " );
            DBG( registerROI[roi_i][3] );
            DBG( ")\n" );
        }
            break;
    }
}

// Main Functionality
void calibrateROI( Mat& img ){
    Mat src, temp;
    img.copyTo( src );
    img.copyTo( temp );

    const char* w_name = "Calibrate ROI";       // window string
    namedWindow( w_name, 0 );

    /* NOTE: we set the value of 'params' to be the image we are working with
     * so that the callback will have the image to edit.
     */
    setMouseCallback( w_name, inputROI, (void*) &src );

    /* Main Function Loop. Here the working image is copied to the temp image,
     * and if the user is drawing, then put the currently edited line onto that temp image.
     * Display the temp image, and wait 15ms for a keystroke, then repeat.
     */
    bool done = false;
    for( ;; ){
        src.copyTo( temp );
        if( drawing_box ){
            render_rect( temp, registerROI[0] );
        }
        imshow( w_name, temp );

        // handle input
        int input = waitKey( 15 );
        switch( input ) {
            case ESC_KEY: {
                DBG( "Got ESC. Moving on...\n" );
                done = true;
            }
                break;

            case U_KEY: {
                std::cout << "'u' key was pressed" << std::endl;
                undo_roi();
                img.copyTo( src );
                render_roi( src );
            }
                break;

            case -1: {
                // this is what is normally returned by `waitKey` w/ no input
            }
                break;

            default: {
                DBG( input );
                DBG( "\n" );
            }
                break;
        }
        if( done ){ break; }
    }

    destroyWindow( w_name );

    DBG( "Finished getting regions of interest\n\n" );
    // TODO: implement persistent data
}


int main( int argc, char** argv ){
    Mat frame;
    VideoCapture vid;
    vid.open( string( argv[1] ) );
    const string output = string( argv[2] );

    // Grab first frame
    vid >> frame;
    DBG( "Video is: " );
    DBG( frame.cols );
    DBG( "x" );
    DBG( frame.rows );
    DBG( "\n" );

    calibrateROI( frame );

    // create directories for each roi
    DBG( "Creating directories for each region of interest\n\n" );
    Mat extracted;
    // TODO: delete any existing directories
    mkdir( RENDER_PATH, 0777 );
    for( std::size_t i = 0; i < registerROI.size(); i++ ){
        Vec4i& v = registerROI[i];
        // TODO: add a catch-all and remove malformed ROI rectangles
        extracted = frame( Rect( Point2i( v[0], v[1] ),
                                 Point2i( v[2], v[3] ) ) );
        string path = string( RENDER_PATH ) + "/" + std::to_string( i );
        mkdir( path.c_str(), 0777 );
    }

    // Begin video writing
    int count = (int) vid.get( CV_CAP_PROP_FRAME_COUNT );
    DBG( "Detected " ); DBG( count ); DBG( " frames\n" );
    for( int i = 1; i < count; i++ ){
        for( std::size_t r = 0; r < registerROI.size(); r++ ){
            string f_name =
                    string( RENDER_PATH ) + "/" + std::to_string( r ) + "/" + output + "_" + std::to_string( i ) +
                    ".jpg";
            Vec4i& v = registerROI[r];
            extracted = frame( Rect( Point2i( v[0], v[1] ),
                                     Point2i( v[2], v[3] ) ) );
            imwrite( f_name, extracted );
        }

        // TODO: show percentage done
        DBG( "Frame: " ); DBG( i ); DBG( "\n" );
        vid >> frame;
        if( frame.empty() ){
            DBG( "\nNo more frames!\n\n" );
            break;
        }
    }

    // ffmpeg and playback
    vector<string> _ffmpeg_cmds, _xdg_cmds;
    for( std::size_t r = 0; r < registerROI.size(); r++ ){
        string path = string( RENDER_PATH ) + std::to_string( r );
        // TODO: fetch framerate
        // ffmpeg command
        string cmd = "'ffmpeg -f image2 -framerate 30 -i " + path;
        cmd += "/";
        cmd += output + "_%d.jpg ";

        cmd += path + '/';
        cmd += output + ".avi";
        cmd += "-y &> /dev/null' ";                // overwrite any existing files and ignore stdout

        _ffmpeg_cmds.push_back( cmd );

        // xdg command
        cmd = "'xdg-open " + path;
        cmd += "/";
        cmd += output + ".avi &> /dev/null' ";    // ignore stdout

        _xdg_cmds.push_back( cmd );
    }

    string ffmpeg_cmd = "parallel ::: ", xdg_cmd = "parallel ::: ";
    for( std::size_t r = 0; r < registerROI.size(); r++ ){
        ffmpeg_cmd.append( _ffmpeg_cmds[r] );
        xdg_cmd.append( _xdg_cmds[r] );
    }

    DBG( "Calling ffmpeg\n" );
    system( ffmpeg_cmd.c_str() );
    DBG( "Calling xdg-open\n" );
    system( xdg_cmd.c_str() );
}
