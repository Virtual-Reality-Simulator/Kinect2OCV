/*
* Copyright (C) 2016, Shubham Batra (https://www.github.com/batrashubham)
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
* either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*
*/


#include "KinectFaceMat.h"

cv::Rect faceRects[BODY_COUNT];

cv::Rect* getHDfaceRect()
{
	/********    To be implemented later   *********/

	return nullptr;
}

cv::Rect* getSDFaceRect(IBodyFrameReader* _body_reader, IFaceFrameReader* _face_reader[],
	IFaceFrameSource* _face_source[], int& trackedFaces, HRESULT faceReaderinit, HRESULT bodyReaderInit)
{
	HRESULT hResult;
	if (SUCCEEDED(faceReaderinit) && SUCCEEDED(bodyReaderInit)) {
		IBodyFrame* pBodyFrame = nullptr;
		hResult = _body_reader->AcquireLatestFrame(&pBodyFrame);
		if (SUCCEEDED(hResult)) {
			IBody* pBody[BODY_COUNT] = { 0 };
			hResult = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody);
			if (SUCCEEDED(hResult)) {
				for (int count = 0; count < BODY_COUNT; count++) {
					BOOLEAN bTracked = false;
					hResult = pBody[count]->get_IsTracked(&bTracked);
					if (SUCCEEDED(hResult) && bTracked) {
						/*// Joint
						Joint joint[JointType::JointType_Count];
						hResult = pBody[count]->GetJoints( JointType::JointType_Count, joint );
						if( SUCCEEDED( hResult ) ){
						for( int type = 0; type < JointType::JointType_Count; type++ ){
						ColorSpacePoint colorSpacePoint = { 0 };
						pCoordinateMapper->MapCameraPointToColorSpace( joint[type].Position, &colorSpacePoint );
						int x = static_cast<int>( colorSpacePoint.X );
						int y = static_cast<int>( colorSpacePoint.Y );
						if( ( x >= 0 ) && ( x < width ) && ( y >= 0 ) && ( y < height ) ){
						cv::circle( bufferMat, cv::Point( x, y ), 5, static_cast<cv::Scalar>( color[count] ), -1, CV_AA );
						}
						}
						}*/

						// Set TrackingID to Detect Face
						UINT64 trackingId = _UI64_MAX;
						hResult = pBody[count]->get_TrackingId(&trackingId);
						if (SUCCEEDED(hResult)) {
							_face_source[count]->put_TrackingId(trackingId);
						}
					}
				}
			}
			for (int count = 0; count < BODY_COUNT; count++) {
				SafeRelease(pBody[count]);
			}
		}
		SafeRelease(pBodyFrame);

		for (int iFace = 0; iFace < BODY_COUNT; iFace++) {
			IFaceFrame* pFaceFrame = nullptr;
			hResult = _face_reader[iFace]->AcquireLatestFrame(&pFaceFrame);
			if (SUCCEEDED(hResult) && pFaceFrame != nullptr) {
				BOOLEAN bFaceTracked = false;
				hResult = pFaceFrame->get_IsTrackingIdValid(&bFaceTracked);
				if (SUCCEEDED(hResult) && bFaceTracked) {
					IFaceFrameResult* pFaceResult = nullptr;
					hResult = pFaceFrame->get_FaceFrameResult(&pFaceResult);
					if (SUCCEEDED(hResult) && pFaceResult != nullptr) {
						RectI faceBox;
						hResult = pFaceResult->get_FaceBoundingBoxInColorSpace(&faceBox);
						if (SUCCEEDED(hResult)) {
							trackedFaces++;
							faceRects[iFace].x = faceBox.Left;
							faceRects[iFace].y = faceBox.Top;
							faceRects[iFace].width = faceBox.Right - faceBox.Left;
							faceRects[iFace].height = faceBox.Bottom - faceBox.Top;
						}
					}
					SafeRelease(pFaceResult);
				}
			}
			SafeRelease(pFaceFrame);
		}
	}
	return faceRects;
}