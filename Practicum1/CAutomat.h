#pragma once
#include <vector>
#include <string>
#include <stack>
#include <assert.h>
#include <iostream>
#include <set>
#include <deque>
#include <utility>

//NFA with epsilon-edges
class CAutomat {

public:
    struct edge {
        edge(const int &_number, const int &_letter) : number(_number), letter(_letter){}
        edge() : letter(EPSILON), number(-1) {}
        edge(const edge & E) : number(E.number), letter(E.letter) {}
        int number;
        char letter;
        bool operator < (const edge &E) const {return (number < E.number) || (number == E.number && letter < E.letter);}
    };

    static const int INFINITY = 100000000;
    static const char EPSILON = '@'; //imagine that '@' is 'epsilon'

    bool isStart(const int &node) const;
    bool isFinal(const int &node) const;
    void setStart(const int &node);
    void addFinal(const int &node);
    void addEdge(const int &from, const char &letter, const int &to);
    std::vector<std::set<edge> > getEdges() const;
    int getStartNode() const;
    std::set<int> getFinalNodes() const;
    int getNodesNumber() const;
    CAutomat();
    CAutomat(const char &letter);
    CAutomat(const CAutomat &aut);
    void copy(const CAutomat &aut);

    //makes NFA from valid regular expression
    CAutomat(const std::string &regExp);


    //"glue" two automats, when multiply (".")
    //we add edges from final nodes of (*this) to start node of $aut and define final nodes
    // as final nodes of $aut and start node as start node of (*this)
    CAutomat concat( const CAutomat &aut,const char &letter = EPSILON);

    //loop automat, when "*"
    //we add new final node
    //we add epsilon-edges from previous final nodes and start node to new final node and define the set of
    // final nodes as the only new final node
    CAutomat loop();

    //add automat, when addition ("+")
    //we make a new start node and add epsilon-edges from it to old start nodes of the both automats
    CAutomat add(const CAutomat & aut);

    // add new start and final node, connected by epsilon-edges with the automat
    CAutomat extendByEpsilons();

    //returns minumum length of word, such as count of definite $symbol equals to $count
    //uses breadth-first search in 0-1 graph, 0 - epsilon-edge, 1 - the others
    //nodes - pair(node is NFA, count of $symbol in word-path to it)
    int calculateMinWordLength(const char &symbol, const int &count) const;
    bool isValid() const;

    void print() const;


private:

    int nodesNumber;
    void addNode();

    std::vector<std::set<edge> > edges = std::vector<std::set<edge> >();
    int startNode = -1;
    std::set<int> finalNodes;
};


bool CAutomat::isStart(const int &node) const { return node == startNode;}

bool CAutomat::isFinal(const int &node) const { return finalNodes.find(node) != finalNodes.end();}

void CAutomat::setStart(const int &node) {
    assert(node < nodesNumber);
    startNode = node;
}

void CAutomat::addFinal(const int &node) {
    assert(node < nodesNumber);
    finalNodes.insert(node);
}

void CAutomat::addEdge(const int &from, const char &letter, const int &to) {
    assert(from < nodesNumber);
    assert(to < nodesNumber);
    edges[from].insert(edge(to, letter));
}

std::vector<std::set<CAutomat::edge> > CAutomat::getEdges() const {return edges;}

int CAutomat::getStartNode() const {return startNode;}

std::set<int> CAutomat::getFinalNodes() const {return finalNodes;}

int CAutomat::getNodesNumber() const {return nodesNumber;}

CAutomat::CAutomat() {
    nodesNumber = 1;
    startNode = 0;
    finalNodes = std::set<int>();
    finalNodes.insert(0);
    edges = std::vector<std::set<edge> > (nodesNumber);
}

CAutomat::CAutomat(const char &letter):CAutomat() {
    CAutomat B = CAutomat();
    concat(B, letter);
}

CAutomat::CAutomat(const CAutomat &aut) {
    copy(aut);
}

void CAutomat::copy(const CAutomat &aut) {
    nodesNumber = aut.getNodesNumber();
    startNode = aut.getStartNode();
    edges = aut.getEdges();
    finalNodes = aut.getFinalNodes();
}

CAutomat::CAutomat(const std::string &regExp) {
    std::stack<CAutomat> blocks;
    for(char lexem : regExp) {
            if(lexem == ' ')
                continue;
            if(lexem == '+') {
                    assert(blocks.size() > 1);
                    CAutomat top = blocks.top();
                    blocks.pop();
                    top.add(blocks.top());
                    blocks.pop();
                    blocks.push(top);
                } else if(lexem == '.') {
                    assert(blocks.size() > 1);
                    CAutomat top1 = blocks.top();
                    blocks.pop();
                    CAutomat top2 = blocks.top();
                    blocks.pop();
                    blocks.push(top2.concat(top1));
                } else if(lexem == '*') {
                    assert(blocks.size());
                    CAutomat top = blocks.top();
                    blocks.pop();
                    blocks.push(top.loop());
                } else if(lexem == '1') {
                    blocks.push(CAutomat(EPSILON));
                } else {
                    blocks.push(CAutomat(lexem));
                }
        }
    assert(blocks.size() == 1);
    copy(blocks.top());
}

CAutomat CAutomat::extendByEpsilons() {
    nodesNumber += 2;
    std::set<edge> tmp = std::set<edge>();
    edges.insert(edges.end(), tmp);
    edges.insert(edges.end(), tmp);
    addEdge(nodesNumber - 2, EPSILON, startNode);
    for(int finalNode : finalNodes) {
        addEdge(finalNode, EPSILON, nodesNumber - 1);
    }
    finalNodes.clear();
    finalNodes.insert(nodesNumber - 1);
    setStart(nodesNumber - 2);
    return *this;
}

CAutomat CAutomat::concat(const CAutomat &aut,const char &letter) {
    auto autEdges = aut.getEdges();
    int prevNodesNumber = nodesNumber;
    nodesNumber += aut.getNodesNumber();
    for(size_t i = 0; i < autEdges.size(); ++i) {
            std::set<edge> tmp = std::set<edge>();
            for(auto _curEdge : autEdges[i]) {
                    edge curEdge = _curEdge;
                    curEdge.number += prevNodesNumber;
                    tmp.insert(curEdge);
                }
            autEdges[i] = std::set<edge>(tmp);
        }
    edges.insert(edges.end(), autEdges.begin(), autEdges.end());
    for(int prevFinalNode : finalNodes)
        addEdge(prevFinalNode, letter, aut.getStartNode() + prevNodesNumber);
    finalNodes.clear();
    for(auto finalNode : aut.getFinalNodes())
        finalNodes.insert(finalNode + prevNodesNumber);
    extendByEpsilons();
    return *this;
}

CAutomat CAutomat::loop() {

    //addEdge(startNode, EPSILON, nodesNumber - 1);
    for(int finalNode : finalNodes) {
        //addEdge(finalNode, EPSILON, nodesNumber - 1);
        addEdge(finalNode, EPSILON, startNode);
    }
    finalNodes.clear();
    finalNodes.insert(startNode);
    extendByEpsilons();
    return *this;
}


CAutomat CAutomat::add(const CAutomat &aut) {
    auto autEdges = aut.getEdges();
    for(size_t i = 0; i < autEdges.size(); ++i) {
            std::set<edge> tmp = std::set<edge>();
            for(auto _curEdge : autEdges[i]) {
                    edge curEdge = _curEdge;
                    curEdge.number += nodesNumber;
                    tmp.insert(curEdge);
                }
            autEdges[i]= std::set<edge>(tmp);
        }
    edges.insert(edges.end(), autEdges.begin(), autEdges.end());
    for(auto finalNode : aut.getFinalNodes())
        finalNodes.insert(finalNode + nodesNumber);
    int startNode2 = aut.getStartNode() + nodesNumber;
    nodesNumber += aut.getNodesNumber();
    addNode();
    addEdge(nodesNumber - 1, EPSILON, startNode);
    addEdge(nodesNumber - 1, EPSILON, startNode2);
    setStart(nodesNumber - 1);
    return *this;
    extendByEpsilons();
}

int CAutomat::calculateMinWordLength(const char &neededSymbol, const int &neededCount) const {
    std::vector<std::vector<int> > dist(nodesNumber, std::vector<int>(neededCount + 2, INFINITY));
    std::vector<std::vector<bool> > used(nodesNumber, std::vector<bool>(neededCount + 2, false));
    std::deque<std::pair<int, int> > Q;
    Q.push_back(std::make_pair(startNode, 0));
    dist[startNode][0] = 0;
    while(!Q.empty()) {
            int node = Q.front().first;
            int curK = Q.front().second;
            Q.pop_front();
            if(used[node][curK])
                continue;
            if(curK > neededCount)
                continue;
            used[node][curK] = true;

            int curDist = dist[node][curK];
            for(auto edge : edges[node]) {
                    if(edge.letter == neededSymbol) {
                            if (curDist + 1 < dist[edge.number][curK + 1]) {
                                    dist[edge.number][curK + 1] = curDist + 1;
                                    Q.push_back(std::make_pair(edge.number, curK + 1));
                                }
                        } else if(edge.letter == EPSILON) {
                            if (curDist < dist[edge.number][curK]) {
                                    dist[edge.number][curK] = curDist;
                                    Q.push_front(std::make_pair(edge.number, curK));
                                }
                        }
                    else {
                            if(curDist + 1 < dist[edge.number][curK]) {
                                    dist[edge.number][curK] = curDist + 1;
                                    Q.push_back(std::make_pair(edge.number, curK));
                                }

                        }
                }

        }
    int minDist = INFINITY;
    for(int final : finalNodes) {
            minDist = std::min(dist[final][neededCount], minDist);
        }
    return minDist;
}

bool CAutomat::isValid() const {
    return (startNode >= 0 && finalNodes.size() > 0);
}

void CAutomat::print() const {
    std::cout << "===================================="<<std::endl;
    std::cout << "               AUTOMAT              "<<std::endl;
    std::cout << "===================================="<<std::endl;
    std::cout << "nodes number: " << nodesNumber <<std::endl;
    for(int node = 0; node < nodesNumber; ++node) {
            std::cout << "Node " << node;
            if(isStart(node)) std::cout << " (start)";
            if(isFinal(node)) std::cout << " (final)";
            std::cout << ":" << std::endl;
            for(auto edge : edges[node]) {
                    std::cout << "    ";
                    if(edge.letter == EPSILON)
                        std::cout << "EPSILON";
                    else
                        std::cout << edge.letter;
                    std::cout <<" : " << edge.number <<std::endl;
                }
        }
    std::cout << "===================================="<<std::endl;
}

void CAutomat::addNode() {
    nodesNumber ++;
    edges.emplace_back();
}

const char CAutomat::EPSILON;
const int CAutomat::INFINITY;
