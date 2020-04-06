/*
 * rgbcolor.cpp
 *
 *  Created on: Apr 4, 2020
 *      Author: dcomes
 */

#include "config.h"
#include "rgbcolor.h"

using namespace cmdc0de;

const RGB RGB::WHITE(255,255,255,100);
const RGB RGB::BLUE(0,0,255,100);
const RGB RGB::GREEN(0,255,0,100);
const RGB RGB::RED(255,0,0,100);



const RGBColor RGBColor::BLACK(0, 0, 0);
const RGBColor RGBColor::RED(255, 0, 0);
const RGBColor RGBColor::GREEN(0, 255, 0);
const RGBColor RGBColor::BLUE(0, 0, 255);
const RGBColor RGBColor::WHITE(255, 255, 255);

bool RGBColor::operator==(const RGBColor &r) const {
	return (r.R == R && r.G == G && r.B == B);
}

bool RGBColor::operator!=(const RGBColor &r) const {
	return !((*this) == r);
}
