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
    int members=30, bits=20, tSize=10; double mutRate = 0.1;
    auto ga = make_unique<FindBitArrayGA>(members,bits);
    cout << "FindBitArray example\n";
    cout << "Needed ites: " << ga->evolve(mutRate,tSize) << endl;
  }
  {
    int members=30, bits=3, tSize=10; double mutRate = 0.1;
    auto ga = make_unique<FindCharArrayGA>(members,bits);
    cout << "FindCharArray example\n";
    cout << "Needed ites: " << ga->evolve(mutRate,tSize) << endl;
  }
}
