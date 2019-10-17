﻿#include "io.h"
#include "mcu.h"
#include "gfx.h" // TODO: ???
#include "lcd.h"
#include "adc.h"
#include "key.h"
#include "gui.h"
#include "nvic.h"
#include "event.h"
#include "timer.h"

static const uint8_t ARIAL[] =
{
    38, // Height
    // +
    43, // Symbol +
    17, // Width
    0xD5, 0x00, // Size 213
    227, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 
    1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 
    1, 0x70, 0x00, 6, 0x00, 0x00, 1, 0x07, 0x17, 13, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x00, 0x00, 1, 0x07, 0x17, 
    13, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x00, 0x00, 1, 0x07, 0x17, 13, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 
    1, 0x00, 0x00, 1, 0x07, 0x17, 13, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 6, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 
    1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 
    1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 192, 0x00, 0x00, 
    // -
    45, // Symbol -
    9, // Width
    0x2A, 0x00, // Size 42
    189, 0x00, 0x00, 1, 0x07, 0x17, 7, 0xFF, 0xDF, 1, 0xBF, 0x00, 1, 0x07, 0x17, 7, 0xFF, 0xDF, 1, 0xBF, 0x00, 1, 0x07, 0x17, 
    7, 0xFF, 0xDF, 1, 0xBF, 0x00, 1, 0x07, 0x17, 7, 0xFF, 0xDF, 1, 0xBF, 0x00, 117, 0x00, 0x00, 
    // 0
    48, // Symbol 0
    18, // Width
    0xC8, 0x01, // Size 456
    185, 0x00, 0x00, 1, 0x07, 0x17, 6, 0xFF, 0xDF, 1, 0xBF, 0x00, 8, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 8, 0xFF, 0xDF, 
    1, 0xFF, 0xDB, 1, 0x94, 0x40, 5, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 10, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 
    3, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 12, 0xFF, 0xDF, 1, 0xFB, 0xCE, 3, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xD9, 0x88, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 2, 0x00, 0x00, 
    1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 4, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xFB, 0xCE, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x49, 0x9B, 
    4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 4, 0x00, 0x00, 
    1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 
    4, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x00, 
    1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 2, 0x00, 0x00, 
    1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 4, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 2, 0x00, 0x00, 
    1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 3, 0x00, 0x00, 1, 0x7B, 0xDF, 12, 0xFF, 0xDF, 1, 0xFB, 0xCE, 4, 0x00, 0x00, 
    1, 0x00, 0x0E, 1, 0xBF, 0xDF, 10, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 5, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 
    8, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 8, 0x00, 0x00, 1, 0x07, 0x17, 6, 0xFF, 0xDF, 1, 0xBF, 0x00, 149, 0x00, 0x00, 

    // 1
    49, // Symbol 1
    14, // Width
    0x3E, 0x01, // Size 318
    148, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x49, 0x9B, 5, 0xFF, 0xDF, 1, 0xBF, 0x00, 6, 0x00, 0x00, 1, 0x49, 0x9B, 6, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 5, 0x00, 0x00, 1, 0x7B, 0xDF, 7, 0xFF, 0xDF, 1, 0xBF, 0x00, 3, 0x00, 0x00, 1, 0x49, 0x9B, 9, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 10, 0xFF, 0xDF, 1, 0xBF, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x08, 
    1, 0x9D, 0xDF, 10, 0xFF, 0xDF, 1, 0xBF, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 2, 0xFF, 0xDF, 1, 0xFF, 0xDB, 
    1, 0x94, 0x40, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 2, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 
    1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x04, 0x52, 
    1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 
    7, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 
    1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 7, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 112, 0x00, 0x00, 
    // 2
    50, // Symbol 2
    18, // Width
    0x53, 0x01, // Size 339
    184, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 6, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 7, 0x00, 0x00, 1, 0x7B, 0xDF, 
    10, 0xFF, 0xDF, 1, 0xFB, 0xCE, 5, 0x00, 0x00, 1, 0x49, 0x9B, 12, 0xFF, 0xDF, 1, 0xFB, 0xCE, 3, 0x00, 0x00, 1, 0x04, 0x52, 
    1, 0xDF, 0xDF, 12, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 2, 0x00, 0x00, 1, 0x49, 0x9B, 5, 0xFF, 0xDF, 1, 0xD9, 0x88, 
    2, 0x00, 0x00, 1, 0x49, 0x9B, 5, 0xFF, 0xDF, 1, 0xD9, 0x88, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 
    1, 0x40, 0x00, 3, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 11, 0x00, 0x00, 1, 0x00, 0x0E, 
    1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 11, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 
    9, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 5, 0xFF, 0xDF, 1, 0xFB, 0xCE, 9, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 
    5, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 8, 0x00, 0x00, 1, 0x07, 0x17, 6, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 
    8, 0x00, 0x00, 1, 0x49, 0x9B, 6, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 8, 0x00, 0x00, 1, 0x7B, 0xDF, 6, 0xFF, 0xDF, 
    1, 0xD9, 0x88, 9, 0x00, 0x00, 1, 0x7B, 0xDF, 5, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 9, 0x00, 0x00, 1, 0x49, 0x9B, 
    5, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 9, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 13, 0xFF, 0xDF, 1, 0xFD, 0xD2, 
    1, 0x40, 0x00, 1, 0x00, 0x00, 1, 0x49, 0x9B, 14, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 
    14, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 14, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    144, 0x00, 0x00, 
    // 3
    51, // Symbol 3
    18, // Width
    0x56, 0x01, // Size 342
    185, 0x00, 0x00, 1, 0x7B, 0xDF, 6, 0xFF, 0xDF, 1, 0xD9, 0x88, 8, 0x00, 0x00, 1, 0x49, 0x9B, 10, 0xFF, 0xDF, 1, 0xD9, 0x88, 
    5, 0x00, 0x00, 1, 0x49, 0x9B, 12, 0xFF, 0xDF, 1, 0xBF, 0x00, 3, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 12, 0xFF, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 2, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 
    1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x44, 0x48, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 
    1, 0xD9, 0x88, 3, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 10, 0x00, 0x00, 1, 0x00, 0x0E, 
    1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 9, 0x00, 0x00, 1, 0x49, 0x9B, 6, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    9, 0x00, 0x00, 1, 0x7B, 0xDF, 5, 0xFF, 0xDF, 1, 0xD9, 0x88, 11, 0x00, 0x00, 1, 0x7B, 0xDF, 6, 0xFF, 0xDF, 1, 0xFF, 0xDB, 
    1, 0x94, 0x40, 8, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 7, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 11, 0x00, 0x00, 
    1, 0x07, 0x17, 5, 0xFF, 0xDF, 1, 0xFB, 0xCE, 12, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    11, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x00, 1, 0x7B, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 2, 0x00, 0x00, 1, 0x07, 0x17, 5, 0xFF, 0xDF, 1, 0xD9, 0x88, 2, 0x00, 0x00, 
    1, 0x04, 0x52, 1, 0xDF, 0xDF, 12, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 3, 0x00, 0x00, 1, 0x49, 0x9B, 12, 0xFF, 0xDF, 
    1, 0xD9, 0x88, 5, 0x00, 0x00, 1, 0x49, 0x9B, 10, 0xFF, 0xDF, 1, 0xD9, 0x88, 8, 0x00, 0x00, 1, 0x7B, 0xDF, 6, 0xFF, 0xDF, 
    1, 0xFB, 0xCE, 149, 0x00, 0x00, 
    // 4
    52, // Symbol 4
    18, // Width
    0x71, 0x01, // Size 369
    189, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 10, 0x00, 0x00, 1, 0x00, 0x08, 
    1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 10, 0x00, 0x00, 1, 0x7B, 0xDF, 5, 0xFF, 0xDF, 1, 0xFF, 0xD7, 
    1, 0x70, 0x00, 9, 0x00, 0x00, 1, 0x7B, 0xDF, 6, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 8, 0x00, 0x00, 1, 0x49, 0x9B, 
    7, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 7, 0x00, 0x00, 1, 0x07, 0x17, 8, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 
    6, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x9B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 
    1, 0x70, 0x00, 5, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x7B, 0xDF, 
    3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 4, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xDB, 
    1, 0x94, 0x40, 1, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 4, 0x00, 0x00, 1, 0x7B, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 3, 0x00, 0x00, 
    1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 3, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 
    2, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 4, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 
    1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x49, 0x9B, 16, 0xFF, 0xDF, 1, 0xD9, 0x88, 1, 0x49, 0x9B, 16, 0xFF, 0xDF, 1, 0xD9, 0x88, 
    1, 0x49, 0x9B, 16, 0xFF, 0xDF, 1, 0xD9, 0x88, 1, 0x49, 0x9B, 16, 0xFF, 0xDF, 1, 0xD9, 0x88, 10, 0x00, 0x00, 1, 0x7B, 0xDF, 
    3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 12, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 
    12, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 12, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 146, 0x00, 0x00, 
    // 5
    53, // Symbol 5
    18, // Width
    0x65, 0x01, // Size 357
    182, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 11, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 3, 0x00, 0x00, 1, 0x00, 0x0E, 
    1, 0xBF, 0xDF, 11, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 3, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 11, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 3, 0x00, 0x00, 1, 0x07, 0x17, 12, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 3, 0x00, 0x00, 
    1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 12, 0x00, 0x00, 1, 0x7B, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 
    11, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 
    1, 0xFF, 0xDB, 1, 0x94, 0x40, 5, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 11, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    3, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 12, 0xFF, 0xDF, 1, 0xFB, 0xCE, 3, 0x00, 0x00, 1, 0x07, 0x17, 14, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x07, 0x17, 
    5, 0xFF, 0xDF, 1, 0xFB, 0xCE, 12, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 11, 0x00, 0x00, 
    1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 
    1, 0x40, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 4, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xFB, 0xCE, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 2, 0x00, 0x00, 
    1, 0x49, 0x9B, 5, 0xFF, 0xDF, 1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x07, 0x17, 13, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    3, 0x00, 0x00, 1, 0x7B, 0xDF, 11, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 5, 0x00, 0x00, 1, 0x7B, 0xDF, 9, 0xFF, 0xDF, 
    1, 0xFF, 0xDB, 1, 0x94, 0x40, 8, 0x00, 0x00, 1, 0x7B, 0xDF, 6, 0xFF, 0xDF, 1, 0xBF, 0x00, 149, 0x00, 0x00, 
    // 6
    54, // Symbol 6
    18, // Width
    0xB6, 0x01, // Size 438
    186, 0x00, 0x00, 1, 0x7B, 0xDF, 5, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 8, 0x00, 0x00, 1, 0x7B, 0xDF, 9, 0xFF, 0xDF, 
    1, 0xD9, 0x88, 6, 0x00, 0x00, 1, 0x7B, 0xDF, 11, 0xFF, 0xDF, 1, 0xBF, 0x00, 4, 0x00, 0x00, 1, 0x49, 0x9B, 12, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    1, 0x00, 0x00, 1, 0x07, 0x17, 5, 0xFF, 0xDF, 1, 0xD9, 0x88, 2, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFF, 0xDB, 
    1, 0x94, 0x40, 2, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 3, 0xFF, 0xDF, 1, 0xD9, 0x88, 3, 0x00, 0x00, 1, 0x49, 0x9B, 
    4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 11, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 1, 0x00, 0x00, 
    1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 4, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x8E, 
    1, 0x9D, 0xDF, 6, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 2, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 13, 0xFF, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 14, 0xFF, 0xDF, 1, 0xD9, 0x88, 1, 0x00, 0x00, 
    1, 0x00, 0x08, 1, 0x9D, 0xDF, 5, 0xFF, 0xDF, 1, 0xD9, 0x88, 2, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 3, 0x00, 0x00, 
    1, 0x04, 0x52, 1, 0xDF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 
    1, 0xFB, 0xCE, 4, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x00, 0x00, 
    1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 3, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 3, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 2, 0x00, 0x00, 
    1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 2, 0x00, 0x00, 1, 0x49, 0x9B, 13, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 3, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 11, 0xFF, 0xDF, 1, 0xFB, 0xCE, 5, 0x00, 0x00, 1, 0x00, 0x08, 
    1, 0x9D, 0xDF, 9, 0xFF, 0xDF, 1, 0xFB, 0xCE, 8, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 5, 0xFF, 0xDF, 1, 0xFF, 0xD7, 
    1, 0x70, 0x00, 148, 0x00, 0x00, 
    // 7
    55, // Symbol 7
    18, // Width
    0x32, 0x01, // Size 306
    180, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 14, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 
    14, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 14, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    1, 0x00, 0x08, 1, 0x9D, 0xDF, 14, 0xFF, 0xDF, 1, 0xD9, 0x88, 11, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 
    11, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 10, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 
    3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 11, 0x00, 0x00, 
    1, 0x04, 0x52, 1, 0xDF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 11, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 11, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 11, 0x00, 0x00, 
    1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 11, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xDB, 
    1, 0x94, 0x40, 11, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 11, 0x00, 0x00, 1, 0x49, 0x9B, 
    4, 0xFF, 0xDF, 1, 0xD9, 0x88, 11, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 11, 0x00, 0x00, 
    1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 11, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 
    3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 11, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    11, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 151, 0x00, 0x00, 
    // 8
    56, // Symbol 8
    18, // Width
    0x89, 0x01, // Size 393
    185, 0x00, 0x00, 1, 0x7B, 0xDF, 6, 0xFF, 0xDF, 1, 0xD9, 0x88, 8, 0x00, 0x00, 1, 0x49, 0x9B, 10, 0xFF, 0xDF, 1, 0xD9, 0x88, 
    5, 0x00, 0x00, 1, 0x49, 0x9B, 12, 0xFF, 0xDF, 1, 0xD9, 0x88, 3, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 12, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 2, 0x00, 0x00, 
    1, 0x07, 0x17, 4, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 2, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 3, 0xFF, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 2, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 
    2, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x07, 0x17, 4, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 3, 0x00, 0x00, 1, 0x49, 0x9B, 12, 0xFF, 0xDF, 1, 0xD9, 0x88, 5, 0x00, 0x00, 1, 0x49, 0x9B, 
    10, 0xFF, 0xDF, 1, 0xD9, 0x88, 6, 0x00, 0x00, 1, 0x7B, 0xDF, 10, 0xFF, 0xDF, 1, 0xFB, 0xCE, 4, 0x00, 0x00, 1, 0x00, 0x08, 
    1, 0x9D, 0xDF, 12, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 2, 0x00, 0x00, 1, 0x49, 0x9B, 5, 0xFF, 0xDF, 1, 0xBF, 0x00, 
    2, 0x00, 0x00, 1, 0x49, 0x9B, 5, 0xFF, 0xDF, 1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 
    4, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x49, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 4, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x00, 
    1, 0x7B, 0xDF, 5, 0xFF, 0xDF, 1, 0xD9, 0x88, 2, 0x00, 0x00, 1, 0x49, 0x9B, 5, 0xFF, 0xDF, 1, 0xFB, 0xCE, 2, 0x00, 0x00, 
    1, 0x07, 0x17, 14, 0xFF, 0xDF, 1, 0xBF, 0x00, 3, 0x00, 0x00, 1, 0x7B, 0xDF, 12, 0xFF, 0xDF, 1, 0xFB, 0xCE, 4, 0x00, 0x00, 
    1, 0x00, 0x08, 1, 0x9D, 0xDF, 10, 0xFF, 0xDF, 1, 0xFB, 0xCE, 7, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 6, 0xFF, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 148, 0x00, 0x00, 
    // 9
    57, // Symbol 9
    18, // Width
    0xB9, 0x01, // Size 441
    184, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 5, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 8, 0x00, 0x00, 1, 0x7B, 0xDF, 
    9, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 5, 0x00, 0x00, 1, 0x7B, 0xDF, 11, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    3, 0x00, 0x00, 1, 0x07, 0x17, 13, 0xFF, 0xDF, 1, 0xD9, 0x88, 2, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 
    1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 1, 0x00, 0x00, 
    1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xDB, 1, 0x94, 0x40, 3, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 
    4, 0xFF, 0xDF, 1, 0xBF, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 
    4, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 1, 0x00, 0x00, 1, 0x00, 0x0E, 1, 0xBF, 0xDF, 3, 0xFF, 0xDF, 
    1, 0xFF, 0xDB, 1, 0x94, 0x40, 3, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 1, 0x00, 0x00, 
    1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 1, 0x7B, 0xDF, 5, 0xFF, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x00, 1, 0x49, 0x9B, 14, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 1, 0x00, 0x00, 
    1, 0x00, 0x08, 1, 0x9D, 0xDF, 13, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 2, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 
    6, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x79, 0x9B, 4, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 4, 0x00, 0x00, 1, 0x07, 0x17, 
    4, 0xFF, 0xDF, 1, 0xD9, 0x88, 1, 0x00, 0x00, 1, 0x7B, 0xDF, 4, 0xFF, 0xDF, 1, 0xFB, 0xCE, 11, 0x00, 0x00, 1, 0x00, 0x08, 
    1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xD9, 0x88, 3, 0x00, 0x00, 1, 0x07, 0x17, 3, 0xFF, 0xDF, 1, 0xFD, 0xD2, 1, 0x40, 0x00, 
    2, 0x00, 0x00, 1, 0x04, 0x52, 1, 0xDF, 0xDF, 4, 0xFF, 0xDF, 1, 0xBF, 0x00, 2, 0x00, 0x00, 1, 0x49, 0x9B, 5, 0xFF, 0xDF, 
    1, 0xBF, 0x00, 1, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 4, 0xFF, 0xDF, 1, 0xFF, 0xD7, 1, 0x70, 0x00, 2, 0x00, 0x00, 
    1, 0x00, 0x0E, 1, 0xBF, 0xDF, 12, 0xFF, 0xDF, 1, 0xD9, 0x88, 4, 0x00, 0x00, 1, 0x07, 0x17, 11, 0xFF, 0xDF, 1, 0xFB, 0xCE, 
    6, 0x00, 0x00, 1, 0x49, 0x9B, 9, 0xFF, 0xDF, 1, 0xFB, 0xCE, 8, 0x00, 0x00, 1, 0x00, 0x08, 1, 0x9D, 0xDF, 5, 0xFF, 0xDF, 
    1, 0xFD, 0xD2, 1, 0x40, 0x00, 149, 0x00, 0x00, 
    // EOF
    0, 0, 0, 0
};

int32_t last = 0;

static timer_callback_t adc_timer([](void)
{
    char text[32];
    auto adc = adc_read();
    
    adc /= 100;
    
    if (adc > 0)
        sprintf(text, "+%d", adc);
    else
        sprintf(text, "%d", adc);
    
    if (last != adc)
    {
        gfx_rect_solid(LCD_COLOR_BLACK, 10, 10, 140, 38);
        gfx_font_string(text, 10, 10, ARIAL);
        last = adc;
    }
});

// Точка входа в приложение
__task __noreturn void main(void)
{
    // Системные модули (порядок не менять)
    nvic_init();
    mcu_init();
    io_init();
    timer_init();
    // Остальные модули
    key_init();
    adc_init();
    lcd_init();
    gui_init();
    
    // Тест
    
    gfx_rect_solid(LCD_COLOR_BLACK, 0, 0, LCD_SIZE_WIDTH, LCD_SIZE_HEIGHT);

    gfx_rect_solid(LCD_COLOR_RED, 10, 60, 20, 20);

    gfx_rect_solid(LCD_COLOR_GREEN, 40, 60, 20, 20);

    gfx_rect_solid(LCD_COLOR_BLUE, 70, 60, 20, 20);
    
    adc_timer.start_us(100, TIMER_FLAG_LOOP);
    
    // Главный цикл событий
    event_t::loop();
}
