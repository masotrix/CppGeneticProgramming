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
  
  { // Experimento 1: Aproximar numero


    cout << "GP: Find Number\n";
    int epochs = 10;
    int members=512, depth=5; 
    float iniD=0,step=0.1,endD=0;
    int low=2,up=6,nVals=2;
    double mutRate = 0.2;
    float varProb=0.f, cross=0.2f, lowerBound=0.0001f;
    function<float(float)> func = [](float x){ 
      return 147; };


    {
      chrono::time_point<chrono::system_clock> start,end;
      chrono::duration<double> seconds;
      static default_random_engine gen;
      uniform_real_distribution<double> coords(0,1);
      float sum;

      unique_ptr<AstGA> ga;
      vector<double> avgs; vector<vector<float>> avgCoords;
      float minLength, maxAvg;
      double duration;

      stringstream ssac; ssac.str(string());
      ssac << fixed << setprecision(1.f);
      stringstream ssec; ssec.str(string());
      ssec << fixed << setprecision(5.f);
      unique_ptr<Plotter> gpPlotter(new Plotter());
    
     
      start = chrono::system_clock::now();

      ga = make_unique<AstGA>(members,func,
        iniD,step,endD,low,up,nVals,varProb, cross,
        lowerBound, depth);
      cout << "GP example ";
      cout << "[P:"<<members<<", MR: "<<mutRate<<"]\n";

      avgs = ga->evolve(mutRate,epochs,true);
      cout << "Needed ites: " << avgs.size() << endl << endl;

      end = chrono::system_clock::now(); seconds = end-start;
      minLength = ga->getMinFitness();
      duration = seconds.count();

      for (int i=0; i<avgs.size(); i++) {
        maxAvg=max(maxAvg,(float)avgs[i]);
        avgCoords.push_back({(float)i,(float)avgs[i]}); 
      }

      {
        shared_ptr<Plot> evoChart(new Plot());
        evoChart->addPlotData(avgCoords, true);
        evoChart->setTicks(4,4);
        evoChart->setXLabel("Epoch");
        evoChart->setYLabel("Avg. Error");
        ssac << minLength; ssec << duration;
        evoChart->setTitle(string("Average Error vs Epoch"));
        ssac.str(string()); ssec.str(string());
        evoChart->setLimits({0,(float)avgs.size(),0,maxAvg});
        gpPlotter->addPlot(evoChart);
      }

      ssac << mutRate;
      gpPlotter->plot(
        string("GP Number Approximation: 147 [")+
      string("E:")+to_string(epochs)+string(", ")+
      string("P:")+to_string(members)+string(", ")+
      string("MR:")+ssac.str()+string("]"));
    }
  }

  { // Experiemento 2: Aproximar funcion
  

    cout << "GP: Find Function\n";
    int epochs = 100;
    int members=512, depth=5;
    float iniD=-1.f,step=0.1,endD=1.f;
    int low=2,up=4,nVals=5;
    double mutRate = 0.1;
    float varProb=0.2f, cross=0.2f, lowerBound=0.0001f;
    function<float(float)> func = [](float x){
      return x*x*x*x+x*x+x+3; };


    {
      chrono::time_point<chrono::system_clock> start,end;
      chrono::duration<double> seconds;
      static default_random_engine gen;
      uniform_real_distribution<double> coords(0,1);
      float sum;

      unique_ptr<AstGA> ga;
      vector<double> avgs;
      vector<vector<float>> avgCoords, domVal1, domVal2;
      float minLength, maxAvg;
      double duration;

      stringstream ssac; ssac.str(string());
      ssac << fixed << setprecision(1.f);
      stringstream ssec; ssec.str(string());
      ssec << fixed << setprecision(5.f);
      unique_ptr<Plotter> gpPlotter(new Plotter());
    
     
      start = chrono::system_clock::now();

      ga = make_unique<AstGA>(members,func,
        iniD,step,endD,low,up,nVals,varProb, cross,
        lowerBound, depth);
      cout << "GP example ";
      cout << "[P:"<<members<<", MR: "<<mutRate<<"]\n";

      avgs = ga->evolve(mutRate,epochs,true);
      cout << "Needed ites: " << avgs.size() << endl << endl;

      end = chrono::system_clock::now(); seconds = end-start;
      minLength = ga->getMinFitness();
      duration = seconds.count();

      for (int i=0; i<avgs.size(); i++) {
        maxAvg=max(maxAvg,(float)avgs[i]);
        avgCoords.push_back({(float)i,(float)avgs[i]}); 
      }

      float minFun=func(iniD),
            maxFun=func(iniD);
      for (float i=iniD; i<endD; i+=step) {
        float funVal = func(i);
        minFun=min(minFun,funVal);
        maxFun=max(maxFun,funVal);
        domVal1.push_back({i,ga->compute({{'x',i}})});
        domVal2.push_back({i,funVal}); 
      }

      {
        shared_ptr<Plot> evoChart(new Plot());
        evoChart->addPlotData(domVal2, false);
        evoChart->addPlotData(domVal1, true);
        evoChart->setTicks(4,4);
        evoChart->setXLabel("X");
        evoChart->setYLabelRotate(false);
        evoChart->setYLabel("Y");
        ssac << minLength; ssec << duration;
        evoChart->setTitle(
            string("Approximate vs True Function"));
        ssac.str(string()); ssec.str(string());
        evoChart->setLimits({min(iniD,0.f),endD,
            min(0.f,minFun),maxFun});
        gpPlotter->addPlot(evoChart);
      }
      {
        shared_ptr<Plot> evoChart(new Plot());
        evoChart->addPlotData(avgCoords, true);
        evoChart->setTicks(4,4);
        evoChart->setXLabel("Epoch");
        evoChart->setYLabel("Avg. Error");
        ssac << minLength; ssec << duration;
        evoChart->setTitle(string("Average Error vs Epoch"));
        ssac.str(string()); ssec.str(string());
        evoChart->setLimits({0,(float)avgs.size(),0,maxAvg});
        gpPlotter->addPlot(evoChart);
      }

      ssac << mutRate;
      gpPlotter->plot(
        string("GP Funcion approximation x⁴+x²+x+3 [")+
      string("E:")+to_string(epochs)+string(", ")+
      string("P:")+to_string(members)+string(", ")+
      string("MR:")+ssac.str()+string("]"));
    }
  }
}

