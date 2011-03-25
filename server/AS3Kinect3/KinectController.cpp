#include "KinectController.h"

const XnChar *KinectController::SAMPLE_XML_PATH = "as3server_config.xml";
const int KinectController::MAX_DEPTH = 10000;
const int KinectController::MAX_USERS = 15;
const int KinectController::MAX_PACKET_SIZE = 640*480*4;
const XnFloat KinectController::colors[][3] =
{
	{0,1,1},
	{0,0,1},
	{0,1,0},
	{1,1,0},
	{1,0,0},
	{1,.5,0},
	{.5,1,0},
	{0,.5,1},
	{.5,0,1},
	{1,1,.5},
	{1,1,1}
};
const int KinectController::NUM_COLORS = 10;

KinectController::KinectController(void)
{
	needPose = FALSE;
	drawBackground = TRUE;
	drawPixels = TRUE;
	drawSkeleton = TRUE;
	msg = new unsigned char[MAX_PACKET_SIZE];
	ready = FALSE;
	skeletons = new As3Skeleton[MAX_USERS];
}

KinectController::~KinectController(void)
{
	context.Shutdown();
}

int KinectController::initialise() {
	printf("[+] Initialising Kinect\n");
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = context.InitFromXmlFile(SAMPLE_XML_PATH);
	CHECK_RC(nRetVal, "InitFromXml");
	printf("    [+] Xml config file found\n");

	nRetVal = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator);
	CHECK_RC(nRetVal, "Find depth generator");
	printf("	[+] Depth generator node found\n");

	nRetVal = context.FindExistingNode(XN_NODE_TYPE_IMAGE, imageGenerator);
	CHECK_RC(nRetVal, "Find image generator");
	printf("	[+] Image generator node found\n");
	
	nRetVal = context.FindExistingNode(XN_NODE_TYPE_GESTURE, gestureGenerator);
	CHECK_RC(nRetVal, "Find gesture generator");
	printf("	[+] Gesture generator node found\n");

	nRetVal = context.FindExistingNode(XN_NODE_TYPE_HANDS, handsGenerator);
	CHECK_RC(nRetVal, "Find hands generator");
	printf("	[+] Hands generator node found\n");
	
	nRetVal = context.FindExistingNode(XN_NODE_TYPE_USER, userGenerator);
	CHECK_RC(nRetVal, "Find user generator");
	printf("	[+] User generator node found\n");

	XnCallbackHandle hHandsCallbacks, hGestureCallbacks, hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
	if (!userGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON) ||
		!userGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
	{
		printf("User generator doesn't support either skeleton or pose detection.\n");
		return XN_STATUS_ERROR;
	}

	imageGenerator.SetPixelFormat(XN_PIXEL_FORMAT_RGB24);

	gestureGenerator.RegisterGestureCallbacks(Gesture_Recognized, Gesture_Progress, this, hGestureCallbacks);
	handsGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, this, hHandsCallbacks); 

	if (userGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		needPose = TRUE;
		userGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPose_PoseDetected, NULL, this, hPoseCallbacks);
		//userGenerator.GetSkeletonCap().GetCalibrationPose(strPose);
	}

	userGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	userGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, this, hUserCallbacks);
	userGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(UserCalibration_CalibrationStart, UserCalibration_CalibrationEnd, this, hCalibrationCallbacks);

	nRetVal = context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");

	//addGestures();

	nRetVal = xnFPSInit(&xnFPS, 30);
	CHECK_RC(nRetVal, "FPS Init");

	ready = true;
	printf("	[+] Kinect Initialized\n\n");

	return 0;
}

void KinectController::stop()
{
	context.StopGeneratingAll();
}

void KinectController::resume()
{
	context.StartGeneratingAll();
}

void KinectController::update()
{
	xnFPSMarkFrame(&xnFPS);
}

void KinectController::shutdown()
{
	context.Shutdown();
}

XnBool KinectController::isReady()
{
	return ready;
}

void KinectController::setClientSocket(SOCKET &clientSocket)
{
	this->clientSocket = clientSocket;
}

int KinectController::sendData(unsigned char *buffer, int length){
	if(clientSocket != INVALID_SOCKET) {
		return send(clientSocket, (char*)buffer, length, 0);
	}
	return 0;
}

void KinectController::sendMessage(const char *data) {
	if(clientSocket != INVALID_SOCKET) {
		std::string _msg = data;
		int len = sizeof(_msg);
		int first = 3;
		int second = 0;
		int m_len = 1 + 1 + sizeof(int);
		memcpy(msg, &first, 1);
		memcpy(msg + 1, &second, 1);
		memcpy(msg + 2, &len, sizeof(int));
		memcpy(msg + m_len, data, len);
		sendData(msg,len+m_len);
	}
}

void KinectController::sendMessage(int first, int second, int value) {
	if(clientSocket != INVALID_SOCKET) {
		unsigned char buff[2 + sizeof(int) * 2];
		buff[0] = first;
		buff[1] = second;
		int size = sizeof(int);
		memcpy(buff+2, &size, sizeof(int));
		memcpy(buff+6, &value, sizeof(int));
		sendData(buff, 2 + sizeof(int) * 2);
	}
}

void KinectController::getJointData(XnUserID player, XnSkeletonJoint eJoint1, const int &skelId, const int &jointId)
{
	if (!userGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return;
	}
	XnSkeletonJointPosition joint1;
	XnSkeletonJointOrientation orient1;
	userGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(player, eJoint1, orient1);

	if (joint1.fConfidence < 0.5)
	{
		return;
	}
	XnPoint3D pt[1];
	pt[0] = joint1.position;
	
	float _x, _y, _z;
	_x = pt[0].X;
	_y = pt[0].Y;
	_z = pt[0].Z;

	skeletons[skelId].setValues(jointId, 
		_x, _y, _z,
		orient1.orientation.elements[0], orient1.orientation.elements[1], orient1.orientation.elements[2],
		orient1.orientation.elements[3], orient1.orientation.elements[4], orient1.orientation.elements[5],
		orient1.orientation.elements[6], orient1.orientation.elements[7], orient1.orientation.elements[8]
	);
}

void KinectController::getColorBuffer(unsigned char* imgBuffer)
{
	ImageMetaData imd;

	imageGenerator.GetMetaData(imd);
	imageGenerator.WaitAndUpdateData();

	if(imd.IsDataNew())
	{
		int cols = imd.XRes();
		int rows = imd.YRes();
		int len = cols * rows;
		const XnRGB24Pixel* rgbImage = imd.RGB24Data();

		for(int i=0; i<len; ++i)
		{
			imgBuffer[0] = rgbImage->nBlue;
			imgBuffer[1] = rgbImage->nGreen;
			imgBuffer[2] = rgbImage->nRed;
			imgBuffer[3] = 0xFF;

			imgBuffer += 4;
			rgbImage++;
		}
	}
}

void KinectController::getDepthBuffer(unsigned short* depthBuffer, unsigned char* depthImageBuffer)
{
	SceneMetaData smd;
	DepthMetaData dmd;

	depthGenerator.GetMetaData(dmd);
	depthGenerator.WaitAndUpdateData();

	userGenerator.GetUserPixels(0, smd);
	unsigned int nValue = 0, colorValue = 0, nIndex = 0, nX = 0, nY = 0, nNumberOfPoints = 0;
	XnUInt16 nXRes = dmd.XRes(), nYRes = dmd.YRes();

	const XnDepthPixel* pDepth = dmd.Data();
	const XnLabel* pLabels = smd.Data();

	nIndex = 0;
	
	for (nY=0; nY<nYRes; nY++)
	{
		for (nX=0; nX < nXRes; nX++, nIndex++)
		{
			depthImageBuffer[0] = 0;
			depthImageBuffer[1] = 0;
			depthImageBuffer[2] = 0;
			depthImageBuffer[3] = 0xFF;

			nValue = *pDepth;

			if (drawBackground)
			{
				
				XnLabel label = *pLabels;
				XnUInt32 nColorID = label % NUM_COLORS;
				if (label == 0)
				{
					nColorID = NUM_COLORS;
				}

				if (nValue != 0)
				{
					colorValue = 255 - (unsigned int)((float)nValue / (float)MAX_DEPTH * 255.0f); 
					depthImageBuffer[0] = (unsigned int)(colorValue * colors[nColorID][0]);
					depthImageBuffer[1] = (unsigned int)(colorValue * colors[nColorID][1]);
					depthImageBuffer[2] = (unsigned int)(colorValue * colors[nColorID][2]);
					depthImageBuffer[3] = 0xFF;
				}
			}

			*depthBuffer = nValue;

			pDepth++;
			pLabels++;
			depthImageBuffer+=4;
			depthBuffer++;
		}
	}
}

void KinectController::updateUserGenerator()
{
	userGenerator.WaitAndUpdateData();
}

As3Skeleton* KinectController::getSkeletons()
{
	XnUserID aUsers[MAX_USERS];
	XnUInt16 nUsers = MAX_USERS;
	userGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		if (drawSkeleton && userGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
			int index = aUsers[i] - 1;
			
			skeletons[index].setUserId(aUsers[i]);

			getJointData(aUsers[i], XN_SKEL_HEAD, index, As3Skeleton::HEAD);
			getJointData(aUsers[i], XN_SKEL_NECK, index, As3Skeleton::NECK);
			getJointData(aUsers[i], XN_SKEL_TORSO, index, As3Skeleton::TORSO);
			
			getJointData(aUsers[i], XN_SKEL_LEFT_SHOULDER, index, As3Skeleton::L_SHOULDER);
			getJointData(aUsers[i], XN_SKEL_LEFT_ELBOW, index, As3Skeleton::L_ELBOW);
			getJointData(aUsers[i], XN_SKEL_LEFT_HAND, index, As3Skeleton::L_HAND);
			getJointData(aUsers[i], XN_SKEL_LEFT_HIP, index, As3Skeleton::L_HIP);
			getJointData(aUsers[i], XN_SKEL_LEFT_KNEE, index, As3Skeleton::L_KNEE);
			getJointData(aUsers[i], XN_SKEL_LEFT_FOOT, index, As3Skeleton::L_FOOT);

			getJointData(aUsers[i], XN_SKEL_RIGHT_SHOULDER, index, As3Skeleton::R_SHOULDER);
			getJointData(aUsers[i], XN_SKEL_RIGHT_ELBOW, index, As3Skeleton::R_ELBOW);
			getJointData(aUsers[i], XN_SKEL_RIGHT_HAND, index, As3Skeleton::R_HAND);
			getJointData(aUsers[i], XN_SKEL_RIGHT_HIP, index, As3Skeleton::R_HIP);
			getJointData(aUsers[i], XN_SKEL_RIGHT_KNEE, index, As3Skeleton::R_KNEE);
			getJointData(aUsers[i], XN_SKEL_RIGHT_FOOT, index, As3Skeleton::R_FOOT);

			skeletons[index].isTracking = userGenerator.GetSkeletonCap().IsTracking(aUsers[i]);
		}
	}

	return skeletons;
}

void KinectController::addGestures()
{
	gestureGenerator.AddGesture("Wave", NULL); 
	//gestureGenerator.AddGesture("Click", NULL); 
	gestureGenerator.AddGesture("RiseHand", NULL); 
	gestureGenerator.AddGesture("Swipe_Right", NULL);
	gestureGenerator.AddGesture("Right", NULL); 
	//gestureGenerator.AddGesture("RaiseHand", NULL); 
	XnUInt16 nGestures = 10;
	XnChar *gestures = new XnChar[10];
	gestureGenerator.GetActiveGestures(gestures, nGestures);
}

void KinectController::gestureRecognized(GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pIDPosition, const XnPoint3D *pEndPosition)
{
	printf("Gesture recognized: %s\n", strGesture);
}

void KinectController::gestureProgress(GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pPosition, XnFloat fProgress)
{
}

void KinectController::handCreated(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime)
{
	printf("New Hand: %d @ (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z); 
}

void KinectController::handUpdated(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime)
{
}

void KinectController::handDestroyed(HandsGenerator& generator, XnUserID nId, XnFloat fTime)
{
	printf("Lost Hand: %d\n", nId);
}

void KinectController::detectedNewUser(UserGenerator& generator, XnUserID nId)
{
	sendMessage(3,1,nId);
	printf("Detected new user: %d\n", nId);
	if (needPose)
		userGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", nId);
	else
		userGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

void KinectController::lostUser(UserGenerator& generator, XnUserID nId)
{
	printf("Lost user: %d\n", nId);
	sendMessage(3,2,nId);
	As3Skeleton skel = skeletons[nId-1];
	skel.isTracking = false;
}

void KinectController::poseDetected(PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId)
{
	sendMessage(3,3,nId);
	printf("Pose detected: %d\n", nId);
	userGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	userGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

void KinectController::calibrationStarted(SkeletonCapability& capability, XnUserID nId)
{
	printf("Calibration started: %d\n", nId);
	sendMessage(3,4,nId);
}

void KinectController::calibrationEnded(SkeletonCapability& capability, XnUserID nId, XnBool bSuccess)
{
	if (bSuccess)
	{
		printf("Calibration ended successfully: %d\n", nId);
		sendMessage(3,5,nId);
		skeletons[nId - 1] = As3Skeleton();
		userGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		printf("Calibration ended unsuccessfully: %d\n", nId);
		sendMessage(3,6,nId);
		if (needPose)
		{
			userGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", nId);
		}
		else
		{
			userGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}