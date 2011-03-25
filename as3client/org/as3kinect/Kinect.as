/*
 * This file is part of the AS3Kinect Project. http://www.Kinect.org
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
	import flash.events.EventDispatcher;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	
	import org.as3kinect.events.KinectEvent;
	import org.as3kinect.events.KinectSocketEvent;

	public class Kinect extends EventDispatcher
	{
		public static const SUCCESS : int = 0;
		public static const ERROR : int = -1;
		
		public static const SERVER_IP : String = "localhost";
		public static const SOCKET_PORT : int = 6001;
		
		public static const CAMERA_ID : int = 0;
		public static const MOTOR_ID : int = 1;
		public static const MIC_ID : int = 2;
		
		public static const GET_DEPTH : int = 0;
		public static const GET_RGB : int = 1;
		public static const GET_SKEL : int = 2;
		public static const GET_DEPTH_IMAGE : int = 3;
		public static const UPDATE_USER_GENERATOR : int = 4;
		
		public static const IMG_WIDTH : int = 640;
		public static const IMG_HEIGHT : int = 480;
		
		public static const RAW_IMG_SIZE : int = IMG_WIDTH * IMG_HEIGHT * 4;
		public static const DATA_IN_SIZE : int = 3 * 2 + 3 * 8;
		public static const COMMAND_SIZE : int = 8;		
		
		private var socket 			: KinectSocket;
		private var data 			: ByteArray;
		private var userId 			: Number;

		public var depthBuffer 		: DepthBuffer;
		public var depthBitmapBuffer: DepthBitmapBuffer;
		public var skeletonBuffer 	: SkeletonBuffer;
		public var colorBuffer		: ColorBuffer;
		public var userBuffer		: UserBuffer;

		public function Kinect()
		{
			socket = new KinectSocket();
			socket.connect( Kinect.SERVER_IP, Kinect.SOCKET_PORT );
			socket.addEventListener( KinectSocketEvent.ON_DATA, dataReceivedHandler );

			depthBuffer = new DepthBuffer( socket );
			depthBitmapBuffer = new DepthBitmapBuffer( socket );
			skeletonBuffer = new SkeletonBuffer( socket );
			colorBuffer = new ColorBuffer( socket );
			userBuffer = new UserBuffer( socket );

			data = new ByteArray();
		}
		
		public function destroy() : void
		{
			socket.removeEventListener( KinectSocketEvent.ON_DATA, dataReceivedHandler );
			socket.destroy();
			socket = null;
		}

		/*
		 * dataReceived from socket (Protocol definition)
		 * Metadata comes in the first and second value of the data object
		 * first:
		 *	0 -> Camera data
		 * 			second:
		 *  			0 -> Depth ARGB received
		 *  			1 -> Video ARGB received
		 *  			2 -> Skeleton data received
		 *	1 -> Motor data
		 *	2 -> Microphone data
		 *	3 -> Server data
		 * 			second:
		 *  			0 -> Debug info received
		 *  			1 -> Got user
		 *  			2 -> Lost user
		 *  			3 -> Pose detected for user
		 *  			4 -> Calibrating user
		 *  			5 -> Calibration complete for user
		 *  			6 -> Calibration failed for user
		 *
		 */
		private function dataReceivedHandler( event : KinectSocketEvent ) : void
		{
			// Send ByteArray to position 0
			event.data.buffer.position = 0;

			switch ( event.data.first )
			{
				case 0: //Camera
					switch ( event.data.second )
					{
						case 0: //Depth received
							var depthEvent : KinectEvent = new KinectEvent( KinectEvent.ON_DEPTH );
							depthEvent.depthBuffer = event.data.buffer;
							dispatchEvent( depthEvent );
							depthBuffer.busy = false;
							break;
						case 1: //Video received
							var colorEvent : KinectEvent = new KinectEvent( KinectEvent.ON_COLOR );
							colorEvent.colorBuffer = event.data.buffer;
							dispatchEvent( colorEvent );
							colorBuffer.busy = false;
							break;
						case 2: //SKEL received
							var skelEvent : KinectEvent = new KinectEvent( KinectEvent.ON_SKELETON );
							skeletonBuffer.processSkeleton( event.data.buffer );
							skelEvent.skeletons = skeletonBuffer.skeletons;
							dispatchEvent( skelEvent );
							skeletonBuffer.busy = false;
							break;
						case 3: //Depth image received
							var depthImageEvent : KinectEvent = new KinectEvent( KinectEvent.ON_DEPTH_BITMAP );
							depthImageEvent.DepthBitmapBuffer = event.data.buffer;
							dispatchEvent( depthImageEvent );
							depthBitmapBuffer.busy = false;
							break;
					}
					break;
				case 1: //Motor
					break;
				case 2: //Mic
					break;
				case 3: //Server
					switch ( event.data.second )
					{
						case 0: //Debug received
							CONFIG::debugging
							{
								Logger.log( Logger.DEBUG, event.data.buffer.toString());
							}
							break;
						case 1: //Got user
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								Logger.log( Logger.DEBUG, "Got user: " + userId );
							}
							break;
						case 2: //Lost user
							userId = event.data.buffer.readInt();
							skeletonBuffer.trackedUsers.pop();
							CONFIG::debugging
							{
								Logger.log( Logger.DEBUG, "Lost user: " + userId );
							}
							break;
						case 3: //Pose detected
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								Logger.log( Logger.DEBUG, "Pose detected for user: " + userId );
							}
							break;
						case 4: //Calibrating
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								Logger.log( Logger.DEBUG, "Calibrating user: " + userId );
							}
							break;
						case 5: //Calibration complete
							userId = event.data.buffer.readInt();
							skeletonBuffer.trackedUsers.push( userId );
							CONFIG::debugging
							{
								Logger.log( Logger.DEBUG, "Calibration complete for user: " + userId );
							}
							break;
						case 6: //Calibration failed
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								Logger.log( Logger.DEBUG, "Calibration failed for user: " + userId );
							}
							break;
					}
					break;
			}
			// Clear ByteArray after used
			event.data.buffer.clear();
		}

		/*
		 * Enable log console on TextField
		 */
		public function set logConsole( txt : TextField ) : void
		{
			Logger.textField = txt;
		}
	}
}
