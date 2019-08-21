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
static Vec4i& get_line();
static void render_line( Mat&, Vec4i& );


// ROI Functions
void calibrateROI( Mat& );
static void inputROI( int event, int x, int y, int flags, void* param );


// ROI-bounding Functions
static Rect calculateBoundingRect( const Mat& );
static void renderBoundingRect( Mat&, Rect& );
