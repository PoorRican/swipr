#include "roi.h"
#include "keycodes.h"


using namespace cv;


void beginRender( char* f_name, VideoWriter& out, Mat& frame ){
    Size dim( frame.cols, frame.rows );
    out.open( f_name, CV_FOURCC( 'D', 'I', 'V', 'X' ), 30.0, dim, true );
    DBG( "Began video render\n" );
}


// Sub-routines
static void new_roi(){
    Vec4i v( -1, -1, 0, 0 );
    registerROI.push_back( v );
    roi_i++;
}

static void render_rect( Mat& img, Vec4i& L ){
    Scalar color( 0, 0, 255 );

    const Point2i A( L[0], L[1] ), B( L[2], L[3] );

    rectangle( img, A, B, color, 4 );
    //circle( img, A, 5, color, 4 );
    //circle( img, B, 5, color, 4 );
}

// ROI Functions
void calibrateROI( Mat& img ){
    // TODO: implement UNDO functionality
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
    for( ;; ){
        src.copyTo( temp );
        if( drawing_box ){
            render_rect( temp, registerROI[0] );
        }
        imshow( w_name, temp );
        if( waitKey( 15 ) == ESC_KEY ){         // ESC to stop
            DBG( "Got ESC. Moving on...\n" );
            break;
        }
    }

    // show final image
    imshow( w_name, temp );
    DBG( "Finished getting ROI rectangles\n" );
    for( ;; )
        if( waitKey( 15 ) == ESC_KEY ){
            DBG( "Got second ESC. Moving on...\n" );
            break;
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


int main( int argc, char** argv ){
    Mat first_frame;
    VideoCapture vid;
    vid.open( string( argv[1] ) );

    // Grab first frame
    vid >> first_frame;
    DBG( "Video is: " );
    DBG( first_frame.cols );
    DBG( "x" );
    DBG( first_frame.rows );
    DBG( "\n" );

    calibrateROI( first_frame );

    // show all ROI frames
    Mat extracted;
    for( int i = 0; i < registerROI.size(); i++ ){
        Vec4i& v = registerROI[i];
        extracted = first_frame( Rect( Point2i( v[0], v[1] ),
                                       Point2i( v[2], v[3] ) ) );
        const char* w_name = "Show ROI frames";
        namedWindow( w_name, 0 );
        imshow( w_name, extracted );
        for( ;; ) if( waitKey( 0 ) == ESC_KEY ) break;
    }

    // Begin video writing
    /*
    VideoWriter out;
    beginRender( argv[2], out, extracted );
    out << extracted;

    // Video playback
    Mat tmp;
    const char* w_name = "Playback window";
    namedWindow( w_name, 0 );
    imshow( w_name, extracted );
    for(;;) if( waitKey(0) == 27 ) break;

    int count = (int) vid.get(CV_CAP_PROP_FRAME_COUNT);
    DBG( "Detected " ); DBG( count ); DBG( " frames\n" );
    for(int i = 1; i < count; i++) {
        DBG( "Frame: " ); DBG( i ); DBG( "\n" );
        vid >> tmp;
        if( tmp.empty() ) {
            DBG( "No more frames\n" );
            break;
        }
        imshow( w_name, extracted );
        if( waitKey(33) == 27 ) break;          // keep playback at ~30fps
        out.write( extracted );
        // NOTE: it might be better to write all images then use ffmpeg
    }

    // run register input
     */
}
