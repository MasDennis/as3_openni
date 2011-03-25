package org.as3kinect.data
{
	import flash.utils.ByteArray;
	
	import org.as3kinect.KinectSocket;

	public class KinectDataBuffer
	{
		protected var socket 		: KinectSocket;
		protected var data			: ByteArray;
		protected var _busy			: Boolean;
		
		public function KinectDataBuffer( socket : KinectSocket )
		{
			this.socket = socket;
			this.data = new ByteArray();
			this._busy = false;
		}
		
		public function update() : void
		{
			if( _busy ) return;
			else _busy = true;
		}
		
		public function set busy( value : Boolean ) : void
		{
			_busy = value;
		}
		
		public function get busy() : Boolean
		{
			return _busy;
		}
	}
}