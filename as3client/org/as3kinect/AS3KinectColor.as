package org.as3kinect
{
	public class AS3KinectColor extends AS3KinectDataBuffer
	{
		public function AS3KinectColor(socket:AS3KinectSocket)
		{
			super(socket);
		}
		
		override public function update() : void
		{
			super.update();
			data.clear();
			data.writeByte( AS3Kinect.CAMERA_ID );
			data.writeByte( AS3Kinect.GET_RGB );
			data.writeInt( 0 );
			if ( socket.sendCommand( data ) != AS3Kinect.SUCCESS )
			{
				throw new Error( 'Data was not complete' );
			}
		}
	}
}