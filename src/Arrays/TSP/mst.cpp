#include <bits/stdc++.h>
#include <TSP.h>
using namespace std;




vector<pair<double,double> > vertex;
vector<pair<double,pair<int,int> > > edges;
vector<vector<int> > neighbors;
vector<int> path;
vector<bool> vis;



// Distance

static double dist(int p1, int p2){

  return sqrt(pow(vertex[p1].first-vertex[p2].first,2) +
              pow(vertex[p1].second-vertex[p2].second,2));
}



// Dfs

void dfs( int nodo ){

    vis[nodo] = 1;
    for( int i = 0; i < neighbors[nodo].size(); i++ ){
        if( !vis[neighbors[nodo][i]] ){
            path.push_back( neighbors[nodo][i] );
            dfs( neighbors[nodo][i] );
        }
    }
}


// RepSet Class

class RepSet {

    private:
        vector<int> _list;
        static vector<RepSet*>* setList;

    public:
        RepSet(int rep){
            _list.push_back( rep );
        }

        static RepSet* findSet(int x){
            return (*setList)[x];
        }
        static void initSets(int n){
            for(int i = 0; i < n; i++ ){
                setList->push_back( new RepSet(i) );
            }
        }
        
        vector<int>* list(){
            return &_list;
        }
        
        void unir( RepSet* aSet ){

            for( int i = 0; i < _list.size(); i++ ){
    
                (*setList)[_list[i]] = aSet;
                aSet->list()->push_back( _list[i] );
            }
        }
};

vector<RepSet*> the_setList;
vector<RepSet*>* RepSet::setList = &the_setList;

pair<vector<vector<float>>,float> mstRun(string filename) {

  int du, i, j, u, v, n=0;
  double x, y, d, sum = 0;

    

    // Get vertex

    ifstream nodefile(filename);

    while (nodefile>>x){
      nodefile >>  y;
      vertex.push_back(make_pair(x,y));
      vis.push_back( false );
      neighbors.push_back(vector<int>());
      n++;
    }
    nodefile.close();


    RepSet::initSets(n);
    // Create edges

    for (i = 0; i < n-1; i++){
      for (j = i+1; j < n; j++){
        d = dist(i, j);
        edges.push_back( make_pair(d,make_pair(i,j)) );
      }
    }


    // Start
    
    
   //clock_t begin = clock();


    // Sort by length

    sort( edges.begin(), edges.end() );



    // Kruskal

    RepSet *s1, *s2;

    for( int i = 0; i < edges.size(); i++ ){
    
        u = edges[i].second.first;
        v = edges[i].second.second; 
        s1 = RepSet::findSet(u);
        s2 = RepSet::findSet(v);        

        if( s1 != s2 ){

            s1->unir(s2);
            neighbors[u].push_back(v);
            neighbors[v].push_back(u);
        }

    }

    // Dfs
    
    srand( time(NULL) );
    int nodo = rand() % n;
    path.push_back( nodo );
    dfs( nodo );


    // Sum
    
    for( int  i = 0; i < n-1; i++ )
        sum += dist( path[i], path[i+1] );
    sum += dist( path[n-1], path[0] );

    vector<vector<float>> ppath;
    for (int i=0; i<path.size(); i++)
      ppath.push_back({(float)vertex[path[i]].first,
          (float)vertex[path[i]].second});

    //cout << sum << endl;
    return make_pair(ppath,(float)sum);

    //clock_t end = clock();
    //cout << "Time: "
    //<<(double(end-begin)/CLOCKS_PER_SEC) << endl;

}

// Main Function

/*int main( int argc, char* argv[] ){

  runMST();


    return 0;
}*/
