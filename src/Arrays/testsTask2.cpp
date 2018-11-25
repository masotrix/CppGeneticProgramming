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

int main() {

  bool verbose = false;
  chrono::time_point<chrono::system_clock> start,end;
  chrono::duration<double> seconds;
  static default_random_engine gen;


  {
    cout << "Total Population:\n";

    int members[] = {700,800,900,1000};
    int cities=25, popDivision=2, epochs=150;
    double mutRate=0.5;
    unique_ptr<FindTravelerGA> ga[4];
    vector<vector<float>> points[4];
    float minFitness[4];
    double duration[4];
    int tests=100;

    ofstream file; double x,y;
    uniform_real_distribution<float> coord(0,1);
    for (int i=0; i<tests; i++) {
      file.open(string("cities/cities")+to_string(i)+
          string(".txt"));
      for (int j=0; j<cities; j++) {
        x=round(coord(gen)*200); y=round(coord(gen)*200);
        file << x << ' ' << y << '\n';
      }
      file.close();
    }

    stringstream ssac; ssac.str(string());
    ssac << fixed << setprecision(1.f);
    stringstream ssec; ssec.str(string());
    ssec << fixed << setprecision(2.f);
    unique_ptr<Plotter> cityPlotter(new Plotter());
  
    for (int k=0; k<4; k++) {
      cout << "Conf"<<k+1<<"/4"<<": ";
      for (int i=0; i<tests; i++) {
        start = chrono::system_clock::now();
        ga[k] = make_unique<FindTravelerGA>(members[k],cities,
            string("cities/cities")+to_string(i)+string(".txt"));
        ga[k]->evolve(mutRate,popDivision,epochs,verbose);
        end = chrono::system_clock::now(); seconds = end-start;
        minFitness[k] += ga[k]->getMinFitness();
        duration[k] += seconds.count();
        if (i%25==0) {cout <<i<<"% "; cout.flush();}
      }
      cout << "100%\n";

      points[k] = ga[k]->getOptPoints(); 
      points[k].push_back(points[k][0]);
      minFitness[k] /= tests;
      duration[k] /= tests;

      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points[k], Scalar(255,0,0));
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minFitness[k]; ssec << duration[k];
      cityChart->setTitle(
          string("Population: ")+to_string(members[k])+
          string(" [D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }
    
    cityPlotter->plot(
      string("Average Statistics and random example for ")+
      string("Epochs: 150, Mutation Rate: 0.5, ")+
      string("Ep. Division: 2, Population x ")+
      string("[Average Distance (D), Average Time Spent (T)]"));

  }
  {
    cout << "Population Division:\n";

    int members[] = {900,900,900,900};
    int cities=25, popDivision[]={2,3,4,5},
        epochs[]={150,150,150,150};
    double mutRate=0.5;
    unique_ptr<FindTravelerGA> ga[4];
    vector<vector<float>> points[4];
    float minFitness[4];
    double duration[4];
    int tests=100;

    ofstream file; double x,y;
    uniform_real_distribution<float> coord(0,1);
    for (int i=0; i<tests; i++) {
      file.open(string("cities/cities")+to_string(i)+
          string(".txt"));
      for (int j=0; j<cities; j++) {
        x=round(coord(gen)*200); y=round(coord(gen)*200);
        file << x << ' ' << y << '\n';
      }
      file.close();
    }

    stringstream ssac; ssac.str(string());
    ssac << fixed << setprecision(1.f);
    stringstream ssec; ssec.str(string());
    ssec << fixed << setprecision(2.f);
    unique_ptr<Plotter> cityPlotter(new Plotter());
  
    for (int k=0; k<4; k++) {
      cout << "Conf"<<k+1<<"/4"<<": ";
      for (int i=0; i<tests; i++) {
        start = chrono::system_clock::now();
        ga[k] = make_unique<FindTravelerGA>(members[k],cities,
            string("cities/cities")+to_string(i)+string(".txt"));
        ga[k]->evolve(mutRate,popDivision[k],epochs[k],verbose);
        end = chrono::system_clock::now(); seconds = end-start;
        minFitness[k] += ga[k]->getMinFitness();
        duration[k] += seconds.count();
        if (i%25==0) {cout <<i<<"% "; cout.flush();}
      }
      cout << "100%\n";

      points[k] = ga[k]->getOptPoints(); 
      points[k].push_back(points[k][0]);
      minFitness[k] /= tests;
      duration[k] /= tests;

      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points[k], Scalar(255,0,0));
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minFitness[k]; ssec << duration[k];
      cityChart->setTitle(
          string("Pop division: ")+to_string(popDivision[k])+
          string(" [D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }
    
    cityPlotter->plot(
      string("Average Statistics and random example for ")+
      string("Epochs: 150, Mutation Rate: 0.5, ")+
      string("Population 900, Ep. Division x ")+
      string("[Average Distance (D), Average Time Spent (T)]"));

  }
  
  {
    cout << "Algorithm comparison\n";

    int members[] = {900,900,900,900};
    int cities=25, popDivision[]={2,2,2,2},
        epochs[]={150,150,150,150};
    double mutRate=0.5; float sum;
    unique_ptr<FindTravelerGA> ga[4];
    vector<vector<float>> points[4];
    float minFitness[]={0,0,0,0};
    double duration[]={0,0,0,0};
    for (int i=0; i<4; i++) {
      minFitness[i]=0; duration[i]=0;
    }
    int tests=100;

    ofstream file; double x,y;
    uniform_real_distribution<float> coord(0,1);
    for (int i=0; i<tests; i++) {
      file.open(string("cities/cities")+to_string(i)+
          string(".txt"));
      for (int j=0; j<cities; j++) {
        x=round(coord(gen)*200); y=round(coord(gen)*200);
        file << x << ' ' << y << '\n';
      }
      file.close();
    }

    stringstream ssac; ssac.str(string());
    ssac << fixed << setprecision(1.f);
    stringstream ssec; ssec.str(string());
    ssec << fixed << setprecision(4.f);
    unique_ptr<Plotter> cityPlotter(new Plotter());
 
    { 
      cout << "Conf"<<1<<"/3"<<": ";
      for (int i=0; i<tests; i++) {
        start = chrono::system_clock::now();
        ga[0] = make_unique<FindTravelerGA>(members[0],cities,
            string("cities/cities")+to_string(i)+string(".txt"));
        ga[0]->evolve(mutRate,popDivision[0],epochs[0],verbose);
        end = chrono::system_clock::now(); seconds = end-start;
        minFitness[0] += ga[0]->getMinFitness();
        duration[0] += seconds.count();
        if (i%25==0) {cout <<i<<"% "; cout.flush();}
      }
      cout << "100%\n";

      points[0] = ga[0]->getOptPoints(); 
      points[0].push_back(points[0][0]);
      minFitness[0] /= tests;
      duration[0] /= tests;

      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points[0], Scalar(255,0,0));
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minFitness[0]; ssec << duration[0];
      cityChart->setTitle(
          string("Genetic TSP ")+
          string("[D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }
    { int k=1;
      cout << "Conf"<<2<<"/3"<<": ";
      for (int i=0; i<tests; i++) {
        start = chrono::system_clock::now();
        tie(points[k],sum) = hullRun(
          string("cities/cities")+to_string(i)+string(".txt"));
        end = chrono::system_clock::now(); seconds = end-start;
        minFitness[k] += sum;
        duration[k] += seconds.count();
        if (i%25==0) {cout <<i<<"% "; cout.flush();}
      }
      cout << "100%\n";

      points[k].push_back(points[k][0]);
      minFitness[k] /= tests;
      duration[k] /= tests;

      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points[k], Scalar(255,0,0));
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minFitness[k]; ssec << duration[k];
      cityChart->setTitle(
          string("Convex Hull TSP ")+
          string("[D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }
    { int k=2;
      cout << "Conf"<<3<<"/3"<<": ";
      for (int i=0; i<tests; i++) {
        start = chrono::system_clock::now();
        tie(points[k],sum) = mstRun(
            string("cities/cities")+to_string(i)+string(".txt"));
        end = chrono::system_clock::now(); seconds = end-start;
        minFitness[k] += sum;
        duration[k] += seconds.count();
        if (i%25==0) {cout <<i<<"% "; cout.flush();}
      }
      cout << "100%\n";

      points[k].push_back(points[k][0]);
      minFitness[k] /= tests;
      duration[k] /= tests;

      shared_ptr<Plot> cityChart(new Plot());
      cityChart->addPlotData(points[k], Scalar(255,0,0));
      cityChart->setTicks(4,4);
      cityChart->setXLabel("X"); cityChart->setYLabel("Y");
      cityChart->setYLabelRotate(false);
      ssac << minFitness[k]; ssec << duration[k];
      cityChart->setTitle(
          string("Minimum ST TSP ")+
          string("[D:")+ssac.str()+
          string(", T:")+ssec.str()+string("]"));
      ssac.str(string()); ssec.str(string());
      cityChart->setLimits({-10,300,-10,300});
      cityPlotter->addPlot(cityChart);
    }

    
    cityPlotter->plot(
      string("Average Statistics and random example for ")+
      string("TSP GA [Epochs: 150, Mutation Rate: 0.5, ")+
      string("Population 900, Ep. Division x] vs ")+
      string("Convex Hull and MST TSPs"));

  }
}
