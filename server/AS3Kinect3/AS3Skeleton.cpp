/*
 * This file is part of the as3kinect Project. http://www.as3kinect.org
 *
 * Copyright (c) 2010 individual as3server contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

#include "as3Skeleton.h"
#include <iostream>
using namespace std;

As3Skeleton::As3Skeleton() {
	int count = 0;

	this->size		= sizeof(int) + (3 * sizeof(float)) * As3Skeleton::R_FOOT * 4;
	this->skel		= new unsigned char[this->size];
	this->isTracking = FALSE;
}

void As3Skeleton::setValues(const unsigned int id, const float x, const float y, const float z,
			const float xrotx, const float xroty, const float xrotz,
			const float yrotx, const float yroty, const float yrotz,
			const float zrotx, const float zroty, const float zrotz)
{
	int relOffset = sizeof(int) + (OFFSET * (id-1));
	unsigned char *loc = &this->skel[relOffset];
	int count = 0;

	memcpy(loc+count, &x, 4); count += 4;
	memcpy(loc+count, &y, 4); count += 4;
	memcpy(loc+count, &z, 4); count += 4;

	memcpy(loc+count, &xrotx, 4); count += 4;
	memcpy(loc+count, &xroty, 4); count += 4;
	memcpy(loc+count, &xrotz, 4); count += 4;

	memcpy(loc+count, &yrotx, 4); count += 4;
	memcpy(loc+count, &yroty, 4); count += 4;
	memcpy(loc+count, &yrotz, 4); count += 4;

	memcpy(loc+count, &zrotx, 4); count += 4;
	memcpy(loc+count, &zroty, 4); count += 4;
	memcpy(loc+count, &zrotz, 4); count += 4;
}

void As3Skeleton::setUserId(const int userId)
{
	int relOffset = 0;
	unsigned char *loc = &this->skel[relOffset];
	memcpy(loc, &userId, sizeof(int));
}

As3Skeleton::~As3Skeleton() {

}