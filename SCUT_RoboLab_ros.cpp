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

    
    if(rRect.size()!=2)
        cv::waitKey(0);
    for (auto i : rRect)
    {
        cv::Point2f vertices[4];
        i.points(vertices);
        for (int j = 0; j < 4; j++)
        {			
            line(frame, vertices[j], vertices[(j + 1) % 4], cv::Scalar(255, 0, 0), 2);
        }
    }
        imshow("frame",frame);
        cv::waitKey(30);
    }
}