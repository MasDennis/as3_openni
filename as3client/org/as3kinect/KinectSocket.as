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
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.Socket;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	import org.as3kinect.events.KinectSocketEvent;

	/**
	 * KinectSocket class recieves Kinect data from the AS3Kinect driver.
	 */
	public class KinectSocket extends EventDispatcher
	{
		private var firstByte : Number;
		private var secondByte : Number;
		private var packetSize : Number;
		private var socket : Socket;
		private var buffer : ByteArray;
		private var dataObj : Object;
		private var port : Number;

		public function KinectSocket()
		{
			socket = new Socket();
			buffer = new ByteArray();
			dataObj = new Object();

			socket.addEventListener( ProgressEvent.SOCKET_DATA, onSocketData );
			socket.addEventListener( IOErrorEvent.IO_ERROR, onSocketError );
			socket.addEventListener( Event.CONNECT, onSocketConnect );
		}

		public function connect( host : String = 'localhost', port : uint = 6001 ) : void
		{
			port = port;
			packetSize = 0;
			if ( !this.connected )
				socket.connect( host, port );
			else
				dispatchEvent( new KinectSocketEvent( KinectSocketEvent.ON_CONNECT, null ));
		}

		public function get connected() : Boolean
		{
			return socket.connected;
		}

		public function destroy() : void
		{
			socket.close();
			socket.removeEventListener( ProgressEvent.SOCKET_DATA, onSocketData );
			socket.removeEventListener( IOErrorEvent.IO_ERROR, onSocketError );
			socket.removeEventListener( Event.CONNECT, onSocketConnect );
			socket = null;
		}

		public function sendCommand( data : ByteArray ) : int
		{
			if ( data.length == Kinect.COMMAND_SIZE )
			{
				try {
					socket.writeBytes( data, 0, Kinect.COMMAND_SIZE );
					socket.flush();
				} catch( e : Error ) {
					trace("Operation attempted on invalid socket");
					return Kinect.ERROR;
				}
				return Kinect.SUCCESS;
			}
			else
			{
				throw new Error( 'Incorrect data size (' + data.length + '). Expected: ' + Kinect.COMMAND_SIZE );
				return Kinect.ERROR;
			}
		}

		private function onSocketData( event : ProgressEvent ) : void
		{
			if ( socket.bytesAvailable > 0 )
			{
				if ( packetSize == 0 )
				{
					socket.endian = Endian.LITTLE_ENDIAN;
					firstByte = socket.readByte();
					secondByte = socket.readByte();
					packetSize = socket.readInt();
				}
				if ( socket.bytesAvailable >= packetSize && packetSize != 0 )
				{
					socket.readBytes( buffer, 0, packetSize );
					buffer.endian = Endian.LITTLE_ENDIAN;
					buffer.position = 0;
					dataObj.first = firstByte;
					dataObj.second = secondByte;
					dataObj.buffer = buffer;
					packetSize = 0;
					dispatchEvent( new KinectSocketEvent( KinectSocketEvent.ON_DATA, dataObj ));
				}
			}
		}

		private function onSocketError( event : IOErrorEvent ) : void
		{
			dispatchEvent( new KinectSocketEvent( KinectSocketEvent.ON_ERROR, null ));
		}

		private function onSocketConnect( event : Event ) : void
		{
			dispatchEvent( new KinectSocketEvent( KinectSocketEvent.ON_CONNECT, null ));
		}
	}
}