#include <opencv2/opencv.hpp>
#include <vector>

#define DEBUG true


using namespace cv;


// ROI memory

/**
 * @brief stores all regions of interest
 *
 * Regions are defined by two coordinates and shall be ordered [X1, Y1, X2, Y2]
 *
 * @remark It is preferred that coordinates be top-left and bottom-right but that is not required.
 */
vector<Vec4i> registerROI;


/** @brief stores the index of the ROI that is being worked on
 * In EDIT mode, this will store the ROI that is being edited.
 * In INPUT mode, this will store the last created ROI
 */
static int roi_i = -1;


// UI state

/**
 * @brief Defines modes of UI state machine
 */
enum UI_mode {
    WAITING, INPUT, DRAWING, EDIT, DONE
};


/**
 * @brief Current UI state
 */
static UI_mode uiMode = WAITING;


// Utility Functions

template<class T>
void DBG(T s) {
    if (DEBUG) std::cout << s;
}


// Sub-routines

/**
 * @brief Sub-routine that creates a new Vec4i value and increases `roi_i` to reflect an additional member.
 *
 * @remark This is used in INPUT mode and implemented in `inputROI`
 */
static void new_roi();

/**
 * @brief Sub-routine that removes the last Vec4i that was created, and decrements down the index counter.
 *
 * @remark This is used in INPUT mode and associated with 'u'
 */
static void undo_roi();

/**
 * @brief Sub-routine that draws a rectangle using the provided coordinates onto the image provided.
 *
 * This function changes the color used based of the UI state `uiMode`. It defaults to using red as the
 *  color used.
 *
 * @remark This is UI function and is called as ROIs are moved and edited.
 *
 * @param [out]img The frame on which to draw on. This should be a duplicate of the original image.
 * @param [in]L Two pairs of coordinates in (X1, Y1), (X2, Y2) form.
 */
static void render_rect( Mat& img, const Vec4i& L );

/**
 * @brief Overloaded sub-routine that draws a stylized rectangle using the provided coordinates onto the image provided.
 *
 * @remark This is UI function and is called as ROIs are moved and edited.
 *
 * @param [out]img The frame on which to draw on. This should be a duplicate of the original image.
 * @param [in]L Two pairs of coordinates in (X1, Y1), (X2, Y2) form.
 * @param [in]color Color to use when rendering rectangle onto `img`
 * @param [in]radius Radius of circles used to accentuate corners
 */
static void render_rect( Mat& img, const Vec4i& L, Scalar color, int radius );

/**
 * @brief Sub-routine method which renders all ROIs in `registerROI`.
 *
 * @remark This is meant to be called after `undo_roi` and during editing.
 *
 * @param [out]img The frame to which to draw on. This should be a duplicate of the original image.
 */
static void render_all_roi( Mat& img );

/**
 * @brief Mouse callback for inputting ROI.
 *
 * @remark This is the callback used when `uiMode` is `INPUT`.
 *
 * When the left button is clicked, `uiMode` is set to `DRAWING` and
 *  `new_roi` is called an set to the coordinates of `x` and `y`.
 * As the mouse is moved, the second pair of coordinates of `registerROI[roi_i]` are updated.
 * When the left button is released, `uiMode` returns to `INPUT`.
 *
 * @param [in]event type of UI input. This corresponds to key-presses and mouse-clicks.
 * @param [in]x X coordinate of UI event
 * @param [in]y Y coordinate of UI event
 * @param [in]flags flags Unused. This is part of the signature required by cv::setMouseCallback
 * @param [in]param This is the src image that is being used as UI input and feedback
 */
static void inputROI( int event, int x, int y, int flags, void* param );

/**
 * @brief Mouse callback for editing existing ROIs.
 *
 * @remark This is the callback used when `uiMode` is `EDIT`
 *
 * When `x` and `y` are within reasonable range of a coordinate in `registerROI`,
 *  the rectangle is marked by `roi_i` and is then highlighted via the overloaded `render_rect` function.
 *
 * @param [in]event type of UI input. This corresponds to key-presses and mouse-clicks.
 * @param [in]x X coordinate of UI event
 * @param [in]y Y coordinate of UI event
 * @param [in]flags flags Unused. This is part of the signature required by cv::setMouseCallback
 * @param [in]param This is the src image that is being used as UI input and feedback
 */
static void editROI( int event, int x, int y, int flags, void* param );


// ROI Functions

/** @brief Provides user interaction for calculating the coordinates of regions of interest (ROIs).
 *
 * Starts a window displaying the frame passed and accepts user input via mouse clicks.
 * The user is expected to draw rectangles over every ROI of interest.
 *
 * @remark The window is constantly refreshed and might not be the most efficient implementation.
 *  This shouldn't be a problem as this is just the calibration step.
 *
 * @param [out]img cv::Mat of the frame to use for calibrating ROI coordinates
 */
void calibrateROI( const Mat& img );

