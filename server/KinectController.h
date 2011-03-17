#pragma once

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnFPSCalculator.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "as3Skeleton.h"

using namespace xn;

class KinectController
{
public:
	KinectController(void);
	~KinectController(void);

	int initialise();
	void update();
	void shutdown();
	XnBool isReady();
	void getColorBuffer(unsigned char* imgBuffer);
	void getDepthBuffer(unsigned short* depthBuffer, unsigned char* depthImageBuffer);
	As3Skeleton* getSkeletons();

	void setClientSocket(SOCKET &clientSocket);
	
	void detectedNewUser(UserGenerator& generator, XnUserID userId);
	void lostUser(UserGenerator& generator, XnUserID nId);
	void poseDetected(PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId);
	void calibrationStarted(SkeletonCapability& capability, XnUserID nId);
	void calibrationEnded(SkeletonCapability& capability, XnUserID nId, XnBool bSuccess);

	void gestureRecognized(GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pIDPosition, const XnPoint3D *pEndPosition);
	void gestureProgress(GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pPosition, XnFloat fProgress);
	void handCreated(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime);
	void handUpdated(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime);
	void handDestroyed(HandsGenerator& generator, XnUserID nId, XnFloat fTime);

private:
	Context					context;
	DepthGenerator			depthGenerator;
	UserGenerator			userGenerator;
	GestureGenerator		gestureGenerator;
	HandsGenerator			handsGenerator;
	ImageGenerator			imageGenerator;

	XnBool					needPose;
	XnBool					ready;
	XnChar					strPose[20];
	XnBool					drawBackground;
	XnBool					drawPixels;
	XnBool					drawSkeleton;
	XnFPSData				xnFPS;
	As3Skeleton				*skeletons;

	static const int		MAX_DEPTH;
	static const int		MAX_PACKET_SIZE;
	static const int		MAX_USERS;
	static const XnFloat	colors[][3];
	static const int		NUM_COLORS;
	static const XnChar		*SAMPLE_XML_PATH;
	unsigned char			*msg;	

	SOCKET					clientSocket;

	void getJointData(XnUserID player, XnSkeletonJoint eJoint1, const int &skelId, const int &jointId);
	void addGestures();
	int sendData(unsigned char *buffer, int length);
	void sendMessage(const char *data);
};

/**
 * Callbacks
 */

static void XN_CALLBACK_TYPE User_NewUser(UserGenerator& generator, XnUserID userId, void* pCookie)
{
	KinectController* self = (KinectController*)pCookie;
	self->detectedNewUser(generator, userId);
}

static void XN_CALLBACK_TYPE User_LostUser(UserGenerator& generator, XnUserID nId, void* pCookie)
{
	KinectController* self = (KinectController*)pCookie;
	self->lostUser(generator, nId);
}

static void XN_CALLBACK_TYPE UserPose_PoseDetected(PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	KinectController* self = (KinectController*)pCookie;
	self->poseDetected(capability, strPose, nId);
}

static void XN_CALLBACK_TYPE Gesture_Recognized(GestureGenerator &generator, const XnChar *strGesture, 
	const XnPoint3D *pIDPosition, const XnPoint3D *pEndPosition, void *pCookie) 
{
	KinectController* self = (KinectController*)pCookie;
	self->gestureRecognized(generator, strGesture, pIDPosition, pEndPosition);
}

static void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	KinectController* self = (KinectController*)pCookie;
	self->calibrationStarted(capability, nId);
}

static void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	KinectController* self = (KinectController*)pCookie;
	self->calibrationEnded(capability, nId, bSuccess);
}

static void XN_CALLBACK_TYPE Gesture_Progress(GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pPosition, XnFloat fProgress, void *pCookie) 
{
	KinectController* self = (KinectController*)pCookie;
	self->gestureProgress(generator, strGesture, pPosition, fProgress);
}

static void XN_CALLBACK_TYPE Hand_Create(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	KinectController* self = (KinectController*)pCookie;
	self->handCreated(generator, nId, pPosition, fTime);
}

static void XN_CALLBACK_TYPE Hand_Update(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie) 
{
	KinectController* self = (KinectController*)pCookie;
	self->handUpdated(generator, nId, pPosition, fTime);
}

static void XN_CALLBACK_TYPE Hand_Destroy(HandsGenerator& generator, XnUserID nId, XnFloat fTime, void* pCookie)
{
	KinectController* self = (KinectController*)pCookie;
	self->handDestroyed(generator, nId, fTime);
}

#define CHECK_RC(nRetVal, what)										\
	if (nRetVal != XN_STATUS_OK)									\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
		return nRetVal;												\
	}
