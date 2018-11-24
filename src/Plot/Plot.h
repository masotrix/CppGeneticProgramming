#ifndef PLOT_H
#define PLOT_H
#include <string>
#include <vector>
#include <memory>
#include <opencv2/core.hpp>

class Plot {
  private:
    std::vector<std::vector<std::vector<float>>> _plotData;
    std::vector<cv::Scalar> _colors;
    std::vector<bool> _styles;
    int _xticks, _yticks;
    std::string _xlabel, _ylabel, _title;
    bool _ylabelRotate;
    std::vector<float> _lims;

  public:
    Plot();

    void addPlotData(std::vector<std::vector<float>> X,
        const cv::Scalar &color = cv::Scalar(0,0,255),
        bool style = true);

    void setTicks(int x, int y);
    void setXLabel(std::string xlabel);
    void setYLabel(std::string ylabel);
    void setYLabelRotate(bool rotate);
    void setTitle(std::string title);
    void setLimits(std::vector<float> lims);
    cv::Mat generatePlot();
};

class Plotter {
  
  private:
    std::vector<std::shared_ptr<Plot>> _plots;

  public:
    void addPlot(std::shared_ptr<Plot>);
    void plot(std::string window = std::string("OpenCV Plot"));
};


#endif
