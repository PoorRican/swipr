#include <opencv2/opencv.hpp>

#define DEBUG true


using namespace cv;


struct {
    Vec4i first;
    Vec4i last;
    Vec4i diagonal;
} roi;


bool drawing_box = false;

enum step_t {first, last, diagonal, done};
step_t step = first, next;


// Utility Functions
template<class T>
void DBG(T s) {
    if (DEBUG) std::cout << s;
}
double calculateSlope( Vec4i& );


// Sub-routines
Vec4i& get_line();
void render_line( Mat&, Vec4i& );


// ROI Functions
void calibrateROI( Mat& );
void inputROI( int event, int x, int y, int flags, void* param );


// ROI-bounding Functions
Rect calculateBoundingRect( const Mat& );
void renderBoundingRect( Mat&, Rect& );
