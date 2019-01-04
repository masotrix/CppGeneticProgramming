#include <GP.h>
#include <Plot.h>
#include <TSP.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <opencv2/core.hpp>
#include <fstream>
using namespace std;
using namespace cv;

int main(int argc, char **argv) {

  int cities=40, members = 2000, popDivision=2, epochs=150;
  double mutRate=0.5; bool verbose = false;


  {
    chrono::time_point<chrono::system_clock> start,end;
    chrono::duration<double> seconds;
    static default_random_engine gen;
    uniform_real_distribution<double> coords(0,1);
    float sum;

    string filename("cityTest.txt");
    if (argc>=2) filename = argv[1];
    else {
      ofstream file("cityTest.txt");
      double x,y;
      for (int i=0; i<cities; i++) {
        x = 200*coords(gen); y = 200*coords(gen);
        file << x << ' ' << y << endl;
      }
      file.close();
    }

    unique_ptr<FindTravelerGA> ga;
    vector<vector<float>> points;
    vector<double> avgs; vector<vector<float>> avgCoords;
    float minLength, maxAvg;
    double duration;

    stringstream ssac; ssac.str(string());
    ssac << fixed << setprecision(1.f);
    stringstream ssec; ssec.str(string());
    ssec << fixed << setprecision(5.f);
    unique_ptr<Plotter> cityPlotter(new Plotter());
  
   
    start = chrono::system_clock::now();
    ga = make_unique<FindTravelerGA>(members,cities,filename);
    avgs = ga->evolve(mutRate,epochs,verbose,popDivision);
    end = chrono::system_clock::now(); seconds = end-start;
    minLength = ga->getMinFitness();
    duration = seconds.count();

    points = ga->getOptPoints(); 
    points.push_back(points[0]);

    for (int i=0; i<avgs.size(); i++) {
      maxAvg=max(maxAvg,(float)avgs[i]);
      avgCoords.push_back({(float)i,(float)avgs[i]}); 
    }

    {
      shared_ptr<Plot> evoChart(new Plot());
      evoChart->addPlotData(avgCoords, false);
      evoChart->setTicks(4,4);
      evoChart->setXLabel("Epoch");
      evoChart->setYLabel("Distance");
      ssac << minLength; ssec << duration;
      evoChart->setTitle(string("Avg Distance vs Epoch"));
      ssac.str(string()); ssec.str(string());
      evoChart->setLimits({0,(float)avgs.size(),0,maxAvg});
      cityPlotter->addPlot(evoChart);
    }
    {
      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points);
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minLength; ssec << duration;
      cityChart->setTitle(
          string("Genetic TSP ")+
          string("[D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }
    {
      start = chrono::system_clock::now();
      tie(points,sum) = mstRun(filename);
      end = chrono::system_clock::now(); seconds = end-start;
      minLength = sum;
      duration = seconds.count();
      points.push_back(points[0]);

      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points);
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minLength; ssec << duration;
      cityChart->setTitle(
          string("MST TSP ")+
          string("[D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }
    {
      start = chrono::system_clock::now();
      tie(points,sum) = hullRun(filename);
      end = chrono::system_clock::now(); seconds = end-start;
      minLength = sum;
      duration = seconds.count();
      points.push_back(points[0]);

      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points);
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minLength; ssec << duration;
      cityChart->setTitle(
          string("Convex Hull TSP ")+
          string("[D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }

    ssac << mutRate;
    cityPlotter->plot(
      string("Avg Fitness vs Epoch [")+
    string("E:")+to_string(epochs)+string(", ")+
    string("P:")+to_string(members)+string(", ")+
    string("MR:")+ssac.str()+string(", ")+
    string("ED:")+to_string(popDivision)+string("]"));
    ssac.str(string());

  }
}
