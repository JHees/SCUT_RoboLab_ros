#include<iostream>
#include<opencv2/opencv.hpp>
#include"functions.h"

#include<stdlib.h>
int
main()
{
    cv::VideoCapture cap( "../resource/water.avi");
    cv::Mat frame;

    while(1)
    {
        cap>>frame;
        if(frame.empty())
        {
            break;
        }
    

    cv::Mat reduce;
    colorReduce(frame,reduce,16);
    cv::Mat HSV;
    cv::cvtColor(reduce,HSV,cv::COLOR_RGB2HSV);
    std::vector <cv::Mat> channels;
    split(HSV,channels);
    cv::Mat Value_sholded;
    myShold(channels[2],Value_sholded,[](int i)->uchar{return i>125?255:0;});
    cv::morphologyEx(Value_sholded,Value_sholded,cv::MORPH_OPEN,cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(7,7)));
    std::vector<std::vector<cv::Point>>contours;
    cv::findContours(Value_sholded,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);
    std::vector<cv::RotatedRect> rRect;
    for(auto i:contours)
    {
        rRect.push_back(cv::minAreaRect(i));
    }
    for (auto &i : rRect)
    {
        if (i.size.width < i.size.height)
        {
            i.angle += 90;
            int buf = i.size.width;
            i.size.width = i.size.height;
            i.size.height = buf;
        }
        i.angle+=i.angle<0?180:0;
    }
    for(size_t i=0;i<rRect.size();++i)
    {
        if(rRect[i].size.width/rRect[i].size.height<3.2||(rRect[i].angle<40||rRect[i].angle>140))
        {
            rRect.erase(rRect.begin()+i);
           --i;
        }
    }

    std::vector<cv::Point2f> corner;
    for (auto i :rRect)
    {
        cv::Point2f vertices[4];
        i.points(vertices);
        if(rRect[0].center.x>rRect[1].center.x?!corner.empty():corner.empty())
        {
            corner.push_back(vertices[0]);
            corner.push_back(vertices[3]);
        }
        else
        {
            corner.push_back(vertices[2]);
            corner.push_back(vertices[1]);
        }
    }
    const std::vector<cv::Point3f>corner_w{cv::Point3f(-6.75,-2.75,0),
                                                                                        cv::Point3f(-6.75,2.75,0),
                                                                                        cv::Point3f(6.75,2.75,0),
                                                                                        cv::Point3f(6.75,-2.75,0)};

    // cv::Mat corner_c(2,4,CV_64FC1);
    // for(size_t i=0;i<4;++i)
    // {
    //     corner_c.ptr(0)[i]=(float)corner[i].x;
    //     corner_c.ptr(1)[i]=(float)corner[i].y;
    // }
    // const cv::Mat corner_w = (cv::Mat_<float>(3,4)<<-6.75,  -6.75,  6.75,   6.75,
    //                                                                                                     -2.75,  2.75,   2.75,  -2.75,
    //                                                                                                             0,        0,         0,          0);
    const cv::Mat cameraM = (cv::Mat_<float>(3,3)<<1128.048344,0                            ,339.421769,
                                                                                                        0                       ,1127.052190    ,236.535242,
                                                                                                        0                       ,0                            ,1);
    const cv::Mat distCoeffs = (cv::Mat_ <float>(1,5)<<-0.568429,0.514592,-0.000126,0.000500,0);
    cv::Mat Rvec(3,1,CV_64FC1),Tvec(3,1,CV_64FC1);
    cv::Mat rotMat(3,3,CV_64FC1);
    cv::solvePnP(corner_w,corner,cameraM,distCoeffs,Rvec,Tvec);
    cv::Rodrigues(Rvec,rotMat);
    Rvec.convertTo(Rvec,CV_64FC1);
    Tvec.convertTo(Tvec,CV_64FC1);
    rotMat.convertTo(rotMat,CV_64FC1);
   // cv::transpose(Tvec,Tvec);
    std::vector<cv::Point3f>corner_cam(corner_w);
    for(auto &i:corner_cam)
    {
        cv::Mat buf(1,3,CV_64FC1);
        cv::Mat(i).convertTo(buf,CV_64FC1);
        buf = rotMat*buf+Tvec;
        //std::cout<<( rotMat*buf+Tvec)<<std::endl;
        i=cv::Point3f(buf);
        std::cout<<i<<std::endl;
    }
    //rotMat.inv();
    
    //std::cout<<Tvec<<std::endl;
    cv::waitKey();
        for (int j = 0; j < 4; j++)
        {			
            line(frame, corner[j], corner[(j + 1) % 4], cv::Scalar(255, 0, 0), 2);
        }
        imshow("frame",frame);
        cv::waitKey(30);
    }
}