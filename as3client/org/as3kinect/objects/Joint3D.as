package org.as3kinect.objects
{
	import flash.utils.ByteArray;

	public class Joint3D
	{
		public var name			: String;
		public var position 	: Point3D;
		public var rotationX 	: Point3D;
		public var rotationY 	: Point3D;
		public var rotationZ 	: Point3D;
		
		public function Joint3D( name : String )
		{
			this.name	= name;
			position 	= new Point3D();
			rotationX 	= new Point3D();
			rotationY 	= new Point3D();
			rotationZ 	= new Point3D();
		}
		
		public function updateFromBytes( ba : ByteArray ) : void
		{
			position.x = ba.readFloat();
			position.y = ba.readFloat();
			position.z = ba.readFloat();
			
			rotationX.x = ba.readFloat();
			rotationX.y = ba.readFloat();
			rotationX.z = ba.readFloat();
			
			rotationY.x = ba.readFloat();
			rotationY.y = ba.readFloat();
			rotationY.z = ba.readFloat();

			rotationZ.x = ba.readFloat();
			rotationZ.y = ba.readFloat();
			rotationZ.z = ba.readFloat();
		}
	}
}