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

package org.as3kinect.objects
{
	import flash.utils.ByteArray;

	public class Skeleton3D
	{
		public static const HEAD 			: String = "head";
		public static const NECK 			: String = "neck";
		public static const TORSO 			: String = "torso";
		
		public static const LEFT_SHOULDER 	: String = "leftShoulder";
		public static const LEFT_ELBOW 		: String = "leftElbow";
		public static const LEFT_HAND 		: String = "leftHand";
		public static const LEFT_HIP		: String = "leftHip";
		public static const LEFT_KNEE 		: String = "leftKnee";
		public static const LEFT_FOOT 		: String = "leftFoot";
		
		public static const RIGHT_SHOULDER 	: String = "rightShoulder";
		public static const RIGHT_ELBOW 	: String = "rightElbow";
		public static const RIGHT_HAND 		: String = "rightHand";
		public static const RIGHT_HIP		: String = "rightHip";
		public static const RIGHT_KNEE 		: String = "rightKnee";
		public static const RIGHT_FOOT 		: String = "rightFoot";
		
		public var userId : uint;
		public var joints : Vector.<Joint3D>;

		public var head : Joint3D;
		public var neck : Joint3D;
		public var torso : Joint3D;

		public var leftShoulder : Joint3D;
		public var leftElbow : Joint3D;
		public var leftHand : Joint3D;
		public var leftHip : Joint3D;
		public var leftKnee : Joint3D;
		public var leftFoot : Joint3D;

		public var rightShoulder : Joint3D;
		public var rightElbow : Joint3D;
		public var rightHand : Joint3D;
		public var rightHip : Joint3D;
		public var rightKnee : Joint3D;
		public var rightFoot : Joint3D;

		public function Skeleton3D() : void
		{
			userId = 0;

			head = new Joint3D( HEAD );
			neck = new Joint3D( NECK );
			torso = new Joint3D( TORSO );

			leftShoulder = new Joint3D( LEFT_SHOULDER );
			leftElbow = new Joint3D( LEFT_ELBOW );
			leftHand = new Joint3D( LEFT_HAND );
			leftHip = new Joint3D( LEFT_HIP );
			leftKnee = new Joint3D( LEFT_KNEE );
			leftFoot = new Joint3D( LEFT_FOOT );

			rightShoulder = new Joint3D( RIGHT_SHOULDER );
			rightElbow = new Joint3D( RIGHT_ELBOW );
			rightHand = new Joint3D( RIGHT_HAND );
			rightHip = new Joint3D( RIGHT_HIP );
			rightKnee = new Joint3D( RIGHT_KNEE );
			rightFoot = new Joint3D( RIGHT_FOOT );
			
			joints = new Vector.<Joint3D>();
			joints.push( head );
			joints.push( neck );
			joints.push( torso );
			
			joints.push( leftShoulder );
			joints.push( leftElbow );
			joints.push( leftHand );
			joints.push( leftHip );
			joints.push( leftKnee );
			joints.push( leftFoot );
			
			joints.push( rightShoulder );
			joints.push( rightElbow );
			joints.push( rightHand );
			joints.push( rightHip );
			joints.push( rightKnee );
			joints.push( rightFoot );
		}

		public function updateFromBytes( ba : ByteArray ) : void
		{
			userId = ba.readInt();

			head.updateFromBytes( ba );
			neck.updateFromBytes( ba );
			torso.updateFromBytes( ba );

			leftShoulder.updateFromBytes( ba );
			leftElbow.updateFromBytes( ba );
			leftHand.updateFromBytes( ba );
			leftHip.updateFromBytes( ba );
			leftKnee.updateFromBytes( ba );
			leftFoot.updateFromBytes( ba );

			rightShoulder.updateFromBytes( ba );
			rightElbow.updateFromBytes( ba );
			rightHand.updateFromBytes( ba );
			rightHip.updateFromBytes( ba );
			rightKnee.updateFromBytes( ba );
			rightFoot.updateFromBytes( ba );
		}
	}
}
