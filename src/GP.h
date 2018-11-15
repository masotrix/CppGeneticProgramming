#ifndef GP_H
#define GP_H
#include <vector>
#include <functional>
#include <memory>

class Member {
  public:
    Member() {}
    Member(const Member&) = delete;
    Member& operator=(const Member&) = delete;
    ~Member() = default;
};
template <class T>
class ArrayMember: public Member {
  private:
    std::vector<T> _array;
    int _fitness;
  public:
    explicit ArrayMember(int B, int ops);
    explicit ArrayMember(std::vector<T>);
    ArrayMember(const ArrayMember&) = delete;
    ArrayMember& operator=(const ArrayMember&) = delete;
    ~ArrayMember() = default;
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


class GeneticAlgorithm {
  public:
    virtual int select(int k)=0;
    virtual int evaluate()=0;
    virtual void reproduce(double mutRate, int rounds)=0;
    virtual int evolve(double mutRate, int tournametSize);
};
template <class T, class K> 
class ArrayGA: public GeneticAlgorithm {
  protected:
    std::vector<std::unique_ptr<K>> _members;
    std::vector<T> _perfectArray;
    int _options;
  public:
    ArrayGA(int N, int B, int ops);
    virtual int select(int k) override;
    virtual int evaluate() override;
    virtual void reproduce(double mutRate,int rounds) override;
    const std::vector<T>& getMember(int i) const;
    const std::vector<T>& getPerfect() const;
    
};
class FindBitArrayGA: public ArrayGA<bool,BitArrayMember> {
  public: FindBitArrayGA(int N,int B);
};
class FindCharArrayGA: public ArrayGA<char,CharArrayMember> {
  public: FindCharArrayGA(int N,int B);
};

#endif
