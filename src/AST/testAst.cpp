#include <GP.h>
#include <iostream>
using namespace std;


int main() {

  {
    string eq = "212/+";
    cout << endl;
    auto ast = make_unique<AST>(eq);
    cout << "Tree: "; ast->print();
    auto copy = move(ast->copy());
    cout << "Copy: "; copy->print();
    cout << "Result: " << ast->compute({}) << "\n";
    cout << "Tree size: " << ast->size() << endl;
  }
  {
    string eq = "x12/-";
    cout << endl;
    auto ast = make_unique<AST>(eq);
    cout << "Tree: "; ast->print();
    auto copy = move(ast->copy());
    cout << "Copy: "; copy->print();
    cout << "Result: " << ast->compute({{'x',2}}) << "\n";
    cout << "Tree size: " << ast->size() << endl;
  }
  {
    string eq = "12/12/+12/12/+-";
    cout << endl;
    auto ast = make_unique<AST>(eq);
    cout << "Tree: "; ast->print();
    map<int,int> sizes;
    for (int i=0; i<4096; i++) {
      auto copy = move(ast->copy());
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
  
  cout << endl;
}
