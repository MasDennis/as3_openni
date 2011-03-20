package
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	
	import org.as3kinect.AS3KinectWrapper;
	import org.as3kinect.events.AS3KinectWrapperEvent;
	
	[SWF(width="640", height="480", frameRate="30")]
	
	public class DepthBuffer extends Sprite
	{
		private var wrapper : AS3KinectWrapper;
		private var bmd : BitmapData;
		
		public function DepthBuffer()
		{
			super();
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			
			bmd = new BitmapData(640, 480, true, 0x00000000 );
			var bm : Bitmap = new Bitmap( bmd );
			addChild( bm );
			
			wrapper = new AS3KinectWrapper();
			wrapper.addEventListener( AS3KinectWrapperEvent.ON_DEPTH_IMAGE, depthImageDataHandler );
			
			addEventListener( Event.ENTER_FRAME, enterFrameHandler );
		}
		
		private function enterFrameHandler( event : Event ) : void
		{
			wrapper.depthImageBuffer.update();
		}

		private function depthImageDataHandler( event : AS3KinectWrapperEvent ) : void
		{
			bmd.setPixels( bmd.rect, event.depthImageBuffer );
		}
	}
}