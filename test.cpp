#include <iostream>
#include <cassert>

#include "boxKdTree.hpp"

class TestClass{
public:
    std::string idStr() {return idStr_;}

    int bngBox[4];

    void idStrIs(std::string newVal){
        idStr_ = newVal;
    }

private:
    std::string idStr_;
};

// tests kd tree on simple example
int main(){
    // variable will hold objects for kd tree
    std::vector<TestClass> elemArr;
    TestClass elem;
    elem.idStrIs("this");
    elem.bngBox[0] = 0;
    elem.bngBox[1] = 10;
    elem.bngBox[2] = 0;
    elem.bngBox[3] = 10;
    elemArr.push_back(elem);
    elem.idStrIs("that");
    elem.bngBox[0] = 10;
    elem.bngBox[1] = 20;
    elem.bngBox[2] = 10;
    elem.bngBox[3] = 20;
    elemArr.push_back(elem);
    KdTree<TestClass> kdTree(elemArr);
    QueryBox queryBox(0, 5, 0, 5);
    auto valArr = kdTree.find(queryBox);
    assert(valArr.size() == 1);
    std::cout << "Tests passed." << std::endl;
}
