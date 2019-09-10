#include <opencv2/opencv.hpp>
#include <vector>

#define DEBUG true


using namespace cv;


// ROI memory

vector<Vec4i> registerROI;          // stores all regions of interest
static int roi_i = -1;            // current index of registerROI vector


// UI state function

static bool drawing_box = false;        // true when user is drawing


// Utility Functions

template<class T>
void DBG(T s) {
    if (DEBUG) std::cout << s;
}

/* Wrapper for VideoWriter.open
 * Sets up frame buffer for writing to file system.
 */
void beginRender( char* f_name, VideoWriter&, Mat& );


// Sub-routines

/* This sub-routine creates a new Vec4i value and sets `current_roi` to point to this Vec4i value.
 */
static void new_roi();

/* Sub-routine that draws `current_roi` onto the frame provided.
 * Vec4i `current_roi` must be in (X1, Y1), (X2, Y2) form.
 */
static void render_rect( Mat&, Vec4i& );


// ROI Functions

/* This provides main user interaction functionality in calculating the location of ROI.
 * Starts a window displaying the frame passed and accepts user input via mouse clicks.
 * The user is expected to draw rectangles over every ROI of interest.
 */
void calibrateROI( Mat& );

/* Mouse callback. If the user presses left button, a box is started.
 * When the user releases that button, then that box is added to `registerROI`
 *  and is rendered on the the current image.
 * When the mouse is dragged, (w/ the button down) the second point of the line is moved.
 */
static void inputROI( int event, int x, int y, int flags, void* param );
