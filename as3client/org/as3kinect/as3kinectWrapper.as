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
	import flash.events.EventDispatcher;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	
	import org.as3kinect.events.AS3KinectSocketEvent;
	import org.as3kinect.events.AS3KinectWrapperEvent;

	public class AS3KinectWrapper extends EventDispatcher
	{

		private var socket 			: AS3KinectSocket;
		private var data 			: ByteArray;
		private var userId 			: Number;

		public var depthBuffer 		: AS3KinectDepth;
		public var depthImageBuffer	: AS3KinectDepthImage;
		public var skeletonBuffer 	: AS3KinectSkeleton;
		public var colorBuffer		: AS3KinectColor;

		public function AS3KinectWrapper()
		{
			socket = new AS3KinectSocket();
			socket.connect( AS3Kinect.SERVER_IP, AS3Kinect.SOCKET_PORT );
			socket.addEventListener( AS3KinectSocketEvent.ON_DATA, dataReceivedHandler );

			depthBuffer = new AS3KinectDepth( socket );
			depthImageBuffer = new AS3KinectDepthImage( socket );
			skeletonBuffer = new AS3KinectSkeleton( socket );
			colorBuffer = new AS3KinectColor( socket );

			data = new ByteArray();
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
		private function dataReceivedHandler( event : AS3KinectSocketEvent ) : void
		{
			// Send ByteArray to position 0
			event.data.buffer.position = 0;

			switch ( event.data.first )
			{
				case 0: //Camera
					switch ( event.data.second )
					{
						case 0: //Depth received
							var depthEvent : AS3KinectWrapperEvent = new AS3KinectWrapperEvent( AS3KinectWrapperEvent.ON_DEPTH );
							depthEvent.depthBuffer = event.data.buffer;
							dispatchEvent( depthEvent );
							depthBuffer.busy = false;
							break;
						case 1: //Video received
							var colorEvent : AS3KinectWrapperEvent = new AS3KinectWrapperEvent( AS3KinectWrapperEvent.ON_COLOR );
							colorEvent.colorBuffer = event.data.buffer;
							dispatchEvent( colorEvent );
							colorBuffer.busy = false;
							break;
						case 2: //SKEL received
							var skelEvent : AS3KinectWrapperEvent = new AS3KinectWrapperEvent( AS3KinectWrapperEvent.ON_SKEL );
							skeletonBuffer.processSkeleton( event.data.buffer );
							skelEvent.skeletons = skeletonBuffer.skeletons;
							dispatchEvent( skelEvent );
							skeletonBuffer.busy = false;
							break;
						case 3: //Depth image received
							var depthImageEvent : AS3KinectWrapperEvent = new AS3KinectWrapperEvent( AS3KinectWrapperEvent.ON_DEPTH_IMAGE );
							depthImageEvent.depthImageBuffer = event.data.buffer;
							dispatchEvent( depthImageEvent );
							depthImageBuffer.busy = false;
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
								AS3KinectLogger.log( AS3KinectLogger.DEBUG, event.data.buffer.toString());
							}
							break;
						case 1: //Got user
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								AS3KinectLogger.log( AS3KinectLogger.DEBUG, "Got user: " + userId );
							}
							break;
						case 2: //Lost user
							userId = event.data.buffer.readInt();
							skeletonBuffer.trackedUsers.pop();
							CONFIG::debugging
							{
								AS3KinectLogger.log( AS3KinectLogger.DEBUG, "Lost user: " + userId );
							}
							break;
						case 3: //Pose detected
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								AS3KinectLogger.log( AS3KinectLogger.DEBUG, "Pose detected for user: " + userId );
							}
							break;
						case 4: //Calibrating
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								AS3KinectLogger.log( AS3KinectLogger.DEBUG, "Calibrating user: " + userId );
							}
							break;
						case 5: //Calibration complete
							userId = event.data.buffer.readInt();
							skeletonBuffer.trackedUsers.push( userId );
							CONFIG::debugging
							{
								AS3KinectLogger.log( AS3KinectLogger.DEBUG, "Calibration complete for user: " + userId );
							}
							break;
						case 6: //Calibration failed
							userId = event.data.buffer.readInt();
							CONFIG::debugging
							{
								AS3KinectLogger.log( AS3KinectLogger.DEBUG, "Calibration failed for user: " + userId );
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
			AS3KinectLogger.textField = txt;
		}
	}
}
