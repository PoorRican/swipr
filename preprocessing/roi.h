#include <opencv2/opencv.hpp>

#define DEBUG true


using namespace cv;


// ROI memory
struct {
    Vec4i first;
    Vec4i last;
    Vec4i diagonal;
} roi;
Rect roiRegion;
vector<Vec4i> registers;


// UI state function
bool drawing_box = false;

enum step_t {first, last, diagonal, done};
step_t step = first, next;


// Utility Functions
template<class T>
void DBG(T s) {
    if (DEBUG) std::cout << s;
}
double calculateSlope( Vec4i& );
void beginRender( char* f_name, VideoWriter&, Mat& );


// Sub-routines
/* This sub-routine returns the next Vec4i value in `roi`.
 *
 * Returns:
 *  Vec4i& to `roi.first`, `roi.last`, or `roi.diagonal` dependant on `step`
 */
static Vec4i& get_line();
/* Sub-routine that draws a line defined by `l` onto `img`.
 * Vec4i must be in (X1, Y1), (X2, Y2) form
 */
static void render_line( Mat&, Vec4i& );


// ROI Functions
void calibrateROI( Mat& );
/* Mouse callback. If the user presses left button, a line is started.
 * When the user releases that button, then that line is added to the current image.
 * When the mouse is dragged, (w/ the button down) the second point of the line is moved.
 */
static void inputROI( int event, int x, int y, int flags, void* param );


// ROI-bounding Functions
static Rect calculateBoundingRect( const Mat& );
static void renderBoundingRect( Mat&, Rect& );
