#include <GP.h>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>
using namespace std;

static default_random_engine gen;

/*
template <class T> GeneticAlgorithm::GeneticAlgorithm(int N):
  _members(vector<unique_ptr<Member>>(N)),
  _offspring(vector<unique_ptr<Member>>(0)) {
  
  for (int i=0; i<N; i++) {
    auto member = make_unique<T>();
    _members.push_back(move(member));
  }  
}
*/
/*
void GeneticAlgorighm::evaluate() {
  for (auto mem: _members) mem->evaluate(_fitness);
}*/
template <class T>
int GeneticAlgorithm<T>::select(int k,
    function<bool(float,float)> comp) {
  int selected, best=-1;
  uniform_int_distribution<int> member(0,this->_members.size()-1);
  for (int i=0; i<k; i++) {
    selected = member(gen);
    if (best==-1 ||
        comp(this->_members[selected]->getFitness(),
        this->_members[best]->getFitness()))
      best = selected;
  }
  return best;
}
template <class T>
float GeneticAlgorithm<T>::getMinFitness() {
  return _minFitness;
}
template <class T>
void GeneticAlgorithm<T>::print(int i) {
  this->_members[i]->print();
}
template <class T>
int GeneticAlgorithm<T>::evolve(double mutRate, int tourSize,
    int epochs, bool verbose) {
  float evalRes;
  for (int k=0;k<epochs;k++) {
    _epochs = k+1;
    evalRes = evaluate();
    if (evalRes!=-1.f || k==epochs-1) break;
    if (verbose && k%(epochs/10)==0) {
      cout << _epochs << ": MinFit: " << _minFitness 
          << ", StdFit: " << _std << endl;
    }
    reproduce(mutRate,tourSize);
  }
  if (verbose) {
    cout << "Best Member: "; print(0);
    cout << "Final fitness: " << 
      this->_members[0]->getFitness() << endl;
  }
  return _epochs;
}

AstGA::AstGA(int N, function<float(float)> perfect,
    float ini, float step, float end, float lowerBound):
_perfectFunc(perfect), _ini(ini), _step(step), _end(end),
_lowerBound(lowerBound) {
  for (int i=0; i<N; i++) {
    auto mem = make_unique<AST>("5");
    this->_members.push_back(move(mem));
  }
}

void AstGA::reproduce(double mutRate, int tourRounds) {
  uniform_real_distribution<double> mutation(0,1);
  int selected1=this->select(tourRounds,less<float>()),
      selected2=this->select(tourRounds,less<float>());
  vector<unique_ptr<AST>> offspring;

  for (int k=0; k<this->_members.size(); k++) {
    auto ast = this->_members[selected1]->copy();
    ast->replaceRandom(this->_members[selected2]->polacRandom());
    if (mutation(gen)>mutRate)
    { /*ast->mutate();*/ }
    offspring.push_back(move(ast));
  }
  this->_members = move(offspring);
}

float AstGA::evaluate() {
  for (int i=0; i<this->_members.size(); i++) {
    float fitness = 0.f;
    for (float j=_ini; j<_end; j+=_step)
      fitness += abs(_perfectFunc(i)-
          this->_members[i]->compute({{'x',j}}));
    if (fitness < _lowerBound)
      return fitness;
    this->_members[i]->setFitness(fitness);
  }
  sort(_members.begin(),_members.end(),
      [](const unique_ptr<AST> &a,
         const unique_ptr<AST> &b) {
        return (a->getFitness() < b->getFitness());});
  return -1.f;
}
template <class T, class K>
ArrayGA<T,K>::ArrayGA(int N, int B): ArrayGA<T,K>(N,B,B) {}
template <class T, class K>
ArrayGA<T,K>::ArrayGA(int N, int B, int ops): 
  _perfectArray(vector<T>(B)), _options(ops)  {
  for (int i=0; i<N; i++) {
    auto mem = make_unique<K>(B);
    this->_members.push_back(move(mem));
  }
  uniform_int_distribution<int> option(0,_options-1);
  for (int i=0; i<B; i++) _perfectArray[i] = option(gen);
}
template <class T> static int matchArray(
  const vector<T>&v1,const vector<T>&v2) {

    int right = 0;
    for (int i=0; i<v1.size(); i++)
      right += v1[i]==v2[i];
    return right;
  };
template <class T, class K>
float ArrayGA<T,K>::evaluate() {
  float maxFit=0;
  for (int i=0; i<this->_members.size(); i++) {
    float fitness = 
      matchArray<T>(this->_members[i]->getArray(),_perfectArray);
    maxFit=max(maxFit,fitness);
    this->_members[i]->setFitness(fitness);
  }
  sort(this->_members.begin(),this->_members.end(),
      [](const unique_ptr<K> &a,
         const unique_ptr<K> &b) {
        return (a->getFitness() > b->getFitness());});
  return maxFit==_perfectArray.size()? maxFit:-1.f;
}
template <class T, class K>
void ArrayGA<T,K>::reproduce(double mutRate, int tourRounds) {
  uniform_int_distribution<int> option(0,_options-1), coin(0,1);
  uniform_real_distribution<double> mutation(0,1);
  uniform_int_distribution<int> cross(0,_perfectArray.size()-1);
  int selected1=this->select(tourRounds),
      selected2(this->select(tourRounds));
  vector<unique_ptr<K>> offspring;
  for (int k=0; k<this->_members.size(); k++) {
    vector<T> new_array(_perfectArray.size(),0);
    for (int i=0; i<new_array.size(); i++)
      new_array[i]=(coin(gen)?
          this->_members[selected1]->getArray()[i]:
          this->_members[selected2]->getArray()[i]);
    if (mutation(gen)<mutRate)
        new_array[cross(gen)]=option(gen);
    auto baby = make_unique<K>(new_array);
    offspring.push_back(move(baby));
  }
  this->_members = move(offspring);
}
FindBitArrayGA::FindBitArrayGA(int N, int B): 
  ArrayGA<bool,BitArrayMember>(N,B,2) {
}
FindCharArrayGA::FindCharArrayGA(int N, int B): 
  ArrayGA<char,CharArrayMember>(N,B,256) {
}
FindTravelerGA::FindTravelerGA(int N, int B, string filename):
  ArrayGA<int,TravelMember>(N,B),
  _points(move(make_unique<vector<vector<float>>>())) {

  ifstream file(filename);
  float x, y;
  for (int i=0; i<B; i++) {
    file >> x >> y;
    _points->push_back({x,y});
  }
  file.close();
}
vector<vector<float>> FindTravelerGA::getOptPoints() {
  
  auto optIdx = this->_members[0]->getArray();
  vector<vector<float>> optPoints(optIdx.size());

  for (int i=0; i<optIdx.size(); i++)
    optPoints[i] = (*_points)[optIdx[i]];

  return optPoints;

}
float FindTravelerGA::evaluate() {
  int B = this->_members[0]->getArray().size();
  double varN=0; long totFit=0;
  float minFit = numeric_limits<float>::max();
  for (int i=0; i<this->_members.size(); i++) {
    float fitness = 0;
    int size = this->_members[i]->getArray().size();
    for (int j=0; j<size; j++) {
      fitness += sqrt(
          pow((*_points)[this->_members[i]->getArray()[j]][0]-
        (*_points)[this->_members[i]->getArray()[(j+1)%B]][0],2)+
          pow((*_points)[this->_members[i]->getArray()[j]][1]-
        (*_points)[this->_members[i]->getArray()[(j+1)%B]][1],2));
    }

    minFit = min(minFit,fitness);
    this->_members[i]->setFitness(fitness);

    double prevAvg = i? ((double)totFit)/i:0;
    totFit += fitness;
    double curAvg = ((double)totFit)/(i+1);
    varN += (fitness-prevAvg)*(fitness-curAvg);
  }
  _std = sqrt(varN/_epochs);
  _minFitness = minFit;
  sort(_members.begin(),_members.end(),
      [](const unique_ptr<TravelMember> &a,
         const unique_ptr<TravelMember> &b) {
        return (a->getFitness() < b->getFitness());});
  return -1.f;
}
void FindTravelerGA::reproduce(double mutRate, int tourRounds) {
  int B = _perfectArray.size();
  uniform_int_distribution<int> option(0,_options-1), coin(0,1);
  uniform_real_distribution<double> mutation(0,1);
  uniform_int_distribution<int> cross(0,B-1);

  vector<unique_ptr<TravelMember>> offspring;
  for (int k=0; k<this->_members.size(); k++) {
    int selected1=this->select(tourRounds),
        selected2=this->select(tourRounds);

    vector<int> vs1 = this->_members[selected1]->getArray(),
      vs2 = this->_members[selected2]->getArray();

    vector<int> new_array(_perfectArray.size(),0);
    int crossIni=cross(gen), crossEnd=cross(gen);
    crossIni=min(crossIni,crossEnd);
    crossEnd=max(crossIni,crossEnd);
    set<int> crossed;
    for (int i=crossIni; i<crossEnd; i++)
      crossed.insert(vs2[i]);

    int new_pos=0, j=0;
    while (new_pos < crossIni) {
      if (crossed.find(vs1[j])==crossed.end())
        new_array[new_pos++]=vs1[j];
      j++;
    }
    while (new_pos<crossEnd) {
      new_array[new_pos]=vs2[new_pos];
      new_pos++;
    }
    while (new_pos<vs1.size()) {
      if (crossed.find(vs1[j])==crossed.end())
        new_array[new_pos++]=vs1[j];
      j++;
    }

    if (mutation(gen)<mutRate) {
      int mutPoint = cross(gen);
      swap(new_array[mutPoint],new_array[(mutPoint+1)%B]);
    }
    auto baby = make_unique<TravelMember>(new_array);
    offspring.push_back(move(baby));
  }
  this->_members = move(offspring);
}
int FindTravelerGA::select(int k,
    function<bool(float,float)> comp) {
  int selected, best=-1;
  uniform_int_distribution<int> member(0,
      this->_members.size()/k);
  return member(gen);
}
FindPathGA::FindPathGA(int N, int B):
    
  ArrayGA<int,PathMember>(N,B,5),
  _labyrinth(move(make_unique<vector<vector<char>>>())) {

  _punish = 10; _minFitness = numeric_limits<float>::max();
  ifstream file("map.txt");
  char val;
  file >> _rows >> _cols;
  for (int i=0; i<_rows; i++) {
    _labyrinth->push_back(vector<char>(_cols));
    for (int j=0; j<_cols; j++) {
      do {file.get(val);} while (val=='\n');
      (*_labyrinth)[i][j] = val;
    }
  }
  file.close();

  for (int i=0; i<_rows; i++) {
    for (int j=0; j<_cols; j++) {
      if ((*_labyrinth)[i][j]=='P') {
        _inix=i; _iniy=j; (*_labyrinth)[i][j]=' '; }
      if ((*_labyrinth)[i][j]=='F') {
        _endx=i; _endy=j; (*_labyrinth)[i][j]=' '; }
    }
  }
}
int FindPathGA::select(int k,
    function<bool(float,float)> comp) {
  int selected, best=-1;
  uniform_int_distribution<int> member(0,
      this->_members.size()/k);
  return member(gen);
}
void FindPathGA::print(int k) {
  vector<vector<char>> charMap = (*_labyrinth);
  int curx,cury,faults=0;
  curx=_inix; cury=_iniy;
  for (int j=0; j<this->_members[k]->getArray().size(); j++) {
    switch (_members[k]->getArray()[j]) {
      case 0: break;
      case 1: if (curx+1<_rows) curx+=1; break;
      case 2: if (cury+1<_cols) cury+=1; break;
      case 3: if (curx-1>=0) curx-=1; break;
      case 4: if (cury-1>=0) cury-=1; break;
    }
    if ((*_labyrinth)[curx][cury]==' ') charMap[curx][cury] = '.';
    else {charMap[curx][cury] = 'X'; faults++;};
  }

  for (int i=0; i<_rows; i++) {
    for (int j=0; j<_cols; j++)
      cout << charMap[i][j];
    cout << endl;
  }
  for (int j=0; j<this->_members[k]->getArray().size(); j++)
    cout << this->_members[k]->getArray()[j];
  cout << endl;
  cout << "Final fitness: " << 
    this->_members[k]->getFitness() << endl;
}
float FindPathGA::evaluate() {
  int curx,cury; int faults; double varN=0; long totFit=0;
  float minFit = numeric_limits<float>::max();
  for (int i=0; i<this->_members.size(); i++) {
    curx=_inix; cury=_iniy; faults=0;
    for (int j=0; j<this->_members[0]->getArray().size(); j++) {
      switch (_members[i]->getArray()[j]) {
        case 0: break;
        case 1: 
          if (curx+1<_rows) curx+=1;
          else faults++;
          break;
        case 2: if (cury+1<_cols) cury+=1;
          else faults++;
          break;
        case 3: if (curx-1>0) curx-=1;
          else faults++;
          break;
        case 4: if (cury-1>0) cury-=1;
          else faults++;
          break;
      }
      if ((*_labyrinth)[curx][cury]=='#')
        faults++;
    }
    float fitness =
      pow(curx-_endx,2)+pow(cury-_endy,2)
        +faults*_punish;
    this->_members[i]->setFitness(fitness);
    
    double prevAvg = i? ((double)totFit)/i:0;
    totFit += fitness;
    double curAvg = ((double)totFit)/(i+1);
    varN += (fitness-prevAvg)*(fitness-curAvg);

    minFit = min(minFit,fitness);
    if (!fitness) break;
  }
  _minFitness = minFit;
  _std = sqrt(varN/_epochs);
  sort(this->_members.begin(),this->_members.end(),
      [](const unique_ptr<PathMember> &a,
         const unique_ptr<PathMember> &b) {
        return (a->getFitness() < b->getFitness());});
  return minFit==0?0:-1.f;
}
void FindPathGA::reproduce(double mutRate, int tourRounds) {
  uniform_int_distribution<int> option(0,_options-1), coin(0,1);
  uniform_real_distribution<double> mutation(0,1);
  uniform_int_distribution<int> cross(0,_perfectArray.size()-1);
  vector<unique_ptr<PathMember>> offspring;
  for (int k=0; k<this->_members.size(); k++) {
    int selected1=this->select(tourRounds),
        selected2=this->select(tourRounds);
    vector<int> new_array(_perfectArray.size(),0);
    int crossPoint = cross(gen);
    for (int i=0; i<crossPoint; i++)
      new_array[i]=this->_members[selected1]->getArray()[i];
    for (int i=crossPoint; i<new_array.size(); i++)
      new_array[i]=this->_members[selected2]->getArray()[i];
    if (mutation(gen)<mutRate)
        new_array[cross(gen)]=option(gen);
    auto baby = make_unique<PathMember>(new_array);
    offspring.push_back(move(baby));
  }
  this->_members = move(offspring);
}



int Member::getFitness() {return _fitness;}
void Member::setFitness(int fitness) {_fitness=fitness;}
template <class T>
ArrayMember<T>::ArrayMember(int B, int options):
  _array(vector<T>(B)) {
  uniform_int_distribution<int> udist(0,options-1);
  for (int i=0; i<_array.size(); i++) _array[i] = udist(gen); }
template <class T>
ArrayMember<T>::ArrayMember(int B): _array(vector<T>(B)) {
  for (int i=0; i<_array.size(); i++) _array[i] = i; 
  random_shuffle(_array.begin(), _array.end());  
}
template <class T>
ArrayMember<T>::ArrayMember(vector<T> v): _array(v) {}
template <class T>
void ArrayMember<T>::print() {
  for (int i=0; i<_array.size(); i++)
    cout << _array[i] << ' ';
  cout << endl;
}
template <class T>
const vector<T>& ArrayMember<T>::getArray() const{return _array;}
template <class T>
void ArrayMember<T>::setFitness(int fitness) { _fitness=fitness; }
template <class T>
int ArrayMember<T>::getFitness() { return _fitness; }
BitArrayMember::BitArrayMember(int B):
  ArrayMember<bool>(B, 2) {}
BitArrayMember::BitArrayMember(vector<bool> v):
  ArrayMember<bool>(v) {}
CharArrayMember::CharArrayMember(int B):
  ArrayMember<char>(B, 256) {}
CharArrayMember::CharArrayMember(vector<char> v):
  ArrayMember<char>(v) {}
TravelMember::TravelMember(int B):
  ArrayMember<int>(B) {}
TravelMember::TravelMember(vector<int> v):
  ArrayMember<int>(v) {}
PathMember::PathMember(int B):
  ArrayMember<int>(B, 5) {}
PathMember::PathMember(vector<int> v):
  ArrayMember<int>(v) {}
