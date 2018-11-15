#include <GP.h>
#include <memory>
#include <random>
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
int GeneticAlgorithm::evolve(double mutRate, int tourSize) {
  int ites;
  //printVec<bool>(ga->getPerfect()); cout << endl;
  for (int k=0;;k++) {
    /*for (int i=0; i<n; i++)
      printVec<bool>(ga->getMember(i));*/
    if (evaluate()!=-1) {
      ites = k; break; }
    reproduce(mutRate,tourSize);// cout << endl;
  }
  return ites;
}

template <class T, class K>
ArrayGA<T,K>::ArrayGA(int N, int B, int ops): 
  _perfectArray(vector<T>(B)), _options(ops)  {
  for (int i=0; i<N; i++) {
    auto mem = make_unique<K>(B);
    _members.push_back(move(mem));
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
int ArrayGA<T,K>::evaluate() {
  for (int i=0; i<_members.size(); i++) {
    int fitness = 
      matchArray<T>(_members[i]->getArray(),_perfectArray);
    if (fitness == _perfectArray.size())
      return i;
    _members[i]->setFitness(fitness);
  }
  return -1;
}
template <class T, class K>
int ArrayGA<T,K>::select(int k) {
  int selected, best=-1;
  uniform_int_distribution<int> member(0,_members.size()-1);
  for (int i=0; i<k; i++) {
    selected = member(gen);
    if (best==-1 ||
        _members[selected]->getFitness() >
        _members[best]->getFitness())
      best = selected;
  }
  return best;
}
template <class T, class K>
void ArrayGA<T,K>::reproduce(double mutRate, int tourRounds) {
  uniform_int_distribution<int> option(0,_options-1), coin(0,1);
  uniform_real_distribution<double> mutation(0,1);
  int selected1=select(tourRounds), selected2(select(tourRounds));
  vector<unique_ptr<K>> offspring;
  for (int k=0; k<_members.size(); k++) {
    vector<T> new_array(_perfectArray.size(),0);
    for (int i=0; i<new_array.size(); i++) {
      if (mutation(gen)>mutRate)
        new_array[i]=(coin(gen)?
            _members[selected1]->getArray()[i]:
            _members[selected2]->getArray()[i]);
      else
        new_array[i]=option(gen);
    }
    auto baby = make_unique<K>(new_array);
    offspring.push_back(move(baby));
  }
  _members = move(offspring);
}
template <class T, class K>
const vector<T>& ArrayGA<T,K>::getMember(int i) const{
  return _members[i]->getArray(); }
template <class T, class K>
const vector<T>& ArrayGA<T,K>::getPerfect() const {
  return _perfectArray; }
FindBitArrayGA::FindBitArrayGA(int N, int B): 
  ArrayGA<bool,BitArrayMember>(N,B,2) {
}
FindCharArrayGA::FindCharArrayGA(int N, int B): 
  ArrayGA<char,CharArrayMember>(N,B,256) {
}


template <class T>
ArrayMember<T>::ArrayMember(int B, int options):
  _array(vector<T>(B)) {
  uniform_int_distribution<int> udist(0,options-1);
  for (int i=0; i<_array.size(); i++) _array[i] = udist(gen); }
template <class T>
ArrayMember<T>::ArrayMember(vector<T> v): _array(v) {}
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

