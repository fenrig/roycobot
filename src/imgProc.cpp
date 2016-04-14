#include "ros/ros.h"
#include "topics.h"
#include "roycobot/imgPosition.h"
#include "roycobot/imgCanPosition.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include <string.h>
#include <aruco/cvdrawingutils.h>

#include <limits.h>

cv::VideoCapture webcam;
cv::Mat inputFrame;
int grad;
cv::vector<cv::Point2f> grid;

#define aantalfotostrekken 4 
#define boardWidth 96
#define boardHeight 160
#define boardX boardWidth
#define boardY boardHeight

#define DEBUG 2


void takepicture(void){ 
	static unsigned int count = 1;
	
	    vector<int> compression_params;
	    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	    compression_params.push_back(1);

	ostringstream convert;
	convert << count;

	std::string namefile = "/home/alarm/driveandcapture/test" + convert.str() + ".png";

	ROS_INFO("Count van foto: %u \n", count);	

	webcam.read(inputFrame); 

	imwrite(namefile, inputFrame, compression_params);
	count++;
}

void vulCoord()
{
    cv::Point2f pt(96,13.2);
    grid.push_back(pt);
    for(int i=0; i < 10; i++ )
    {
        pt.y += 13.3;
        grid.push_back(pt);
    }
    pt.x = 76.8;
    pt.y = 160;
    grid.push_back(pt);
    for(int i=0; i < 3; i++ )
    {
        pt.x -= 19.2;
        grid.push_back(pt);
    }
    pt.x = 0;
    pt.y = 146.9;
    grid.push_back(pt);
    for(int i=0; i < 10; i++ )
    {
        pt.y -= 13.3;
        grid.push_back(pt);
    }
    pt.x = 19;
    pt.y = 0;
    grid.push_back(pt);
    for(int i=0; i < 3; i++ )
    {
        pt.x += 19.2;
        grid.push_back(pt);
    }
}

cv::Point2f cosinusRegel(aruco::Marker m1, aruco::Marker m2)
{
    float x1 = m1.Tvec.ptr< float >(0)[0];
    float z1 = m1.Tvec.ptr< float >(0)[2];
    float x2 = m2.Tvec.ptr< float >(0)[0];
    float z2 = m2.Tvec.ptr< float >(0)[2];
    float dist = 13.3;
    float d12 = x1*x1+z1*z1;
    float d22 = x2*x2+z2*z2;
    float cosAlpha = (d22 + dist * dist - d12) / (2 * sqrt(d22) * dist);
    float alpha = 180 * acos(cosAlpha) / M_PI;
    float y = sqrt(d22) * cosAlpha;
    float x = sqrt(d22 - y * y);
    return cv::Point2f(x, y);
}

double findCan()
{
    cv::Mat HSV[3];
    cv::Mat thres[4];
    int maxHist[2];
    cv::Mat inputClone = inputFrame.clone();
    for(int i=0; i<(inputClone.rows)/5; i++)
    {
        for(int j=0; j<inputClone.cols; j++)
        {
            inputClone.at<cv::Vec3b>(i,j)=(0,0,0);
        }
    }
    cv::cvtColor(inputClone,inputClone,CV_BGR2HSV);
    cv::split(inputClone,HSV);
    cv::threshold(HSV[0],thres[0],10,255,CV_THRESH_BINARY_INV);
    cv::threshold(HSV[0],thres[1],165,255,CV_THRESH_BINARY);
    cv::threshold(HSV[2],thres[2],35,255,CV_THRESH_BINARY);
    cv::threshold(HSV[1],thres[3],170,255,CV_THRESH_BINARY);
    cv::Mat mask = (thres[0] + thres[1]) & thres[2] & thres[3];
    cv::erode(mask,mask,cv::Mat());
    cv::dilate(mask,mask,cv::Mat());
    int histogram[mask.cols];
    maxHist[0]=0;
    maxHist[1]=0;
    std::fill_n(histogram,mask.cols,0);
    for(int i=0; i<mask.rows; i++)
    {
        for(int j=0; j<mask.cols; j++)
        {
            if(mask.at<uchar>(i,j)>0)
            {
                histogram[j]++;
            }
        }
    }
    for(int i=0; i<mask.cols; i++)
    {
        if(histogram[i]>maxHist[0])
        {
            maxHist[0]=histogram[i];
            maxHist[1]=i;
        }
    }
    double gpp = 44.5/mask.cols;
    if(maxHist[0]>70)
    {
        double graden = (maxHist[1]-(mask.cols/2.0))*gpp+2.25;
        return graden;
    }
    else
    {
        return INT_MAX;
    }
}

cv::Point2f positionDef()
{
    vulCoord();
    aruco::MarkerDetector MDetector;
    cv::vector<aruco::Marker> Markers;
    aruco::CameraParameters TheCameraParams;
    TheCameraParams.readFromXMLFile("/home/alarm/catkin_ws/src/roycobot/src/out_camera_params.yml");
    MDetector.detect(inputFrame,Markers,TheCameraParams,0.176);
	    
    if(Markers.size() == 0){
	ROS_INFO("Test: marker.size == %d failed", Markers.size());
        goto lblret;
    }

    for (unsigned int i=0; i<Markers.size(); i++)
    {
        Markers[i].draw(inputFrame,cv::Scalar(0,0,255),2);
        Markers[i].calculateExtrinsics(9.021597938, TheCameraParams);

        ///cv::Mat modelV = Mat(4, 4, CV_32FC1, &modelview_matrix);
        ///modelV.inv();
	ROS_INFO("i = %u", i);
        
    }
    if (Markers[0].id == 110 || Markers[0].id == 114 || Markers[0].id == 125 || Markers[0].id == 129)
    {
        for (unsigned int j=0; j<Markers.size(); j++)
        {
            Markers[j] = Markers[j + 1];
        }
        if (Markers.size() == 2)
        {
            goto lblret;
        }
    }
    if (Markers.size() >= 2 && Markers[0].id != 110 && Markers[0].id != 114 && Markers[0].id != 125 && Markers[0].id != 129)
    {
        cv::Point2f pos = cosinusRegel(Markers[0], Markers[1]);
        if (100 <= Markers[0].id && Markers[0].id <= 110)
        {
            pos.x = boardX - pos.x;
            pos.y = grid[Markers[1].id - 100].y - pos.y;
        }
        else if (111 <= Markers[0].id && Markers[0].id <= 114)
        {
            pos.x = grid[Markers[1].id - 100].x + pos.x;
            pos.y = boardY - pos.y;
        }
        else if (115 <= Markers[0].id && Markers[0].id <= 125)
        {
            pos.x = pos.x;
            pos.y = grid[Markers[1].id - 100].y + pos.y;
        }
        else
        {
            pos.x = grid[Markers[1].id - 100].x - pos.x;
            pos.y = pos.y;
        }
        return pos;
    }

lblret:
    return cv::Point2f(UINT_MAX,UINT_MAX);
}

bool getPosition(roycobot::imgPosition::Request &req,
		 roycobot::imgPosition::Response &res)
{
    if(req.cmd == "getPos")
    {
        ROS_INFO("Calculating position");
        
        int frame = 0;
        for(; frame < aantalfotostrekken + 1; frame++){
#if ( DEBUG && 2)
	takepicture();
#else
	webcam >> inputFrame;
#endif
        }
        
	cv::Point2f point = positionDef();
        res.x= (uint)point.x;
        res.y= (uint)point.y;

        ROS_INFO("POS: (x = %u ,y = %u )", res.x, res.y);
	return true;
    }
    else
    {
        ROS_ERROR("unknown command");
	return false;
    }
}

bool chatterCan(roycobot::imgCanPosition::Request &req,
		 roycobot::imgCanPosition::Response &res)
{
    if(req.cmd == "findcan")
    {
        ROS_INFO("Please calculate cans position");

    int frame = 0;
    for(; frame < aantalfotostrekken + 1; frame++){
#if ( DEBUG && 2)
	takepicture();
#else
	webcam >> inputFrame;
#endif
     }
        res.rot = (int ) findCan();

	ROS_INFO("POS: (rot = %d)", res.rot);

	return true;
    }
    else
    {
	return false;
    }
}

int main(int argc, char *argv[])
{
    ros::init(argc,argv,"imgProc");
    ros::NodeHandle n;
    //posScrib=pHandlerLis.subscribe(pathplanning_img,1, chatterPosition);
//    ros::Subscriber turnScrib=tHandlerLis.subscribe(robotturn,1,chatterCan);
    //posPub= pHandlerPub.advertise<roycobot::position2d>(robotposition, 10);
//    ros::Publisher turnPub=tHandlerPub.advertise<roycobot::turn>(robotturn,1000);
    ros::ServiceServer serviceRobotPos = n.advertiseService(robotposition, getPosition);
    ros::ServiceServer serviceCanPos = n.advertiseService(canposition, chatterCan);
    
    webcam = cv::VideoCapture(0);
    webcam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    webcam.set(CV_CAP_PROP_FRAME_WIDTH, 640);

    ros::spin();
    return 0;
}
