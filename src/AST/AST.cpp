#include <GP.h>
#include <iostream>
using namespace std;

#define OPS 2
#define ADD 0
#define MULT 1
#define SUB 2


static default_random_engine gen;
static map<char,function<void(
    deque<unique_ptr<AstNode>> &nodes)>> buildChar = {
  {'/', BinaryAstNode::build<DivisionNode> },
  {'*', BinaryAstNode::build<MultNode> },
  {'+', BinaryAstNode::build<AddNode> }, 
  {'-', BinaryAstNode::build<SubNode> }
};

float AstNode::compute(const map<char,float> &dic) {}
void AstNode::setDepth(int depth) {
  _depth = depth;
}
int AstNode::size() { return _size; }
string AstNode::polacRandom(int &depth) {
  return this->getPolac(depth);}
unique_ptr<AstNode> AstNode::replaceRandom(const string &polacV,
    unique_ptr<AstNode> thisOne,int maxRandDepth,int max_depth) {
  if (_depth<=maxRandDepth)
    return move(replace(polacV,_depth)); 
  return thisOne; }
unique_ptr<AstNode> AstNode::replaceRandom(int max_depth,
    unique_ptr<AstNode> thisOne,
    vector<int> &vals, float varProb) {
  uniform_int_distribution<int> mut_ast(1,max_depth-_depth+1);
  int depth2 = mut_ast(gen);
  return move(replace(depth2,vals,varProb,_depth)); }
unique_ptr<AstNode> AstNode::replace(const string& equation,
    int depth) {
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
  nodes.back()->setDepth(depth);
  return move(nodes.back());
}
unique_ptr<AstNode> AstNode::replace(int depth,
    vector<int> &vals, float varProb, int depthFath) {
 
  unique_ptr<AstNode> node = nullptr; 
  uniform_int_distribution<int> rndOp(0,OPS);
  uniform_real_distribution<float> uni(0,1);
  if (depth==1) {
    if (uni(gen)<varProb) node=make_unique<VarNode>('x');
    else node=make_unique<NumberNode>(
        vals[(int)(uni(gen)*vals.size())]);
  } else {
    switch (rndOp(gen)) {
      case ADD: node=make_unique<AddNode>(depth, vals,varProb);
                break;
      case MULT: node=make_unique<MultNode>(depth, vals,varProb);
                 break;
      case SUB: node=make_unique<SubNode>(depth, vals,varProb);
                break;
    }
  }
  node->setDepth(depthFath);
  return move(node);
}
string AstNode::getPolac(int &depth) {
  vector<char> polacV;
  depth = 0;
  this->polac(polacV, depth);
  string polacS(polacV.begin(), polacV.end());
  return polacS;
}
NumberNode::NumberNode(float value): _value(value) {_size=1;}
float NumberNode::compute(const map<char,float> &dic) {
  return _value; }
void NumberNode::print() {/*printf("%.1f", _value);*/
  cout<<_value;  }
void NumberNode::polac(vector<char> &polacV, int &depth) {
  polacV.push_back((char)(_value+48)); depth++;}
VarNode::VarNode(char name): _name(name) {_size=1;}
float VarNode::compute(const map<char,float> &dic) {
  return dic.find(_name)->second; }
void VarNode::print() { cout << _name; }
void VarNode::polac(vector<char> &polacV, int &depth) {
  polacV.push_back(_name); depth++;}
BinaryAstNode::BinaryAstNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right, char s):
  _left(move(left)), _right(move(right)), _symbol(s) {
    _size = _left->size()+1+_right->size(); }
void BinaryAstNode::setDepth(int depth) {
  _depth=depth;
  _left->setDepth(depth+1);
  _right->setDepth(depth+1);
}
BinaryAstNode::BinaryAstNode(int depth,
    vector<int> &vals, char s,
    float varProb):
  _symbol(s) {
  
  uniform_real_distribution<float> uni(0,1);
  uniform_int_distribution<int> rndOp(0,OPS);
  if (depth==2) {
    if (uni(gen)<varProb) _left=make_unique<VarNode>('x');
    else {
      int selectedVal = vals[(int)(uni(gen)*vals.size())];
      _left=make_unique<NumberNode>(selectedVal);
    }
    if (uni(gen)<varProb) _right=make_unique<VarNode>('x');
    else {
      int selectedVal = vals[(int)(uni(gen)*vals.size())];
      _right=make_unique<NumberNode>(selectedVal);
    }
  } else {
    switch (rndOp(gen)) {
      case ADD:
        _left=make_unique<AddNode>(depth-1,vals,varProb); break;
      case MULT:
        _left=make_unique<MultNode>(depth-1,vals,varProb); break;
      case SUB:
        _left=make_unique<SubNode>(depth-1,vals,varProb); break;
    }
    switch (rndOp(gen)) {
      case ADD:
        _right=make_unique<AddNode>(depth-1,vals,varProb); break;
      case MULT:
        _right=make_unique<MultNode>(depth-1,vals,varProb); break;
      case SUB:
        _right=make_unique<SubNode>(depth-1,vals,varProb); break;
    }
  }
  _size = _left->size()+1+_right->size();
}
unique_ptr<AstNode>
BinaryAstNode::replaceRandom(const string &polacV,
    unique_ptr<AstNode> thisOne, int maxRandDepth,int max_depth) {
  int leftSize=_left->size(), rightSize=_right->size();
  uniform_int_distribution<int> udist(0,_size-1);
  int randNode = udist(gen);
  if (randNode < leftSize) {
    _left = move(_left->replaceRandom(
          polacV,move(_left),maxRandDepth,max_depth));
    _size += (_left->size()-leftSize);
    return thisOne;
  } else if (randNode > leftSize) {
    _right = move(_right->replaceRandom(
          polacV,move(_right),maxRandDepth,max_depth));
    _size += (_right->size()-rightSize);
    return thisOne;
  } else {
    if (_depth<=maxRandDepth)
      return move(replace(polacV,_depth));
    else return thisOne;
  }
}
unique_ptr<AstNode>
BinaryAstNode::replaceRandom(int max_depth,
    unique_ptr<AstNode> thisOne,
    vector<int> &vals, float varProb) {
  int leftSize=_left->size(), rightSize=_right->size();
  uniform_int_distribution<int> udist(0,_size-1);
  int randNode = udist(gen);
  if (randNode < leftSize) {
    _left = move(_left->replaceRandom(
          max_depth,move(_left),vals,varProb));
    _size += (_left->size()-leftSize);
    return thisOne;
  } else if (randNode > leftSize) {
    _right = move(_right->replaceRandom(
          max_depth,move(_right),vals,varProb));
    _size += (_right->size()-rightSize);
    return thisOne;
  } else {
    uniform_int_distribution<int> mut_ast(1,max_depth-_depth+1);
    return move(replace(mut_ast(gen),vals,varProb,_depth)); 
  }}
string BinaryAstNode::polacRandom(int &depth) {
  int leftSize=_left->size(), rightSize=_right->size();
  uniform_int_distribution<int> udist(0,_size-1);
  int randNode = udist(gen);
  if (randNode < leftSize)
    return _left->polacRandom(depth);
  else if (randNode > leftSize)
    return _right->polacRandom(depth);
  else { return this->getPolac(depth); }}
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
void BinaryAstNode::polac(vector<char> &polacV, int &depth) {
  int depthB1=1, depthB2=1;
  _left->polac(polacV,depthB1); _right->polac(polacV,depthB2);
  depth += max(depthB1,depthB2);
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
MultNode::MultNode(
    int depth, vector<int> &vals, float varProb):
  BinaryAstNode(depth, vals, '*', varProb) {}
float MultNode::compute(const map<char,float> &dic) {
  return operate(multiplies<float>(),dic);}
AddNode::AddNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right):
  BinaryAstNode(move(left),move(right),'+') {}
AddNode::AddNode(
    int depth, vector<int> &vals, float varProb):
  BinaryAstNode(depth, vals, '+', varProb) {}
float AddNode::compute(const map<char,float> &dic) {
  return operate(plus<float>(),dic);}
SubNode::SubNode(
    unique_ptr<AstNode> left, unique_ptr<AstNode> right):
  BinaryAstNode(move(left),move(right),'-') {}
SubNode::SubNode(
    int depth, vector<int> &vals, float varProb):
  BinaryAstNode(depth, vals, '-', varProb) {}
float SubNode::compute(const map<char,float> &dic) {
  return operate(minus<float>(),dic);}

AST::AST(int depth, vector<int> &vals, float varProb) {
  
  uniform_int_distribution<int> rndOp(0,OPS);
  uniform_real_distribution<float> uni(0,1);
  if (depth==1) {
    if (uni(gen)<varProb) _head=make_unique<VarNode>('x');
    else _head=make_unique<NumberNode>(
        vals[(int)(uni(gen)*vals.size())]);
  } else {
    switch (rndOp(gen)) {
      case ADD: _head=make_unique<AddNode>(depth,vals,varProb);
                break;
      case MULT: _head=make_unique<MultNode>(depth,vals,varProb);
                 break;
      case SUB: _head=make_unique<SubNode>(depth,vals,varProb);
                break;
    }
  }
  _size = _head->size();
  _head->setDepth(1);
}
AST::AST(const string& equation):
  _head(move(AstNode::replace(equation,1))),
  _size(_head->size()) {_head->setDepth(1);}
void AST::mutate(int max_depth, vector<int>&vals, float varProb) {
  replaceRandom(vals, varProb, max_depth);
}
string AST::polacRandom(int &depth) {
  return _head->polacRandom(depth);}
/**
 * Reemplaza nodo aleatorio de AST por nodo codificado
 * en ecuación polaca (polac) entregada. Debido a su
 * mayor cantidad, las hojas tienen una probabilidad
 * exponencialmente mayor de ser seleccionadas. Generalizando,
 * la probabilidad de seleccionar un nodo de una profundida
 * dada, aunmenta exponencialmente con tal profundidad.
 */
void AST::replaceRandom(const string &polac, int maxRandDepth,
    int max_depth) {
  _head = move(_head->replaceRandom(
        polac,move(_head),maxRandDepth,max_depth)); 
  _size = _head->size(); }
void AST::replaceRandom(
    vector<int>&vals, float varProb, int max_depth) {

  _head = move(_head->replaceRandom(max_depth,move(_head),vals,
        varProb)); 
  _size = _head->size(); }

int AST::size() {return _size;}
void AST::print(bool ret) {
  _head->print(); if (ret) cout << endl; }
string AST::polac() {
  vector<char> polacV; int depth=0;
  _head->polac(polacV, depth);
  string polacS(polacV.begin(), polacV.end());
  return polacS;
}
float AST::compute(const map<char,float> &dic) {
  return _head->compute(dic);
}
