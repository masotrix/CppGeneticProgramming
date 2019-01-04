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
vector<double> GeneticAlgorithm<T>::evolve(double mutRate,
    int epochs, bool verbose, int tourSize) {
  float evalRes; vector<double> avgs;
  for (int k=0;k<epochs;k++) {
    _epochs = k+1;
    evalRes = evaluate();
    avgs.push_back(this->_avg);
    if (evalRes!=-1.f || k==epochs-1) break;
    if (verbose && k%(epochs/10)==0) {
      cout << _epochs << ": MinFit: " << _minFitness 
          << ", StdFit: " << _std << endl;
    }

    reproduce(mutRate,tourSize);
  }
  sort(_members.begin(),_members.end(),
      [](const unique_ptr<T> &a,
         const unique_ptr<T> &b) {
        return (a->getFitness() < b->getFitness());});

  if (verbose) {
    if (_bestMember!=nullptr) {
      cout << "Best Member: "; _bestMember->print(true);
      cout << "Final fitness: "<<_bestMember->getFitness()<<endl;
    }
    else {
      int best = _members.size()-1;
      cout << "Best Member: "; _members[best]->print(true);
      cout << "Final fitness: "<<_members[best]->getFitness()
        <<endl;
    }
  }
  return avgs;
}

AstGA::AstGA(int N, function<float(float)> perfect,
    float ini, float step, float end,
    int low, int up, int nVals, float varProb, float cross,
    float lowerBound, int depth):
  _perfectFunc(perfect), _ini(ini), _step(step), _end(end),
  _lowerBound(lowerBound), _depth(depth), _varProb(varProb), 
  _cross(cross) {

  _curMinFitness = numeric_limits<float>::max();
  uniform_int_distribution<int> intDist(low,up);
  for (int i=0; i<nVals; i++)
    _vals.push_back(intDist(gen));

  for (int i=0; i<N; i++) {
    auto mem = make_unique<AST>(_depth, _vals, _varProb);
    this->_members.push_back(move(mem));
  }
}

float AstGA::compute(const map<char,float>&dic) {
  return this->_bestMember->compute(dic);
}
int AstGA::select(int k,
    function<bool(float,float)> comp) {
  uniform_int_distribution<int> parent(0,
      this->_parents.size()-1);
  return parent(gen);
}
/*
class BirnarySelectComp {
  public: 
    bool operator()(const AST& x, const AST& y) const
    { if (}
};
*/

void AstGA::selectParents() {
  vector<float> inv_fitness;
  for (int i=0; i<this->_members.size(); i++)
    inv_fitness.push_back(1/this->_members[i]->getFitness());
  sort(inv_fitness.begin(),inv_fitness.end(),
      [](float &a, float &b) {return (a < b);});
  
  vector<float> accinv_fitness;
  accinv_fitness.push_back(inv_fitness[0]);
  for (int i=1; i<this->_members.size(); i++)
    accinv_fitness.push_back(accinv_fitness[i-1]+inv_fitness[i]);

  vector<unique_ptr<AST>> parents;
  uniform_real_distribution<float> selector(0,
      accinv_fitness[accinv_fitness.size()-1]);
  vector<float>::iterator up;
  for (int i=0; i<this->_members.size(); i++) {
    float val = selector(gen);
    up = upper_bound(accinv_fitness.begin(),
        accinv_fitness.end(), val);
   /* if (up==accinv_fitness.end())
      exit(1);*/
    parents.push_back(move(make_unique<AST>(
          _members[up-accinv_fitness.begin()]->polac())));
  }
  this->_parents = move(parents);
}

void AstGA::reproduce(double mutRate, int tourRounds) {
  uniform_real_distribution<double> uni(0,1);
  vector<unique_ptr<AST>> offspring;
  selectParents();

  for (int k=0; k<this->_members.size(); k++) {
    int selected1=this->select(tourRounds),
        selected2=this->select(tourRounds);
   // auto ast = this->_parents[selected1]->copy();
    auto ast = make_unique<AST>(
        this->_parents[selected1]->polac());
    if (uni(gen)<_cross) {
      int depth2;
      //string polac = this->_parents[selected2]->
      string polac = this->_parents[selected2]->
        polacRandom(depth2);
      ast->replaceRandom(polac, _depth-depth2+1, _depth);
    }
    if (uni(gen)<mutRate)
    { ast->mutate(_depth,_vals,_varProb); }
    offspring.push_back(move(ast));
  }
  this->_members = move(offspring);
}

void AstGA::print(bool verbose) {

  for (int i=0; i<this->_members.size(); i++) {
    float fitness = 0.f;
    for (float j=_ini; j<=_end; j+=_step)
      fitness += abs(_perfectFunc(j)-
          this->_members[i]->compute({{'x',j}}));

    if (verbose) {
      cout << "Member " << i << ": ";
      this->_members[i]->print();
      this->_members[i]->setFitness(fitness);
      cout << ", Fitness: " << fitness << endl;
    } else {
      cout << i <<" Fitness: " << fitness << endl;
    }
  }
}

float AstGA::evaluate() {
  double varN=0; double totFit=0; this->_avg=0;
  float minFit = numeric_limits<float>::max();

  for (int i=0; i<this->_members.size(); i++) {
    float fitness = 0.f;
    for (float j=_ini; j<=_end; j+=_step)
      fitness += abs(_perfectFunc(j)-
          this->_members[i]->compute({{'x',j}}));
    this->_members[i]->setFitness(fitness);

    minFit = min(minFit,fitness);
    totFit += fitness;
    double curAvg = totFit/(i+1);
    varN += (fitness-this->_avg)*(fitness-curAvg);
    this->_avg = curAvg;
  }

  _std = sqrt(varN/_epochs);
  _minFitness = minFit;

  sort(_members.begin(),_members.end(),
      [](const unique_ptr<AST> &a,
         const unique_ptr<AST> &b) {
        return (a->getFitness() > b->getFitness());});

  int best = _members.size()-1;
  if (_members[best]->getFitness()<_curMinFitness) {
    _curMinFitness = _minFitness;
    _bestMember = make_unique<AST>(_members[best]->polac());
    _bestMember->setFitness(_members[best]->getFitness());
  }

  if (_minFitness < _lowerBound)
    return _minFitness;
  return -1.f;
}
template <class T, class K>
ArrayGA<T,K>::ArrayGA() {
  this->_bestMember = nullptr;
}
template <class T, class K>
ArrayGA<T,K>::ArrayGA(int N, int B): ArrayGA<T,K>(N,B,B) {}
template <class T, class K>
ArrayGA<T,K>::ArrayGA(int N, int B, int ops): 
  _perfectArray(vector<T>(B)), _options(ops)  {
  this->_bestMember = nullptr;
  this->_curMinFitness = numeric_limits<float>::max();
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
  float minFit = numeric_limits<float>::max();
  double varN=0; double totCost=0; this->_avg=0;
  for (int i=0; i<this->_members.size(); i++) {
    float fitness = 
      matchArray<T>(this->_members[i]->getArray(),_perfectArray);
    minFit=min(minFit,fitness);
    this->_members[i]->setFitness(fitness);

    totCost += fitness;
    double curAvg = totCost/(i+1);
    varN += (fitness-this->_avg)*(fitness-curAvg);
    this->_avg = curAvg;
  }
  sort(this->_members.begin(),this->_members.end(),
      [](const unique_ptr<K> &a,
         const unique_ptr<K> &b) {
        return (a->getFitness() > b->getFitness());});

  this->_std = sqrt(varN/this->_epochs);
  this->_minFitness = minFit;

  if (this->_minFitness > this->_curMinFitness)
    this->_curMinFitness = this->_minFitness;

  int best = this->_members.size()-1;
  /*if (this->_members[best]->getFitness()<this->_curMinFitness) {
    this->_bestMember = make_unique<K>(
        ((ArrayMember<K>)this->_members[best])->getArray());
    this->_bestMember->setFitness(
        this->_members[best]->getFitness());
  }*/

  return minFit==_perfectArray.size()? minFit:-1.f;
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
FindCharArrayGA::FindCharArrayGA(int N, int B)
{
  for (int i=0; i<N; i++) {
    auto mem = make_unique<CharArrayMember>(B);
    this->_members.push_back(move(mem));
  }
  uniform_int_distribution<int> option(97,122);
  _perfectArray = vector<char>(B);
  for (int i=0; i<B; i++) _perfectArray[i] = option(gen);
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
  
  int best = this->_members.size()-1;
  auto optIdx = this->_members[best]->getArray();
  vector<vector<float>> optPoints(optIdx.size());

  for (int i=0; i<optIdx.size(); i++)
    optPoints[i] = (*_points)[optIdx[i]];

  return optPoints;

}
float FindTravelerGA::evaluate() {
  int best = this->_members.size()-1;
  int B = this->_members[best]->getArray().size();
  double varN=0; double totCost=0; this->_avg=0;
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

    totCost += fitness;
    double curAvg = totCost/(i+1);
    varN += (fitness-this->_avg)*(fitness-curAvg);
    this->_avg = curAvg;
  }
  _std = sqrt(varN/_epochs);
  _minFitness = minFit;
  sort(_members.begin(),_members.end(),
      [](const unique_ptr<TravelMember> &a,
         const unique_ptr<TravelMember> &b) {
        return (a->getFitness() > b->getFitness());});
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
  uniform_int_distribution<int> member(
     (int)(this->_members.size()*((k-1)/(float)k)),
      this->_members.size()-1);
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
  uniform_int_distribution<int> member(
      (int)(this->_members.size()*(k-1)/(float)k),
      this->_members.size()-1);
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
  int best = this->_members.size()-1;
  int curx,cury; int faults; double varN=0; long totFit=0;
  float minFit = numeric_limits<float>::max();
  for (int i=0; i<this->_members.size(); i++) {
    curx=_inix; cury=_iniy; faults=0;
    for (int j=0;j<this->_members[best]->getArray().size();j++) {
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
    float fitness = pow(curx-_endx,2)+pow(cury-_endy,2)
        +faults*_punish;

    fitness = 1/fitness;
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
        return (a->getFitness() > b->getFitness());});
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



float Member::getFitness() {return _fitness;}
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
void ArrayMember<T>::print(bool ret) {
  for (int i=0; i<_array.size(); i++)
    cout << _array[i] << ' ';
  cout << endl;
}
template <class T>
ArrayMember<T>::ArrayMember(const ArrayMember &mem) {
  _array = mem->getArray();
}
template <class T>
const vector<T>& ArrayMember<T>::getArray() const{return _array;}
/*
template <class T>
unique_ptr<Member> copy() {
  return make_unique<ArrayMember<T>>(_array);
}*/
template <class T>
void ArrayMember<T>::setFitness(int fitness) { _fitness=fitness; }
template <class T>
float ArrayMember<T>::getFitness() { return _fitness; }
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
