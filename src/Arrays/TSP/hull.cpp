#include <TSP.h>
#include <list>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <set>
#include <fstream>
using namespace std;

static list<int>::iterator vMin, hVer, plVer;
static vector<bool> in;
static vector<pair<double,double>> v;

// Distance
static double dist(int p1, int p2){
  return sqrt(pow(v[p1].first-v[p2].first,2) +
            pow(v[p1].second-v[p2].second,2)); }

// Cross product
long long cross(int k, int i, int j){
    return (v[i].first-v[k].first)*(v[j].second-v[k].second) -
           (v[i].second-v[k].second)*(v[j].first-v[k].first); }

bool arcComp( pair<list<int>::iterator,double> v1,
        pair<list<int>::iterator,double> v2 ){
  if (v1.second < v2.second) return true;
  if (v1.second > v2.second) return false;
  else return *v1.first < *v2.first;
}

// Convex hull
vector<int> convHull(int n){

  int k = 0, t;
  vector<int> h(2*n);

  sort( v.begin(), v.end() );
  for( int i = 0; i < n; i++ ){
      while( k >= 2 && cross( h[k-2], h[k-1], i ) <= 0 ) k--;
      h[k++] = i; 
  }

  t = k+1;
  for (int i = n-2; i >= 0; i--){
    while (k>=t && cross(h[k-2],h[k-1],i)<=0) k--;
    h[k++] = i;
  }

  h.resize(k);

  return h;
}

pair<vector<vector<float>>,float> hullRun(string filename) {

  in = vector<bool>();
  v = vector<pair<double,double>>();

  int du, i, j, n=0, pos;
  double x, y, d, sum = 0;

  // Get vertex
  ifstream nodefile(filename);
  while (nodefile>>x) {
    
    nodefile >> y;
    v.push_back(make_pair(x,y));
    in.push_back(false);
    n++;
  }
  nodefile.close();


  // Start
  //clock_t begin = clock();

  vector<int> h = convHull(n);
  list<int> hull = list<int>( h.begin(), h.end() ),
      pointsLeft;

  // Preparing for loop
  double min;

  for( hVer = hull.begin(); hVer != hull.end(); hVer++ ){
    in[*hVer] = true;
  }


  for( i = 0; i < v.size(); i++ ){
    if( !in[i] ){
      pointsLeft.push_back( i );
    }
  }

  bool(*arcComp_p)(
          pair<list<int>::iterator,double>,
          pair<list<int>::iterator,double>) = arcComp;

  vector<set<pair<list<int>::iterator,double>,
   bool(*)(pair<list<int>::iterator,double>,
         pair<list<int>::iterator,double>)
     >> arcs(v.size(),
             set<pair<list<int>::iterator,double>,
             bool(*)(pair<list<int>::iterator,double>,
                     pair<list<int>::iterator,double>)>(arcComp));

  vector<vector<double>> table(v.size(),vector<double>(v.size()));


  for(plVer=pointsLeft.begin(); plVer!=pointsLeft.end(); plVer++){
    for (hVer=hull.begin(); next(hVer)!=hull.end(); hVer++) {
        
      d = dist(*hVer, *plVer) + 
          dist(*plVer, *next(hVer)) -
          dist(*hVer, *next(hVer));

      arcs[*plVer].insert( make_pair(hVer,d) );
      table[*plVer][*hVer] = d;
    }
  }


  // Loop
  
  while (pointsLeft.size()) {
  
    min = -1;
    vMin = pointsLeft.end();
    vector<pair<list<int>::iterator,list<int>::iterator>> vePairs;


// O(n^2): can be improved

    for (plVer=pointsLeft.begin(); 
      plVer!=pointsLeft.end(); plVer++){

      vePairs.push_back(make_pair(
                  arcs[*plVer].begin()->first,
                  plVer));
    }

    // Use pairs

    min = -1;
    for( i = 0; i < vePairs.size(); i++ ){
    
      d=(dist(*(vePairs[i].second),*(vePairs[i].first)) +
          dist(*(vePairs[i].first),*next(vePairs[i].second))) /
          dist(*(vePairs[i].second),*next(vePairs[i].second));

      if (d<min || min==-1) {
        min = d;
        pos = i;
      }
    }


    hVer = vePairs[pos].first;
    vMin = vePairs[pos].second;

    hull.insert( next(hVer), *vMin );
   

    for (plVer=pointsLeft.begin(); 
        plVer!=pointsLeft.end(); plVer++){
        
      arcs[*plVer].erase(make_pair(hVer,table[*plVer][*hVer]));

      d = dist(*hVer, *plVer) + 
          dist(*plVer, *vMin) -
          dist(*hVer, *vMin);

      table[*plVer][*hVer] = d;
      arcs[*plVer].insert(make_pair(hVer,d));

      d = dist(*vMin, *plVer) + 
          dist(*plVer, *next(next(hVer))) -
          dist(*next(next(hVer)), *vMin);

      table[*plVer][*vMin] = d;
      arcs[*plVer].insert(make_pair(next(hVer),d));
    }
    
    pointsLeft.erase( vMin );

  }


  // Return 

  for (hVer=hull.begin(); next(hVer)!=hull.end(); hVer++)
      sum += dist(*hVer, *(next(hVer)));

  vector<int> hindx{begin(hull), end(hull)};
  vector<vector<float>> path;
  for (int i=0; i<v.size(); i++)
    path.push_back({(float)v[hindx[i]].first,
        (float)v[hindx[i]].second});

  pair<vector<vector<float>>,float> retPair = 
    make_pair(path,(float)sum);

  //cout << sum << endl;
  return retPair;

  //clock_t end = clock();
  //cout << "Time: " << (double(end-begin)/CLOCKS_PER_SEC) <<
  //endl;

}


// Main
/*int main(int argc, char* argv[]){

  hullRun();

  return 0;
}*/
