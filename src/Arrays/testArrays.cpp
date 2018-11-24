#include <GP.h>
#include <iostream>
using namespace std;

template <class T> void printVec(const vector<T> &v) {
  for (int i=0; i<v.size(); i++)
    cout << v[i] << ' ';
  cout << endl;
} 

int main() {

  {
    int members=30, bits=20, tSize=10; double mutRate = 0.2;
    bool verbose = false;
    auto ga = make_unique<FindBitArrayGA>(members,bits);
    cout << "FindBitArray example\n";
    int ites = ga->evolve(mutRate,tSize);
    cout << "Needed ites: " << ites << endl << endl;
  }
  {
    int members=30, bits=3, tSize=10; double mutRate = 0.2;
    bool verbose = false;
    auto ga = make_unique<FindCharArrayGA>(members,bits);
    cout << "FindCharArray example\n";
    int ites = ga->evolve(mutRate,tSize);
    cout << "Needed ites: " << ites << endl << endl;
  }
  {/*
    int members=200000,  bits=30, ktil=2, epochs=50;
    double mutRate=0.5; bool verbose = true;
    auto ga = make_unique<FindPathGA>(members,bits);
    cout << "FindPath example\n";
    int ites = ga->evolve(mutRate,ktil,epochs,verbose);
    cout << "Needed ites: " << ites << endl << endl;*/
  }
  {
    int members=100, bits=25, ktil=2, epochs=100;
    double mutRate=0.5; bool verbose = true;
    auto ga = make_unique<FindTravelerGA>(members,bits,
        string("cities/cities1.txt"));
    cout << "FindTravel example\n";
    int ites = ga->evolve(mutRate,ktil,epochs,verbose);
    cout << "Needed ites: " << ites << endl << endl;
  }
}
