/*
 * This file is part of the AS3Kinect Project. http://www.AS3Kinect.org
 *
 * Copyright (c) 2010 individual AS3Kinect contributors. See the CONTRIB file
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

package org.as3kinect
{
	import flash.utils.ByteArray;

	public class AS3KinectDepth extends AS3KinectDataBuffer
	{
		public function AS3KinectDepth( socket : AS3KinectSocket )
		{
			super( socket );
		}

		/*
		 * Tell server to send the latest depth frame
		 * Note: We should lock the command while we are waiting for the data to avoid lag
		 */
		override public function update() : void
		{
			super.update();
			data.clear();
			data.writeByte( AS3Kinect.CAMERA_ID );
			data.writeByte( AS3Kinect.GET_DEPTH );
			data.writeInt( 0 );
			if ( socket.sendCommand( data ) != AS3Kinect.SUCCESS )
			{
				throw new Error( 'Data was not complete' );
			}
		}
	}
}
