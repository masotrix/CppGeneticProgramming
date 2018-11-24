#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <Plot.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
using namespace cv;
using namespace std;

static const Size g_basicSz(600,335);
static const Scalar textCol = Scalar::all(90);
static const Scalar deleteCol = Scalar::all(255);
static const Scalar axesCol = Scalar::all(170);
static const Scalar vanishCol = Scalar::all(220);

Plot::Plot():
  _xticks(4), _yticks(4),
  _xlabel(string("")),
  _ylabel(string("")),
  _title(string("")),
  _ylabelRotate(true),
  _lims({{-50,-120,50,-120}})
{}

void Plot::setYLabelRotate(bool rotate) {
  _ylabelRotate = rotate;}


void Plot::addPlotData(vector<vector<float>> X,
    const Scalar &color, bool style) {

  _plotData.push_back(X);
  _colors.push_back(color);
  _styles.push_back(style);
}

void Plot::setTicks(int x, int y) {
  _xticks = x; _yticks = y;
}

void Plot::setXLabel(string xlabel) {
  _xlabel = xlabel;
}

void Plot::setYLabel(string ylabel) {
  _ylabel = ylabel;
}

void Plot::setTitle(string title) {
  _title = title;
}

void Plot::setLimits(vector<float> lims) {
  _lims = lims;
}

Mat Plot::generatePlot() {

  vector<float> lims(_lims.begin(),_lims.end());
  
  Size chartSz(g_basicSz), padding(95,75),
       paddSz=chartSz-padding-padding;
  int leftSz=paddSz.width*max(0.f,-lims[0])/(lims[1]-lims[0]),
      rightSz=paddSz.width*max(0.f,lims[1])/(lims[1]-lims[0]),
      bottomSz=paddSz.height*max(0.f,-lims[2])/(lims[3]-lims[2]),
      topSz=paddSz.height*max(0.f,lims[3])/(lims[3]-lims[2]);
  float maxHoriLim = max(-lims[0],lims[1]),
        maxHoriSz=max(leftSz,rightSz),
        maxVertLim = max(-lims[2],lims[3]),
        maxVertSz = max(bottomSz,topSz);
  Point2f origin(padding.width+leftSz,padding.height+topSz);
  Mat chart(chartSz,CV_8UC3); chart.setTo(deleteCol);

  for (int k=0; k<_plotData.size(); k++) {
    if (_styles[k]) {
      for (int i=1; i<_plotData[k].size(); i++) {
        Point2f p1(_plotData[k][i-1][0],-_plotData[k][i-1][1]);
        Point2f p2(_plotData[k][i][0],-_plotData[k][i][1]);
        float xscale,yscale;
        xscale=maxHoriSz/maxHoriLim; yscale=maxVertSz/maxVertLim;
        p1.x*=xscale; p1.y*=yscale; p2.x*=xscale; p2.y*=yscale;
        p1+=origin; p2+=origin; 
      
        line(chart, (Point)p1, (Point)p2, _colors[k], 1, CV_AA);
      }
      for (int i=0; i<_plotData[k].size(); i++) {
        Point2f p(_plotData[k][i][0],-_plotData[k][i][1]);
        float xscale,yscale;
        xscale=maxHoriSz/maxHoriLim; yscale=maxVertSz/maxVertLim;
        p.x*=xscale; p.y*=yscale; p+=origin; 
      
        circle(chart, (Point)p, 3, Scalar(0,0,0), -1, CV_AA);
      }
    }
    else {
      for (int i=0; i<_plotData[k].size(); i++) {
        float alpha = 0.7;
        Size alphArea(6,6), circleArea(4,4);
        Point circleIni = Point((alphArea-circleArea)/2);
        Mat imgRect(alphArea,CV_8UC3, deleteCol); 
        Point2f p(_plotData[k][i][0],-_plotData[k][i][1]);
        p.x*=maxHoriSz/maxHoriLim; p.y*=maxVertSz/maxVertLim;
        p+=origin;

        RotatedRect rect(circleIni,circleArea,0.0);
        ellipse(imgRect, rect, _colors[k],-1,CV_AA);
        Mat roi = chart(Rect((Point)p-circleIni,alphArea));
        addWeighted(roi,1-alpha,imgRect,alpha,0,roi);
      }
    }
  }

  line(chart, Point(padding.width+leftSz,padding.height),
      Point(padding.width+leftSz,paddSz.height+padding.height),
      axesCol, 1, CV_AA);
  line(chart, Point(padding.width,padding.height+topSz),
      Point(padding.width+paddSz.width,padding.height+topSz),
      axesCol, 1, CV_AA);
  putText(chart, _title,
      Point(chartSz.width/2-6.25*_title.size(),40),
      FONT_HERSHEY_SIMPLEX, 0.8, textCol, 1, CV_AA);
  putText(chart, _xlabel,
      Point(chartSz.width/2-5*_xlabel.size(),chartSz.height-20),
      FONT_HERSHEY_SIMPLEX, 0.7, textCol, 1, CV_AA);
  int dum; Size ylabelSz = getTextSize(_ylabel,
      FONT_HERSHEY_SIMPLEX, 0.7, 1, &dum);
  ylabelSz += Size(6,7);

  if (!_ylabelRotate) {
    putText(chart, _ylabel,
        Point(18,
          chartSz.height/2-5*_ylabel.size()),
        FONT_HERSHEY_SIMPLEX, 0.7, textCol, 1, CV_AA);
  } else {
    Mat ylabelImg(ylabelSz,CV_8UC3); ylabelImg.setTo(deleteCol);
    putText(ylabelImg, _ylabel,Point(3,ylabelSz.height-6),
        FONT_HERSHEY_SIMPLEX, 0.7, textCol, 1, CV_AA);
    ylabelImg=ylabelImg.t();
    flip(ylabelImg,ylabelImg,0);
    ylabelImg.copyTo(chart(Rect(17,4*chartSz.height/9-5*
            _ylabel.size(),ylabelSz.height,ylabelSz.width)));
  }


  int topTicks=(int)(_yticks*topSz/(topSz+bottomSz));
  int bottomTicks=(int)(_yticks*bottomSz/(topSz+bottomSz));
  int leftTicks=(int)(_xticks*leftSz/(leftSz+rightSz));
  int rightTicks=(int)(_xticks*rightSz/(leftSz+rightSz));
  stringstream sstream;
    
  {
    float ypos = padding.height, yvalue=lims[3]; string label;
    int precision; maxVertLim>=1000?precision=0:precision=1;
    for (int i=0; i<topTicks; i++) {
      sstream.str(string());
      sstream << fixed << setprecision(precision) << yvalue;
      label = sstream.str();
      line(chart, Point(padding.width+leftSz-5,ypos),
          Point(padding.width+leftSz+5,ypos),
          axesCol, 1, CV_AA);
      putText(chart, label,
          Point(padding.width+leftSz-10*(label.size()+1),ypos+5),
          FONT_HERSHEY_PLAIN, 1, axesCol, 1, CV_AA);
      ypos += topSz/topTicks;
      yvalue -= lims[3]/topTicks;
    }

    ypos = padding.height+topSz+bottomSz; yvalue = lims[2];
    for (int i=0; i<bottomTicks; i++) {
      sstream.str(string());
      sstream << fixed << setprecision(precision) << yvalue;
      label = sstream.str();
      line(chart, Point(padding.width+leftSz-5,ypos),
          Point(padding.width+leftSz+5,ypos),
          axesCol, 1, CV_AA);
      putText(chart, label,
          Point(padding.width+leftSz-10*(label.size()+1),ypos+5),
          FONT_HERSHEY_PLAIN, 1, axesCol, 1, CV_AA);
      ypos -= bottomSz/bottomTicks; 
      yvalue -= lims[2]/bottomTicks;
    }
  }

  {
    float xpos = padding.width, xvalue=lims[0]; string label;
    int precision; maxHoriLim>=1000?precision=0:precision=1;
    for (int i=0; i<leftTicks; i++) {
      sstream.str(string());
      sstream << fixed << setprecision(precision) << xvalue;
      label = sstream.str();
      line(chart,
          Point(xpos,padding.height+topSz-5),
          Point(xpos,padding.height+topSz+5),
          axesCol, 1, CV_AA);
      putText(chart, label,
          Point(xpos-10*(label.size()-2.5),
            padding.height+topSz+25),
          FONT_HERSHEY_PLAIN, 1, axesCol, 1, CV_AA);
      xpos += leftSz/leftTicks;
      xvalue -= lims[0]/leftTicks;
    }

    xpos = padding.width+leftSz+rightSz; xvalue=lims[1];

    for (int i=0; i<rightTicks; i++) {
      sstream.str(string());
      sstream << fixed << setprecision(precision) << xvalue;
      label = sstream.str();
      line(chart,
        Point(xpos,padding.height+topSz-5),
        Point(xpos,padding.height+topSz+5),
        axesCol, 1, CV_AA);
      putText(chart, label,
          Point(xpos-10*(label.size()-2.5),
            padding.height+topSz+25),
          FONT_HERSHEY_PLAIN, 1, axesCol, 1, CV_AA);
      xpos -= rightSz/rightTicks;
      xvalue -= lims[1]/rightTicks;
    }
  }

  return chart;
}

void Plotter::addPlot(shared_ptr<Plot> plot) {
  _plots.push_back(plot);
}

void Plotter::plot(string window) {

  namedWindow(window, 1);
  if (!_plots.size()) return;
  
  Size chartSz;
  Mat chart, aplot, roi; Point ori;


  switch (_plots.size()) {

    case 1:
      chartSz = Size(g_basicSz);
      chart = Mat(chartSz, CV_8UC3);

      ori=Point(0,0);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[0]->generatePlot();
      aplot.copyTo(roi);

      imshow(window, chart);
      break;
    case 2:
      chartSz = Size(g_basicSz.width*2,g_basicSz.height);
      chart = Mat(chartSz, CV_8UC3);

      ori=Point(0,0);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[0]->generatePlot();
      aplot.copyTo(roi);

      ori=Point(g_basicSz.width,0);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[1]->generatePlot();
      aplot.copyTo(roi);

      line(chart, Point(g_basicSz.width,0),
          Point(g_basicSz), vanishCol, 1, CV_AA);

      imshow(window, chart);
      break;
    case 3:
      chartSz = Size(2*g_basicSz.width,2*g_basicSz.height);
      chart = Mat(chartSz, CV_8UC3);

      ori=Point(0,0);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[0]->generatePlot();
      aplot.copyTo(roi);

      ori=Point(g_basicSz.width,0);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[1]->generatePlot();
      aplot.copyTo(roi);

      ori=Point(0,g_basicSz.height);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[2]->generatePlot();
      aplot.copyTo(roi);

      line(chart, Point(g_basicSz.width,0),
          Point(g_basicSz), vanishCol, 1, CV_AA);
      line(chart, Point(0,g_basicSz.height),
          Point(g_basicSz), vanishCol, 1, CV_AA);

      imshow(window, chart);
      break;
    case 4:
      chartSz = Size(2*g_basicSz.width,2*g_basicSz.height);
      chart = Mat(chartSz, CV_8UC3);

      ori=Point(0,0);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[0]->generatePlot();
      aplot.copyTo(roi);

      ori=Point(g_basicSz.width,0);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[1]->generatePlot();
      aplot.copyTo(roi);

      ori=Point(0,g_basicSz.height);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[2]->generatePlot();
      aplot.copyTo(roi);

      ori=Point(g_basicSz.width,g_basicSz.height);
      roi = chart(Rect(ori,g_basicSz));
      aplot = _plots[3]->generatePlot();
      aplot.copyTo(roi);

      line(chart, Point(g_basicSz.width,0),
          Point(g_basicSz.width,chartSz.height),
          vanishCol, 1, CV_AA);
      line(chart, Point(0,g_basicSz.height),
          Point(chartSz.width, g_basicSz.height),
          vanishCol, 1, CV_AA);

      imshow(window, chart);
      break;
  }

  waitKey();
  destroyAllWindows();
}
