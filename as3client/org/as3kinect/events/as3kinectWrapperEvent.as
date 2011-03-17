/*
 * This file is part of the as3server Project. http://www.as3server.org
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

package org.as3kinect.events
{
	import flash.events.Event;
	import flash.utils.ByteArray;
	
	import org.as3kinect.objects.Skeleton3D;

	public class AS3KinectWrapperEvent extends Event
	{
		public static const ON_DEPTH 		: String = "onDepth";
		public static const ON_DEPTH_IMAGE 	: String = "onDepthImage";
		public static const ON_DEBUG 		: String = "onDebug";
		public static const ON_SKEL 		: String = "onSkel";
		public static const ON_COLOR		: String = "onColor";

		public var depthBuffer 		: ByteArray;
		public var depthImageBuffer	: ByteArray;
		public var colorBuffer 		: ByteArray;
		public var skeletons 		: Vector.<Skeleton3D>;

		public function AS3KinectWrapperEvent( type : String )
		{
			super( type );
		}
	}
}