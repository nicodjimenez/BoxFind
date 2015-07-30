#ifndef BOX_KD_TREE_HPP
#define BOX_KD_TREE_HPP

#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>

int median(std::vector<int> &v)
{
    size_t n = v.size() / 2;
    nth_element(v.begin(), v.begin()+n, v.end());
    return v[n];
}

/* Use query box as input type for 
 * kdTree's find method */
class QueryBox {
public:
    QueryBox(int imin, int imax, int jmin, int jmax){
        arr_[0] = imin;
        arr_[1] = imax;
        arr_[2] = jmin;
        arr_[3] = jmax;
    }

    int& operator[](int index){
        return arr_[index];
    }

private:
    int arr_[4];
};

class Node {
public:
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    // empty string signifies not leaf
    std::string idStr = "";
    int splitIndex;
    int splitValue;
};


/*  T is the type of the object that will be stored in bounding box container.
 *  The following operations must be defined for any instance elem of type T:
 *      elem.bngBox[0] - returns int that is min i coordinate of box
 *      elem.bngBox[1] - returns int that is max i coordinate of box
 *      elem.bngBox[2] - returns int that is min j coordinate of box
 *      elem.bngBox[3] - returns int that is max j coordinate of box
 *      elem.idStr() - returns a unique std::string identifier for this bounding box
 */
template<typename T>
class KdTree {
public:
    std::vector<T> find(QueryBox& box);

    KdTree(std::vector<T>& elemArr);

private:
    void buildTree();

    void buildTreeRecursive(std::shared_ptr<Node> top, std::vector<T> elemArr);

    void findRecursive(QueryBox& queryBox, const std::shared_ptr<Node>& rootNode, std::vector<T>& curArr);

    std::shared_ptr<Node> topNode_;

    typedef std::unordered_map<std::string, T> ElemMapType;
    ElemMapType elemMap_;
    
};

template<typename T> KdTree<T>::KdTree(std::vector<T>& elemArr) 
{
    // add all elements to map
    for (auto& elem : elemArr){
        elemMap_[elem.idStr()] = elem;
    }
    buildTree();
}

template<typename T> 
void KdTree<T>::buildTree(){
    topNode_ = std::make_shared<Node>();
    topNode_->splitIndex = 0;
    // create vector with all the entries
    std::vector<T> elemArr(elemMap_.size());
    int i = 0;
    for (const auto& curElem : elemMap_){
        elemArr[i] = curElem.second;
        i++;
    }
    buildTreeRecursive(topNode_, elemArr);
}

template<typename T> 
void KdTree<T>::buildTreeRecursive(std::shared_ptr<Node> rootNode, std::vector<T> elemArr){
    int arrSize = elemArr.size();
    if (arrSize == 0) return;
    if (arrSize == 1) {
        rootNode->idStr = elemArr[0].idStr();
        return;
    }
    if (!rootNode) return;

    int curBoxInd = rootNode->splitIndex;
    // insert coordinates into vec to facilitate median calculation
    std::vector<int> posVec(arrSize);
    for (int i = 0; i < arrSize; ++i){
        posVec[i] = elemArr[i].bngBox[curBoxInd]; 
    }
    int medianValue = median(posVec);

    rootNode->splitValue = medianValue;
    // less than median value
    std::vector<T> leftSideArr;
    // greater or equal to median value
    std::vector<T> rightSideArr;
    // this loop takes care of possibility of duplicate values
    for (const auto& elem : elemArr){
        if (elem.bngBox[curBoxInd] < medianValue){
            leftSideArr.push_back(elem); 
        } else {
            rightSideArr.push_back(elem);
        }
    }
    auto nodeLeft = std::make_shared<Node>();
    auto nodeRight = std::make_shared<Node>();
    int nextBoxInd = (curBoxInd + 1) % 4;
    nodeLeft->splitIndex = nextBoxInd;
    nodeRight->splitIndex = nextBoxInd;
    rootNode->left = nodeLeft;
    rootNode->right = nodeRight;
    buildTreeRecursive(nodeLeft, leftSideArr);
    buildTreeRecursive(nodeRight, rightSideArr);
}

template<typename T> 
std::vector<T> KdTree<T>::find(QueryBox& queryBox){
    std::vector<T> retElemArr;
    findRecursive(queryBox, topNode_, retElemArr);
    return retElemArr;
}

template<typename T> 
void KdTree<T>::findRecursive(QueryBox& queryBox, const std::shared_ptr<Node>& rootNode, std::vector<T>& curArr){
    // if node is null pointer
    if (!rootNode) return;

    std::string nodeIdStr = rootNode->idStr;
    // means node is leaf
    if (nodeIdStr.size() != 0){
        // TODO: check that box intersects
        curArr.push_back(elemMap_[nodeIdStr]);
        return;
    }

    int curBoxInd = rootNode->splitIndex;
    int splitVal = rootNode->splitValue;
    switch (curBoxInd){
        case 0:
            findRecursive(queryBox, rootNode->left, curArr); 
            // if imax of search is smaller than imin split, don't look right
            if (queryBox[1] >= splitVal){
               findRecursive(queryBox, rootNode->right, curArr); 
            }
            break;
        case 1:
            findRecursive(queryBox, rootNode->right, curArr); 
            // if imin of search is bigger than imax split, don't look left
            if (queryBox[0] <= splitVal){
               findRecursive(queryBox, rootNode->left, curArr); 
            }
            break;
        case 2:
            findRecursive(queryBox, rootNode->left, curArr); 
            // if jmax of search is smaller than jmin split, don't look right
            if (queryBox[3] >= splitVal){
               findRecursive(queryBox, rootNode->right, curArr); 
            }
            break;
        case 3:
            findRecursive(queryBox, rootNode->right, curArr); 
            // if jmin of search is bigger than jmax split, don't look left
            if (queryBox[2] <= splitVal){
               findRecursive(queryBox, rootNode->left, curArr); 
            }
            break;
    }
}
#endif


