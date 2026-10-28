/**
   Arduino Electronic Safe

   Copyright (C) 2020, Uri Shaked.
   Released under the MIT License.
*/

#ifndef ICONS_H
#define ICONS_H

#include <LiquidCrystal_I2C.h>   // <-- use I2C header

// Our custom icon numbers
#define ICON_LOCKED_CHAR   (byte)0
#define ICON_UNLOCKED_CHAR (byte)1

// This is a standard icon on the LCD1602 character set
#define ICON_RIGHT_ARROW   (byte)126

void init_icons(LiquidCrystal_I2C &lcd);   // <-- signature uses LiquidCrystal_I2C

#endif /* ICONS_H */
