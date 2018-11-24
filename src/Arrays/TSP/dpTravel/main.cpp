#include <bits/stdc++.h>
using namespace std;



// Global Variables

int i, j, k, val, n, l;
double menor, x, y;
vector<pair<double,double> > v;

// Distance

double dist( int p1, int p2 ){
  return sqrt(pow(v[p1].first-v[p2].first,2) +
        pow(v[p1].second-v[p2].second,2));
}

static int getSubSetLine(int i, int j){
  return i & ~(1 << j);
}

static bool isTwoPower(int n){
  int log = log2(n);
  return pow(2, log) == n;
}

static void setMenor(double &menor, double val1, double val2){

  if (val1 != -1){
    if (menor == -1)
      menor = val1 + val2;
    else
      menor = min(menor, val1+val2);
  }
}

static void dpIte( vector<vector<double>> &dp,
            vector<vector<double>> mp,
            int i, int j ){

    l = getSubSetLine(i, j);
    if (l==i) return;

    menor = -1;
    for (k = 0; k<n; k++)
      setMenor(menor, dp[l][k], mp[k+1][j+1]);
    dp[i][j] = menor;
}

static void initDp(vector<vector<double>> &dp,
                   vector<vector<double>> mp){
  int potencia = 1;
  for (i = 0; i<n; i++) {
    dp[potencia][i] = mp[0][i+1];
    potencia *= 2;
  }
}


int main(int argc, char* argv[]){

  n = 15;
  vector<vector<double>> dp(pow(2,n-1),
      vector<double>(n-1,-1)),
      mp(n, vector<double>(n));

  ifstream nodefile("cities.txt");
  for (i = 0; i < n; ++ i) {
    //scanf("%lf %lf", &x, &y);
    nodefile >> x >> y;
    v.push_back(make_pair(x,y));
  }
  nodefile.close();

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      mp[i][j] = dist(i, j);
    }
  }

  n--;
  int height = pow(2,n);

  // Start

  clock_t begin = clock();

  initDp(dp, mp);
  for (i=1; i<height; i++) {
    if (!isTwoPower(i)){
      for (j=0; j<n; j++)
        dpIte(dp, mp, i, j);
    }
  }

  menor = -1;
  height--;

  for (i = 0; i < n; i++){
    setMenor(menor, dp[height][i], mp[i+1][0]);
  }

  cout << menor << endl;

  clock_t end = clock();
  cout << "Time: " << (double(end-begin) / CLOCKS_PER_SEC);
  cout << endl;

  return 0;
}
