#include <GP.h>
#include <iostream>
using namespace std;


int main() {

  {
    string eq = "212/+";
    cout << endl;
    auto ast = make_unique<AST>(eq);
    cout << "Tree: "; ast->print(); cout << endl;
    auto copy = make_unique<AST>(ast->polac());
    cout << "Copy: "; copy->print(); cout << endl;
    cout << "Result: " << ast->compute({}) << "\n";
    cout << "Tree size: " << ast->size() << endl;
  }
  {
    string eq = "xx*xx**";
    cout << endl;
    auto ast = make_unique<AST>(eq);
    cout << "Tree: "; ast->print(); cout << endl;
    auto copy = make_unique<AST>(ast->polac());
    cout << "Copy: "; copy->print(); cout << endl;
    cout << "Result: " << ast->compute({{'x',0}}) << "\n";
    cout << "Tree size: " << ast->size() << endl;
  }
  {
    cout << endl;
    string eq = "12/12/+12/12/+-";
    auto ast = make_unique<AST>(eq);
    cout << "Tree: "; ast->print(); cout << endl;
    map<int,int> sizes;
    for (int i=0; i<4096; i++) {
      auto copy = make_unique<AST>(ast->polac());
      copy->replaceRandom("9");
      if (sizes.find(copy->size())==sizes.end())
        sizes[copy->size()]=1;
      else
        sizes[copy->size()]++;
    }
    cout << "Original Size: " << eq.size() << endl;
    for (auto ite=sizes.begin(); ite!=sizes.end(); ++ite)
      cout<<"Chgd Size: "<< ite->first<<": "<<ite->second<<endl;
  }
  {
    cout << endl;
    cout << "Random depth 3 Tree" << endl;
    vector<int> vals = {1,2,3,4,5};
    auto ast = make_unique<AST>(3,vals,0.2);
    cout << "Tree: "; ast->print(); cout << endl;
    ast->mutate(3,vals,0.2);
    ast->mutate(3,vals,0.2);
    cout << "Mutated: "; ast->print(); cout << "\n\n";
  }
  {

    cout << "GP: Find Number\n";
    int maxEpochs = 10;
    int members=512, ktil=2, depth=5; 
    float ini=0,step=0.1,end=0;
    int low=0,up=9,nVals=12;
    double mutRate = 0.1;
    float varProb=0.f, cross=0.2f, lowerBound=0.0001f;
    function<float(float)> func = [](float x){ 
      return 147; };
    bool verbose = false;


    auto ga = make_unique<AstGA>(members,func,
        ini,step,end,  low,up,nVals,varProb, cross,
        lowerBound, depth);
    cout << "GP example ";
    cout << "[P:"<<members<<", MR: "<<mutRate<<"]\n";

    //cout << "Perfect func: x^2\n";
    //ga->print();

    int ites = ga->evolve(mutRate,ktil,maxEpochs,true).size();
    cout << "Needed ites: " << ites << endl << endl;
    
  }
  {
    cout << "GP: Find Function\n";
    int maxEpochs = 100;
    int members=512, ktil=2, depth=5; 
    float ini=-1.f,step=0.1,end=1.f;
    int low=2,up=4,nVals=5;
    double mutRate = 0.1;
    float varProb=0.2f, cross=0.2f, lowerBound=0.0001f;
    function<float(float)> func = [](float x){ 
      return x*x*x*x+x*x+x+3; };

    bool verbose = false;
    auto ga = make_unique<AstGA>(members,func,
        ini,step,end,  low,up,nVals,varProb, cross,
        lowerBound, depth);
    cout << "GP example ";
    cout << "[P:"<<members<<", MR: "<<mutRate<<"]\n";

    //cout << "Perfect func: x^2\n";
    //ga->print();

    int ites = ga->evolve(mutRate,maxEpochs,true,ktil).size();
    cout << "Needed ites: " << ites << endl << endl;

  }
  
  cout << endl;
}
