package org.as3kinect
{
	public class UserBuffer extends KinectDataBuffer
	{
		public function UserBuffer(socket:KinectSocket)
		{
			super(socket);
		}
		
		override public function update() : void
		{
			super.update();
			data.clear();
			data.writeByte( Kinect.CAMERA_ID );
			data.writeByte( Kinect.UPDATE_USER_GENERATOR );
			data.writeInt( 0 );
			data.writeShort( 0 );
			if ( socket.sendCommand( data ) != Kinect.SUCCESS )
			{
				trace( 'Data was not complete' );
			}
			
		}
	}
}