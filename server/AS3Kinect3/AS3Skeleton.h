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

#pragma once

class As3Skeleton {
  	public:
		enum parts {
			USER_ID,
			HEAD,
			NECK,
			TORSO,
	
			L_SHOULDER,
			L_ELBOW,
			L_HAND,
			L_HIP,
			L_KNEE,
			L_FOOT,

			R_SHOULDER,
			R_ELBOW,
			R_HAND,
			R_HIP,
			R_KNEE,
			R_FOOT
		};

		int size;
		unsigned char *skel;
		bool isTracking;
		
		As3Skeleton();
		~As3Skeleton();
		void setValues(const unsigned int id, const float x, const float y, const float z,
			const float xrotx, const float xroty, const float xrotz,
			const float yrotx, const float yroty, const float yrotz,
			const float zrotx, const float zroty, const float zrotz);
		void setUserId(const int userId);
		void debug(unsigned char *s);
	private:
		static const int OFFSET = (3 * sizeof(float)) * 4;
		//
};