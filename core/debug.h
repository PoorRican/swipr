//
// Created by Josue David Figueroa on 9/23/19.
// Copyright (c) 2019 Creation Consortium. All rights reserved.
//

#ifndef SWIPR_DEBUG_H
#define SWIPR_DEBUG_H

#endif //SWIPR_DEBUG_H

#ifdef DBG_OUTPUT
#define DEBUG true
#else
#define DEBUG false
#endif


template<class T>
void DBG( T s ){
    if( DEBUG ) std::cout << s;
}
