package org.as3kinect
{
	public class DepthBitmapBuffer extends KinectDataBuffer
	{
		public function DepthBitmapBuffer(socket:KinectSocket)
		{
			super(socket);
		}
		
		override public function update() : void
		{
			super.update();
			data.clear();
			data.writeByte( Kinect.CAMERA_ID );
			data.writeByte( Kinect.GET_DEPTH_IMAGE );
			data.writeInt( 0 );
			data.writeShort( 0 );
			if ( socket.sendCommand( data ) != Kinect.SUCCESS )
			{
				trace( 'Data was not complete' );
			}
		}
	}
}