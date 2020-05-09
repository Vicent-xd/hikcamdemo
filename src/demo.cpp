#include <stdio.h>
#include <iostream>
#include <string.h>
//#include "HCNetSDK.h"
#include "../include/HCNetSDK.h"
#include <unistd.h>

#include "opencv2/opencv.hpp"
//#include "plaympeg4.h"
#include "../include/PlayM4.h"
#include "../include/LinuxPlayM4.h"

//#include <QDir>
#include <fstream>
 
#define HPR_ERROR       -1
#define HPR_OK           0
#define USECOLOR         0

static cv::Mat dst;

HWND h = NULL;
LONG nPort=-1;
//LONG lUserID;
long lRealPlayHandle;

pthread_mutex_t mutex;
std::list<cv::Mat> g_frameList;

using namespace std;
using namespace cv;

void CALLBACK DecCBFun(LONG nPort, char *pBuf, LONG nSize, FRAME_INFO *pFrameInfo, void* nReserved1, LONG nReserved2)
{
   long lFrameType = pFrameInfo->nType;
     //std::cerr << lFrameType << std::endl; 
     if (lFrameType == T_YV12)
     {
      //cv::Mat dst(pFrameInfo->nHeight, pFrameInfo->nWidth,
      //            CV_8UC3);  // 8UC3表示8bit uchar无符号类型,3通道值
           dst.create(pFrameInfo->nHeight, pFrameInfo->nWidth,
                 CV_8UC3);
 
           cv::Mat src(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (uchar *)pBuf);
           cv::cvtColor(src, dst, cv::COLOR_YUV2BGR_YV12);//CV_YUV2BGR_YV12);
           //pthread_mutex_lock(&mutex);
           g_frameList.push_back(dst);
	   cv::imshow("bgr", dst);
           cv::waitKey(10);
          // pthread_mutex_unlock(&mutex);
     }
    usleep(1000);
 
   //cv::Mat src(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (uchar *)pBuf);
   //cv::cvtColor(src, dst, CV_YUV2BGR_YV12);
   //cv::imshow("bgr", dst);
   //pthread_mutex_lock(&mutex);
   //g_frameList.push_back(dst);
   //pthread_mutex_unlock(&mutex);
   //vw << dst;
   //cv::waitKey(10);
 
}

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize,DWORD dwUser)
{
   /*
   if (dwDataType == 1)
   {
       PlayM4_GetPort(&nPort);
       PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME);
       PlayM4_OpenStream(nPort, pBuffer, dwBufSize, 1024 * 1024);
       PlayM4_SetDecCallBackEx(nPort, DecCBFun, NULL, NULL);
       PlayM4_Play(nPort, h);
   }
   else
   {
       BOOL inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
   }*/
   //std::cerr << dwDataType << std::endl;
   DWORD dRet;
   switch (dwDataType)
   {
     case NET_DVR_SYSHEAD:           //系统头
       if (!PlayM4_GetPort(&nPort))  //获取播放库未使用的通道号
       {
         break;
       }
       if (dwBufSize > 0) {
         if (!PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME)) {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }
         if (!PlayM4_OpenStream(nPort, pBuffer, dwBufSize, 1024 * 1024)) {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }
         //设置解码回调函数 只解码不显示
        //  if (!PlayM4_SetDecCallBack(nPort, DecCBFun)) {
        //     dRet = PlayM4_GetLastError(nPort);
        //     break;
        //  }
 
         //设置解码回调函数 解码且显示
         if (!PlayM4_SetDecCallBackEx(nPort, DecCBFun, NULL, NULL))
         {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }
 
         //打开视频解码
         if (!PlayM4_Play(nPort, h))
         {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }
 
         //打开音频解码, 需要码流是复合流
         /*if (!PlayM4_PlaySound(nPort)) {
           dRet = PlayM4_GetLastError(nPort);
           break;
         }*/
       }
       break;
       //usleep(500);
     case NET_DVR_STREAMDATA:  //码流数据
       if (dwBufSize > 0 && nPort != -1) {
         BOOL inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
         while (!inData) {
           sleep(100);
           inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
           std::cerr << "PlayM4_InputData failed \n" << std::endl;
         }
       }
       break;
   }
}

void CALLBACK g_HikDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser)
{
    //NET_DVR_PTZControlWithSpeed(lRealHandle,PAN_LEFT,0,4); //handle,towards,start/stop,speed
    printf("pyd---(private)Get data,the size is %d.\n", dwBufSize);
}
void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = {0};
    switch(dwType) 
    {
    case EXCEPTION_RECONNECT:			
        printf("pyd----------reconnect--------%d\n", time(NULL));
        break;
    default:
        break;
    }
}

int Demo_RealPlay(LONG lUserID)
{         
    //Set callback function of getting stream.
    //long lRealPlayHandle;
    /*
    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.lChannel     = 1;  //channel NO
    struPlayInfo.dwLinkMode   = 0;
    struPlayInfo.bBlocked = 0;
    struPlayInfo.dwDisplayBufNum = 1;
    lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, NULL, NULL);
    
    if (lRealPlayHandle < 0)
    {
        printf("pyd1---NET_DVR_RealPlay_V40 error\n");
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -1;
    }
    printf("Init RealPlay SUCCESS\n");*/
    //Set callback function of getting stream.
    int iRet;
    //iRet = NET_DVR_SetRealDataCallBack(lRealPlayHandle, g_HikDataCallBack, 0);
    iRet = NET_DVR_SetRealDataCallBack(lRealPlayHandle, g_RealDataCallBack_V30,0);
    //iRet=NET_DVR_SaveRealData(lRealPlayHandle, "./yuntai.mp4");
    //NET_DVR_PTZControlWithSpeed(lRealPlayHandle,PAN_LEFT,1,4); 
    if (!iRet)
    {
        printf("pyd1---NET_DVR_RealPlay_V40 error\n");
        NET_DVR_StopRealPlay(lRealPlayHandle);
        NET_DVR_Logout_V30(lUserID);
        NET_DVR_Cleanup();  
        return -1;
    }
    //sleep(500);   //second
    return 0;
}
int main()
{
    NET_DVR_Init();
    NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);
    long lUserID;
    char cUserChoose = 'r';
    //login
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
    struLoginInfo.bUseAsynLogin = false;


    struLoginInfo.wPort = 8000;
    memcpy(struLoginInfo.sDeviceAddress, "172.17.1.99", NET_DVR_DEV_ADDRESS_MAX_LEN);
    memcpy(struLoginInfo.sUserName, "admin", NAME_LEN);
    memcpy(struLoginInfo.sPassword, "Ab12345678", NAME_LEN);
    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

    if (lUserID < 0)
    {
        printf("pyd1---Login error, %d\n", NET_DVR_GetLastError());
        return -1;
    }
    printf("Login SUCCESS\n");
    //long lRealPlayHandle;
    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.lChannel     = 1;  //channel NO
    struPlayInfo.dwLinkMode   = 0;
    struPlayInfo.bBlocked = 1;
    struPlayInfo.dwDisplayBufNum = 1;
    lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, NULL, NULL);
    
    if (lRealPlayHandle < 0)
    {
        printf("pyd1---NET_DVR_RealPlay_V40 error\n");
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -1;
    }
    printf("Init RealPlay SUCCESS\n");
    //Demo_RealPlay(lUserID);
    while ('q' != cUserChoose)
    {
        printf("\n");
        printf("Input 1, Test RealPlay\n");
        printf("      w/s/a/d, Test PTZControl UP,DOWN,LEFT,RIGHT\n");
        printf("      r/t/f/g, Test PTZControl UP_LEFT,UP_RIGHT,DOWN_LEFT,DOWN_RIGHT\n");
        printf("      p, Test PTZControl STOP\n");
        printf("      q, Quit.\n");
        
        printf("Input:");

        cin>>cUserChoose;
        switch (cUserChoose)
        {
        case '1':
            Demo_RealPlay(lUserID);
            break; 
        case 'w'://云台上
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,TILT_UP,0,4); //Setting params.
            break;                      
        case 's'://云台下
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,TILT_DOWN,0,4); //Setting params.
            break;                  
        case 'a'://云台左
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,PAN_LEFT,0,4); //Setting params.
            break;
        case 'd'://云台右
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,PAN_RIGHT,0,4); //Setting params.
            break;        
        case 'r'://左上
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,UP_LEFT,0,4); //Setting params.
            break;   
        case 't'://右上
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,UP_RIGHT,0,4); //Setting params.
            break;   
        case 'f'://左下
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,DOWN_LEFT,0,4); //Setting params.
            break;   
        case 'g'://右下
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,DOWN_RIGHT,0,4); //Setting params.
            break;                           
	      case 'p'://STOP
            NET_DVR_PTZControlWithSpeed(lRealPlayHandle,PAN_LEFT,1,4); //Setting params.
            break;            
        default:
            break; 
        }          
    
    }
    //stop
    NET_DVR_StopRealPlay(lRealPlayHandle);
    NET_DVR_Logout_V30(lUserID);
    NET_DVR_Cleanup();
    printf("Logout SUCCESS\n");
    return 0;

}
