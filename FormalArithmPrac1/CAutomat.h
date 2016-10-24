#pragma once
#include <vector>
#include <string>
#include <stack>
#include <assert.h>
#include <iostream>
#include <set>
class CAutomat {

public:
    struct edge {
        int number;
        char letter;
        edge(const int &_number, const int &_letter) : number(_number), letter(_letter){}
        edge() : letter(EPSILON), number(-1) {}
    };

    static const int INF = 100000000;
    static const char EPSILON = '@';

    bool isStart(const int &node) const;
    bool isFinal(const int &node) const;
    void setStart(const int &node);
    void addFinal(const int &node);
    void addEdge(const int &from, const char &letter, const int &to);
    std::vector<std::vector<edge> > getEdges() const;
    int getStartNode() const;
    std::set<int> getFinalNodes() const;
    int getNodesNumber() const;
    CAutomat();
    CAutomat(const char &letter);
    CAutomat(const CAutomat &aut);
    CAutomat(const std::string &regExp);
    void copy(const CAutomat &aut);
    //"glue" two automats, when multiply (".")
    CAutomat concat( const CAutomat &aut,const char &letter = EPSILON);
    //loop automat, when "*"
    CAutomat loop();
    //add automat, when addition ("+")
    CAutomat add(const CAutomat & aut);


    bool isValid() const {
        return (startNode >= 0 && finalNodes.size() > 0);
    }
    //returns length of minimum cycle and -1 if there are no cycles
    int findMinCycleWithSymbol(const char &symbol) const {
        assert(startNode >= 0);
        int minCycle = INF;
        std::vector<int> depth(nodesNumber);
        std::vector<int> color(nodesNumber);
        std::string sequence = "";
        _findMinCycleWithSymbol(symbol, startNode, 0, &minCycle, color, depth, sequence)                          ;
        if(minCycle < INF)
            return minCycle;
        else return -1;
    }
    void print() const {
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
    }


private:

    int nodesNumber;
    void addNode() {
        nodesNumber ++;
        edges.emplace_back(0);
    }
    //based on dfs
    void _findMinCycleWithSymbol(const char &symbol, const int &curNode, const int &curDepth, int *minCycle, std::vector<int> color, std::vector<int> depth, std::string &curWord) const {
        assert(curNode >= 0 && curNode < nodesNumber);
        color[curNode] = 1;
        depth[curNode] = curDepth;
        for(auto to : edges[curNode]) {
            if(color[to.number] == 2)
                continue;
            if(color[to.number] == 1) {
                *minCycle = std::min(*minCycle, curDepth + 1 - depth[to.number]);
                continue;
            }
            else {
                curWord.push_back(to.letter);
                _findMinCycleWithSymbol(symbol, to.number, curDepth + (to.letter == EPSILON ? 1 : 0 ), minCycle, color, depth, curWord);
                curWord.pop_back();
            }
        }
        color[curNode] = 2;
    }

    std::vector<std::vector<edge> > edges = std::vector<std::vector<edge> >();
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
    edges[from].push_back(edge(to, letter));
}

std::vector<std::vector<CAutomat::edge> > CAutomat::getEdges() const {return edges;}

int CAutomat::getStartNode() const {return startNode;}

std::set<int> CAutomat::getFinalNodes() const {return finalNodes;}

int CAutomat::getNodesNumber() const {return nodesNumber;}

CAutomat::CAutomat() {
    nodesNumber = 1;
    startNode = 0;
    finalNodes = std::set<int>();
    finalNodes.insert(0);
    edges = std::vector<std::vector<edge> > (nodesNumber);
}

CAutomat::CAutomat(const char &letter):CAutomat() {//TODO
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
                    blocks.top().print();
                } else if(lexem == '*') {
                    assert(blocks.size());
                    CAutomat top = blocks.top();
                    blocks.pop();
                    blocks.push(top.loop());
                } else if(lexem == EPSILON) {
                    blocks.push(CAutomat(EPSILON));
                } else {
                    blocks.push(CAutomat(lexem));
                    blocks.top().print();
                }
        }
    assert(blocks.size() == 1);
    copy(blocks.top());
}

CAutomat CAutomat::concat(const CAutomat &aut,const char &letter) {
    auto autEdges = aut.getEdges();
    int prevNodesNumber = nodesNumber;
    nodesNumber += aut.getNodesNumber();
    for(size_t i = 0; i < autEdges.size(); ++i)
        for(size_t j = 0; j < autEdges[i].size(); ++j)
            autEdges[i][j].number += prevNodesNumber;
    edges.insert(edges.end(), autEdges.begin(), autEdges.end());
    for(int prevFinalNode : finalNodes)
        addEdge(prevFinalNode, letter, aut.getStartNode() + prevNodesNumber);
    finalNodes.clear();
    for(auto finalNode : aut.getFinalNodes())
        finalNodes.insert(finalNode + prevNodesNumber);
    (*this).print();
    return *this;
}

CAutomat CAutomat::loop() {
    for(int finalNode : finalNodes)
        addEdge(finalNode, EPSILON, startNode);
    finalNodes.clear();
    finalNodes.insert(startNode);
    return *this;
}

CAutomat CAutomat::add(const CAutomat &aut) {
    auto autEdges = aut.getEdges();
    for(size_t i = 0; i < autEdges.size(); ++i)
        for(size_t j = 0; j < autEdges[i].size(); ++j)
            autEdges[i][j].number += nodesNumber;
    edges.insert(edges.end(), autEdges.begin(), autEdges.end());
    for(auto finalNode : aut.getFinalNodes())
        finalNodes.insert(finalNode + nodesNumber);
    int startNode2 = aut.getStartNode() + nodesNumber;
    nodesNumber += aut.getNodesNumber();
    addNode();
    addEdge(nodesNumber - 1, EPSILON, startNode);
    addEdge(nodesNumber - 1, CAutomat::EPSILON, startNode2);
    setStart(nodesNumber - 1);
    return *this;
}
const char CAutomat::EPSILON;
const int CAutomat::INF;
