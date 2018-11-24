#include <GP.h>
#include <random>
#include <iostream>
using namespace std;

static default_random_engine gen;
static map<char,function<void(
    deque<unique_ptr<AstNode>> &nodes)>> buildChar = {
  {'/', BinaryAstNode::build<DivisionNode> },
  {'*', BinaryAstNode::build<MultNode> },
  {'+', BinaryAstNode::build<AddNode> }, 
  {'-', BinaryAstNode::build<SubNode> }
};

float AstNode::compute(const map<char,float> &dic) {}
int AstNode::size() { return _size; }
string AstNode::polacRandom() {return this->getPolac();}
unique_ptr<AstNode>
AstNode::replaceRandom(const string &polacV,
    unique_ptr<AstNode> thisOne) {
  return move(replace(polacV)); }
unique_ptr<AstNode>
AstNode::replace(const string& equation) {
  int count = 0;
  deque<unique_ptr<AstNode>> nodes;
  while (count<equation.size()) {
    if (isdigit(equation[count])) {
      auto node = make_unique<NumberNode>(
          (float)(equation[count]-48));
      nodes.push_back(move(node));}
    else if (isalpha(equation[count])) {
      auto node = make_unique<VarNode>(equation[count]);
      nodes.push_back(move(node));}
    else buildChar[equation[count]](nodes);
    count++;}
  return move(nodes.back());
}
string AstNode::getPolac() {
  vector<char> polacV;
  this->polac(polacV);
  string polacS(polacV.begin(), polacV.end());
  return polacS;
}
NumberNode::NumberNode(float value): _value(value) {_size=1;}
float NumberNode::compute(const map<char,float> &dic) {
  return _value; }
void NumberNode::print() { cout << _value; }
void NumberNode::polac(vector<char> &polacV) {
  polacV.push_back((char)(_value+48));}
VarNode::VarNode(char name): _name(name) {_size=1;}
float VarNode::compute(const map<char,float> &dic) {
  return dic.find(_name)->second; }
void VarNode::print() { cout << _name; }
void VarNode::polac(vector<char> &polacV) {
  polacV.push_back(_name);}
BinaryAstNode::BinaryAstNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right, char s):
  _left(move(left)), _right(move(right)), _symbol(s) {
    _size = _left->size()+1+_right->size(); }
unique_ptr<AstNode>
BinaryAstNode::replaceRandom(const string &polacV,
    unique_ptr<AstNode> thisOne) {
  int leftSize=_left->size(), rightSize=_right->size();
  uniform_int_distribution<int> udist(0,_size-1);
  int randNode = udist(gen);
  if (randNode < leftSize) {
    _left = move(_left->replaceRandom(polacV,move(_left)));
    _size += (_left->size()-leftSize);
    return thisOne;
  } else if (randNode > leftSize) {
    _right = move(_right->replaceRandom(polacV,move(_right)));
    _size += (_right->size()-rightSize);
    return thisOne;
  } else return move(replace(polacV)); }
string BinaryAstNode::polacRandom() {
  int leftSize=_left->size(), rightSize=_right->size();
  uniform_int_distribution<int> udist(0,_size-1);
  int randNode = udist(gen);
  if (randNode < leftSize)
    return _left->polacRandom();
  else if (randNode > leftSize)
    return _right->polacRandom();
  else return this->getPolac(); }
template <class T>
void BinaryAstNode::build(deque<unique_ptr<AstNode>> &nodes) {
  auto node2 = move(nodes.back()); nodes.pop_back();
  auto node1 = move(nodes.back()); nodes.pop_back();
  auto node = make_unique<T>(
      move(node1),move(node2));
  nodes.push_back(move(node)); }
void BinaryAstNode::print() {
  cout<<'('; _left->print(); cout<<_symbol; 
  _right->print(); cout<<')'; }
void BinaryAstNode::polac(vector<char> &polacV) {
  _left->polac(polacV); _right->polac(polacV);
  polacV.push_back(_symbol);}
float BinaryAstNode::operate(function<float(float,float)> oper,
    const map<char,float> &dic) {
    oper(_left->compute(dic),_right->compute(dic));}
DivisionNode::DivisionNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right):
  BinaryAstNode(move(left),move(right),'/') {}
float DivisionNode::compute(const map<char,float> &dic) {
  return operate(
      [](float a,float b){return b?a/b:1;},dic);}
MultNode::MultNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right):
  BinaryAstNode(move(left),move(right),'*') {}
float MultNode::compute(const map<char,float> &dic) {
  return operate(multiplies<float>(),dic);}
AddNode::AddNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right):
  BinaryAstNode(move(left),move(right),'+') {}
float AddNode::compute(const map<char,float> &dic) {
  return operate(plus<float>(),dic);}
SubNode::SubNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right):
  BinaryAstNode(move(left),move(right),'-') {}
float SubNode::compute(const map<char,float> &dic) {
  return operate(minus<float>(),dic);}

AST::AST(int size, float ini, float end): _size(size) {
  
  uniform_real_distribution<float> range(ini,end);
  uniform_int_distribution<int> coin(0,1);
  if (size==1) {
    if (coin(gen)) _head=make_unique<VarNode>('x');
    else _head=make_unique<NumberNode>(range(gen));
  } else {
  
  }
}
void AST::mutate() {}

AST::AST(const string& equation):
  _head(move(AstNode::replace(equation))),
  _size(_head->size()) {}
string AST::polacRandom() {return _head->polacRandom();}
void AST::replaceRandom(const string &polac) {
  _head = move(_head->replaceRandom(polac,move(_head))); 
  _size = _head->size(); }
int AST::size() {return _size;}
unique_ptr<AST> AST::copy() {
  auto ast = make_unique<AST>(polac());
  return move(ast);
}
void AST::print() { _head->print(); cout << endl; }
string AST::polac() {
  vector<char> polacV;
  _head->polac(polacV);
  string polacS(polacV.begin(), polacV.end());
  return polacS;
}
float AST::compute(const map<char,float> &dic) {
  return _head->compute(dic);
}
