//
// Created by Josue David Figueroa on 10/5/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//

#ifndef SWIPR_FILE_H
#define SWIPR_FILE_H

#include <cstdlib>
#include <string>


/**
 * @brief Utility function to check if file exists on the filesystem
 *
 * @param fn[in] path to file
 *
 * @return true if file exists, otherwise false
 */
inline bool file_exists( const std::string& fn ){
    std::string cmd = "test -e " + fn;           // POSIX exit code is 0 when true, 1 when false
    return !(bool) (system( cmd.c_str() ));  // invert and cast output
}

#endif //SWIPR_FILE_H
