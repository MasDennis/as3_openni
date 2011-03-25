package org.as3kinect
{
	public class ColorBuffer extends KinectDataBuffer
	{
		public function ColorBuffer(socket:KinectSocket)
		{
			super(socket);
		}
		
		override public function update() : void
		{
			super.update();
			data.clear();
			data.writeByte( Kinect.CAMERA_ID );
			data.writeByte( Kinect.GET_RGB );
			data.writeInt( 0 );
			data.writeShort( 0 );
			if ( socket.sendCommand( data ) != Kinect.SUCCESS )
			{
				throw new Error( 'Data was not complete' );
			}
		}
	}
}