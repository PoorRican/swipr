#include "roi.h"
#include <functional>       // for reference_wrapper


using namespace cv;


// Utility Functions
double calculateSlope( Vec4i& l ) {
    return ( double(l[3]) - double(l[1]) ) / ( double(l[2]) - double(l[0]) );
}


void beginRender( char* f_name, VideoWriter& out, Mat& frame ) {
    Size dim( frame.cols, frame.rows );
    out.open( f_name, CV_FOURCC('D', 'I', 'V', 'X'), 30.0, dim, true );
    DBG( "Began video render\n" );
}


// Sub-routines
static Vec4i& get_line() {
    switch( step ) {
        case first: {
            next = last;
            return roi.first;
        } break;

        case last: {
            next = diagonal;
            return roi.last;
        } break;

        case diagonal: {
            next = done;
            DBG( "Done acquiring roi markers\n" );
            return roi.diagonal;
        } break;

        case done: {
            return roi.diagonal;
        } break;

    }
    // TODO: somehow return void or null value
    return roi.diagonal;
}

static void render_line( Mat& img, Vec4i& l ) {
    Scalar color;
    if( step == diagonal ) color = Scalar( 0, 255, 0 );
    else color = Scalar( 0, 0, 255 );

    const Point2i A( l[0], l[1] ), B( l[2], l[3] );

    line( img, A, B, color, 4 );
    circle( img, A, 5, color, 4 );
    circle( img, B, 5, color, 4 );
}


// ROI Functions
void calibrateROI( Mat& img ) {
    // TODO: implement UNDO functionality
    Mat src, temp;
    img.copyTo(src);
    img.copyTo(temp);

    const char* w_name = "Calibrate ROI";

    namedWindow( w_name, 0);

    /* NOTE: we set the value of 'params' to be the image we are working with
     * so that the callback will have the image to edit.
     */
    setMouseCallback( w_name, inputROI, (void*) &src );

    /* Main Function Loop. Here the working image is copied to the temp image,
     * and if the user is drawing, then put the currently edited line onto that temp image.
     * Display the temp image, and wait 15ms for a keystroke, then repeat.
     */
    for(;;) {
        src.copyTo(temp);
        if( drawing_box ) render_line( temp, get_line() );
        imshow( w_name, temp );
        if ( step == done ) break;
        else if ( waitKey ( 15 ) == 27) break;        // ESC to stop pre-maturely
    }

    roiRegion = calculateBoundingRect( temp );
    renderBoundingRect( temp, roiRegion );
    imshow( w_name, temp );
    DBG( "Got ROI rectangle\n" );
    for(;;) if ( waitKey ( 15 ) == 27) break;        // ESC to stop pre-maturely
}

static void inputROI( int event, int x, int y, int flags, void* param ) {
    Mat& image = *(Mat*) param;

    Vec4i& L = get_line();

    if( step != done ) {
        switch( event ) {
            case EVENT_MOUSEMOVE: {
                if( drawing_box ) {
                    L[2] = x;
                    L[3] = y;
                }
            }
            break;

            case EVENT_LBUTTONDOWN: {
                if( DEBUG ) {                       // stdout debug feedback
                    switch( step ) {
                        case first: {
                            std::cout << "\nDrawing first line\n";
                        } break;
                        case last: {
                            std::cout << "\nDrawing last line\n";
                        } break;
                        case diagonal: {
                            std::cout << "\nDrawing diagonal line\n";
                        } break;
                    }
                }

                drawing_box = true;
                L = { x, y, x, y };
            }
            break;

            case EVENT_LBUTTONUP: {
                drawing_box = false;
                render_line( image, L );
                step = next;
                DBG( "Slope: " ); DBG( calculateSlope(L) ); DBG( "\n" );
                DBG( "(" ); DBG( L[0] ); DBG( "x" ); DBG( L[1] ); DBG( ")\t(" ); DBG( L[2] ); DBG( "x" ); DBG( L[3] ); DBG( ")\n" ); 
            }
            break;
        }
    }
}


// ROI-bounding Functions
static Rect calculateBoundingRect( const Mat& img ) {
    std::reference_wrapper<Vec4i> lines[] = { roi.first, roi.last, roi.diagonal };

    // order points in `roi` from left-to-right (X is smaller)
    int i;
    for( i = 0; i < 3; i++ ) {
        Vec4i& line_ref = lines[i];
        if( line_ref[0] > line_ref[2] ) {
            int pt_tmp[2] = { line_ref[0], line_ref[1] };
            line_ref[0] = line_ref[2];
            line_ref[1] = line_ref[3];
            line_ref[2] = pt_tmp[0];
            line_ref[3] = pt_tmp[1];
        }
    }

    // find extrema
    struct {
        // initialized with ballpark numbers
        int tl_x = roi.first[0];
        int tl_y = roi.first[1];
        int br_x = roi.last[0];
        int br_y = roi.last[1];
    } extrema;
    for( i = 0; i < 3; i++ ) {
        Vec4i& line_ref = lines[i];

        // find tl-x (smallest x)
        if( line_ref[0] < extrema.tl_x ) extrema.tl_x = line_ref[0];
        // find tl-y (smallest y)
        if( line_ref[1] < extrema.tl_y ) extrema.tl_y = line_ref[1];
        if( line_ref[3] < extrema.tl_y ) extrema.tl_y = line_ref[3];
        // find bl-x (biggest x)
        if( line_ref[2] > extrema.br_x ) extrema.br_x = line_ref[2];
        // find bl-y (biggest y)
        if( line_ref[1] > extrema.br_y ) extrema.br_y = line_ref[1];
        if( line_ref[3] > extrema.br_y ) extrema.br_y = line_ref[3];
    }

    // add margin/padding
    int pad = floor(img.cols/50);             // padding (in px)
    if( extrema.tl_x < 25 ) extrema.tl_x = 1;
    else extrema.tl_x -= pad;

    if( extrema.tl_y < pad ) extrema.tl_y = 1;
    else extrema.tl_y -= pad;

    if( extrema.br_x > (img.cols - pad) ) extrema.br_x = img.cols - 1;
    else extrema.br_x += pad;

    if( extrema.br_y > (img.rows - pad) ) extrema.br_y = img.rows - 1;
    else extrema.br_y += pad;

    Point2i tl(extrema.tl_x, extrema.tl_y), br(extrema.br_x, extrema.br_y);
    DBG( "\nBounding Rectangle:\n" );
    DBG( "Top Left: " ); DBG ( tl ); DBG( "  Bottom Right:  " ); DBG( br ); DBG( "\n" );
    return Rect( tl, br );
}

static void renderBoundingRect( Mat& img, Rect& bound) {
    rectangle( img, bound.tl(), bound.br(), Scalar(255,0,0), 4 );
}


int main( int argc, char** argv ) {
    Mat first_frame;
    VideoCapture vid;
    vid.open( string(argv[1]) );


    // Grab first frame
    vid >> first_frame;
    DBG( "Video is: " ); DBG( first_frame.cols ); DBG( "x" ); DBG( first_frame.rows ); DBG( "\n" );
    calibrateROI(first_frame);

    // extract roiRegion from first_frame
    Mat extracted = first_frame( roiRegion );

    // Begin video writing
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
        extracted = tmp( roiRegion );
        imshow( w_name, extracted );
        if( waitKey(33) == 27 ) break;          // keep playback at ~30fps
        out.write( extracted );
        // NOTE: it might be better to write all images then use ffmpeg
    }

    // run register input
}
