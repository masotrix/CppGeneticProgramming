#ifndef GP_H
#define GP_H
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <deque>
#include <limits>

class Member {
  protected:
    int _fitness;
  public:
    Member() {}
    Member(const Member&) = delete;
    Member& operator=(const Member&) = delete;
    ~Member() = default;
    virtual void print()=0;
    void setFitness(int fitness);
    int getFitness();
};
template <class T>
class ArrayMember: public Member {
  protected:
    std::vector<T> _array;
  public:
    explicit ArrayMember(int B, int ops);
    explicit ArrayMember(int B);
    explicit ArrayMember(std::vector<T>);
    ArrayMember(const ArrayMember&) = delete;
    ArrayMember& operator=(const ArrayMember&) = delete;
    ~ArrayMember() = default;
    virtual void print();
    const std::vector<T> & getArray() const;
    void setFitness(int fitness);
    int getFitness();
};
class BitArrayMember: public ArrayMember<bool> {
  public:
    explicit BitArrayMember(int B);
    explicit BitArrayMember(std::vector<bool>);
    BitArrayMember(const BitArrayMember&) = delete;
    BitArrayMember& operator=(const BitArrayMember&) = delete;
    ~BitArrayMember() = default;
};
class CharArrayMember: public ArrayMember<char> {
  public:
    explicit CharArrayMember(int B);
    explicit CharArrayMember(std::vector<char>);
    CharArrayMember(const CharArrayMember&) = delete;
    CharArrayMember& operator=(const CharArrayMember&) = delete;
    ~CharArrayMember() = default;
};
class PathMember: public ArrayMember<int> {
  public:
    explicit PathMember(int B);
    explicit PathMember(std::vector<int> path);
    PathMember(const PathMember&) = delete;
    PathMember& operator=(const PathMember&) = delete;
    ~PathMember() = default;
};
class TravelMember: public ArrayMember<int> {
  public:
    explicit TravelMember(int B);
    explicit TravelMember(std::vector<int> path);
    TravelMember(const TravelMember&) = delete;
    TravelMember& operator=(const TravelMember&) = delete;
    ~TravelMember() = default;
};

template <class T>
class GeneticAlgorithm {
  protected:
    std::vector<std::unique_ptr<T>> _members;
    float _minFitness;
    int _epochs; double _std,_avg;
  public:
    virtual int select(int k,
        std::function<bool(float,float)> comp =
          std::greater<float>());
    virtual float evaluate()=0;
    virtual void print(int i);
    virtual void reproduce(double mutRate, int rounds)=0;
    virtual std::vector<double> 
      evolve(double mutRate, int tournametSize,
        int epochs=std::numeric_limits<int>::max(),
        bool verbose=false);
    virtual float getMinFitness();
};
template <class T, class K> 
class ArrayGA: public GeneticAlgorithm<K> {
  protected:
    std::vector<T> _perfectArray;
    int _options;
  public:
    ArrayGA();
    ArrayGA(int N, int B, int ops);
    ArrayGA(int N, int B);
    virtual float evaluate() override;
    virtual void reproduce(double mutRate,int rounds) override;
};
class FindBitArrayGA: public ArrayGA<bool,BitArrayMember> {
  public: FindBitArrayGA(int N,int B);
};
class FindCharArrayGA: public ArrayGA<char,CharArrayMember> {
  public: FindCharArrayGA(int N,int B);
};
class FindPathGA: public ArrayGA<int,PathMember> {
  protected:
    std::unique_ptr<std::vector<std::vector<char>>> _labyrinth;
    int _rows,_cols,_inix,_iniy,_endx,_endy,_punish;
  public: 
    FindPathGA(int N, int B);
    virtual void print(int i);
    virtual float evaluate() override;
    virtual int select(int k,
        std::function<bool(float,float)> comp =
          std::greater<float>());
    virtual void reproduce(double mutRate,int rounds) override;
};
class FindTravelerGA: public ArrayGA<int,TravelMember> {
  std::unique_ptr<std::vector<std::vector<float>>> _points;
  public:
    FindTravelerGA(int N, int B, std::string filename);
    virtual float evaluate() override;
    virtual int select(int k,
        std::function<bool(float,float)> comp =
          std::greater<float>());
    virtual void reproduce(double mutRate,int rounds) override;
    virtual std::vector<std::vector<float>> getOptPoints();
};


class AstNode {
  protected: int _size;
  public: 
    static std::unique_ptr<AstNode> 
      replace(const std::string &equation);
    std::string polacRandom();
    virtual std::unique_ptr<AstNode> 
      replaceRandom(const std::string &equation, 
          std::unique_ptr<AstNode>);
    virtual float compute(const std::map<char,float>&);
    virtual void print()=0;
    virtual std::string getPolac();
    virtual void polac(std::vector<char>&)=0;
    virtual int size();
};
class NumberNode: public AstNode {
  private: float _value;
  public:
    NumberNode(float value);
    virtual void print() override;
    virtual void polac(std::vector<char>&) override;
    virtual float compute(const std::map<char,float>&) override;
};
class VarNode: public AstNode {
  private: char _name;
  public:
    VarNode(char name);
    virtual void print() override;
    virtual void polac(std::vector<char>&) override;
    virtual float compute(const std::map<char,float>&) override;
};
class BinaryAstNode: public AstNode {
  protected:
    std::unique_ptr<AstNode> _left, _right;
    char _symbol;
  public:
    BinaryAstNode(
        std::unique_ptr<AstNode> left, 
        std::unique_ptr<AstNode> right,
        char symbol);
    virtual std::unique_ptr<AstNode>
      replaceRandom(const std::string &equation,
          std::unique_ptr<AstNode>) override;
    virtual void print() override;
    std::string polacRandom();
    virtual void polac(std::vector<char>&) override;
    template <class T>
    static void build(std::deque<std::unique_ptr<AstNode>>&);
    float operate(std::function<float(float,float)> oper,
        const std::map<char,float>&);
};
class DivisionNode: public BinaryAstNode {
  public:
    DivisionNode(
        std::unique_ptr<AstNode> left, 
        std::unique_ptr<AstNode> right);
    virtual float compute(const std::map<char,float>&) override;
};
class MultNode: public BinaryAstNode {
  public:
    MultNode(
        std::unique_ptr<AstNode> left, 
        std::unique_ptr<AstNode> right);
    virtual float compute(const std::map<char,float>&) override;
};
class AddNode: public BinaryAstNode {
  public:
    AddNode(
        std::unique_ptr<AstNode> left, 
        std::unique_ptr<AstNode> right);
    virtual float compute(const std::map<char,float>&) override;
};
class SubNode: public BinaryAstNode {
  public:
    SubNode(
        std::unique_ptr<AstNode> left, 
        std::unique_ptr<AstNode> right);
    virtual float compute(const std::map<char,float>&) override;
};


class AST: public Member {
  private:
    std::unique_ptr<AstNode> _head; int _size;
  public:
    explicit AST(int size, float a, float b);
    explicit AST(const std::string& polac_equation);
    std::string polacRandom();
    void replaceRandom(const std::string &polac);
    virtual int size();
    AST(const AST&) = delete;
    AST& operator=(const AST&) = delete;
    ~AST() = default;
    std::unique_ptr<AST> copy();
    virtual void print();
    virtual std::string polac();
    virtual void mutate();
    virtual float compute(const std::map<char,float>&);
};
class AstGA: public GeneticAlgorithm<AST> {
  protected:
    std::function<float(float)> _perfectFunc;
    float _ini, _step, _end, _lowerBound;
  public:
    AstGA(int N, std::function<float(float)>,
        float, float, float, float lB=0.0001);
    virtual float evaluate() override;
    virtual void reproduce(double mutRate,int rounds) override;
};
#endif
