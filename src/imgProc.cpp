#include "ros/ros.h"
#include "topics.h"
#include "roycobot/imgPosition.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include <string.h>
#include <aruco/cvdrawingutils.h>

cv::Mat inputFrame;
int grad;
cv::VideoCapture cap("0");
cv::vector<cv::Point2f> grid;

ros::Subscriber posScrib;
ros::Publisher posPub;

#define boardX 0
#define boardY 0

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
    cv::threshold(HSV[0],thres[1],140,255,CV_THRESH_BINARY);
    cv::threshold(HSV[2],thres[2],190,255,CV_THRESH_BINARY_INV);
    cv::threshold(HSV[2],thres[3],50,255,CV_THRESH_BINARY);
    cv::Mat mask = thres[0] + thres[1] & thres[2] & thres[3];
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
        return 404;
    }
}

cv::Point2f positionDef()
{
    vulCoord();
    aruco::MarkerDetector MDetector;
    cv::vector<aruco::Marker> Markers;
    aruco::CameraParameters TheCameraParams;
    TheCameraParams.readFromXMLFile("out_camera_params.yml");
    MDetector.detect(inputFrame,Markers,TheCameraParams,0.176);
    for (unsigned int i=0; i<Markers.size(); i++)
    {
        Markers[i].draw(inputFrame,cv::Scalar(0,0,255),2);
        Markers[i].calculateExtrinsics(9.021597938, TheCameraParams);
        double modelview_matrix[16];
        Markers[i].glGetModelViewMatrix(modelview_matrix);
        ///cv::Mat modelV = Mat(4, 4, CV_32FC1, &modelview_matrix);
        ///modelV.inv();
    }
    if (Markers[0].id == 110 || Markers[0].id == 114 || Markers[0].id == 125 || Markers[0].id == 129)
    {
        for (unsigned int j=0; j<Markers.size(); j++)
        {
            Markers[j] = Markers[j + 1];
        }
        if (Markers.size() == 2)
        {
            return cv::Point2f(404,404);
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
    else
    {
        return cv::Point2f(-1,-1);
    }
}

bool getPosition(roycobot::imgPosition::Request &req,
		 roycobot::imgPosition::Response &res)
{
    if(req.cmd == "getPos")
    {
        ROS_INFO("Calculating position");
        
	cap >> inputFrame;
        
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

/*
void chatterCan(const roycobot::turn::ConstrPtr& msg)
{
    if(msg->r == 666)
    {
        ROS_INFO("Please calculate cans position");
        cap>>inputFrame;
        double grads = findCan();
        grad = (int)grads;

        roycobot::turn sendmsg;
        sendmsg.r=grad;
        turnPub.publish(sendmsg);

        ros::spinOnce();

    }
    else
    {
        ROS_INFO("Turn calculated r: %u",msg->r);
    }
}
*/

int main(int argc, char *argv[])
{
    ros::init(argc,argv,"pathplanner");
    ros::NodeHandle n;
    //posScrib=pHandlerLis.subscribe(pathplanning_img,1, chatterPosition);
//    ros::Subscriber turnScrib=tHandlerLis.subscribe(robotturn,1,chatterCan);
    //posPub= pHandlerPub.advertise<roycobot::position2d>(robotposition, 10);
//    ros::Publisher turnPub=tHandlerPub.advertise<roycobot::turn>(robotturn,1000);
    ros::ServiceServer service = n.advertiseService(robotposition, getPosition);

    ros::spin();
    return 0;
}
