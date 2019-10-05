//
// Created by Josue David Figueroa on 9/28/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//

#ifndef SWIPR_MENU_H
#define SWIPR_MENU_H

#include "../preprocessing/calibrate.h"


/// @enum Main Menu possible state-machine values
enum menu_mode {
    MAIN, EXIT, SOURCES, CALIBRATE
};


/// @var Current Main Menu State
static menu_mode menuMode = MAIN;


/**
 * @brief Display main menu
 *
 * Choices shall be 'Sources', and 'Exit'.
 *
 */
void main_menu();


// Video Sources Functionality

/**
 * @brief Presents UI for creating and calibrating region filters from video sources
 */
void sources_menu();

/**
 * @brief Creates new source_filter_t and calibrates from existing video
 */
void new_region();

/**
 * @brief Calibrates existing source_filter_t and calibrates from existing video
 */
void existing_region();

#endif //SWIPR_MENU_H
