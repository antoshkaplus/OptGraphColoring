#pragma once

#include "util.hpp"


// if graph is dence it's much better to keep adjacent colors in array and not map
// also it's much better to make colors as a sequence 

// mutable class
// here we expect that colors could be any number and not necessarily ordered

// instead of inheritance use shared_ptr to reuse graph
// can also use template to pick reuse or not
class ColoredGraph : public Graph {
private:

    using CountIndex = ant::CountIndex<false, Color, Color>;

    Coloring                    coloring;
    // support data structure
    // node, color => number of adjacent nodes with that color 
    vector<CountIndex>          _adjacentNodesOfColorCount;
    // how many nodes with the particular color
    CountIndex                  _nodeCountOfColor;

public:
    ColoredGraph() {}
    ColoredGraph(const Graph& gr) : Graph(gr), coloring(gr.nodeCount()) {

        _adjacentNodesOfColorCount.resize(nodeCount());
    }

    ColoredGraph(const Graph& gr, const vector<Color>& coloring)
        : Graph(gr), coloring(coloring) {
        
        _adjacentNodesOfColorCount.resize(nodeCount());
        for (Node i = 0; i < nodeCount(); i++) {
            if (color(i) == COLOR_NONE) {
                continue;
            }
            
            for (Node j : nextNodes(i)) {
                _adjacentNodesOfColorCount[j].Increase(color(i));
            }
            _nodeCountOfColor.Increase(color(i));
        }
    }
    
    size_t adjacentNodesOfColorCount(Node i, Color c) const {
        return _adjacentNodesOfColorCount[i][c];
    } 

    const vector<Color>& adjacentColors(Node i) const {
        return _adjacentNodesOfColorCount[i].keys();
    }
    
    const CountIndex& adjacentNodesOfColorCount(Node i) {
        return _adjacentNodesOfColorCount[i];
    }
    
    Count adjacentNodesWithSameColorCount(Node i) {
        return adjacentNodesOfColorCount(i, color(i));
    }
    
    
    const vector<Color> colors() const {
        return _nodeCountOfColor.keys();
    }
    
    size_t nodeCountOfColor(Color c) const {
        return _nodeCountOfColor[c];
    }
    
    const CountIndex& nodeCountOfColor() const {
        return _nodeCountOfColor;
    }
    
    size_t colorCount() const {
        return _nodeCountOfColor.size();
    }
    
    size_t adjacentColorCount(Node i) const {
        return _adjacentNodesOfColorCount[i].size();
    }  
    
    size_t adjacentUncoloredNodeCount(Node i) const {
        size_t count = 0;
        for (Node j : nextNodes(i)) {
            if (isColored(j) == false) count++;
        }
        return count;
    }
    
    Color color(Node i) const {
        return coloring.color(i);
    }

    size_t uncoloredNodeCount() const {
        return coloring.uncoloredNodes().size();
    }
    
    const vector<Node>& uncoloredNodes() const {
        return coloring.uncoloredNodes();
    }
  
    bool isColored(Node i) const {
        return coloring[i] != COLOR_NONE;
    }
  
    // i: index of node, which should be without color
    void unsetColor(int i) {
        int c = color(i);
        assert(c != -1); // unsetting uncolored node!
        for (int j : nextNodes(i)) {
            _adjacentNodesOfColorCount[j].Decrease(c);
        }
        _nodeCountOfColor.Decrease(c);
        coloring.reset(i);
    }
  
    void setColor_2(int i, int c) {
        if (isColored(i)) unsetColor(i);
        setColor(i, c);
    }
  
    // call this method if you want to 
    // set a color of an !!!uncolored node!!!
    void setColor(int i, int c) {
        assert(color(i) == -1); // setting color of colored node!;
        for (int j : nextNodes(i)) {
            _adjacentNodesOfColorCount[j].Increase(c);
        }
        _nodeCountOfColor.Increase(c);
        coloring.set(i, c);
    }

    // set any available color to node
    void setColor(Node i) {
        assert(adjacentColorCount(i) < colorCount());
        for (Color c : colors()) {
            if (adjacentNodesOfColorCount(i, c) == 0) {
                setColor(i, c);
                break;
            }
        }
    }

    // sets another first possible color for a node
    void resetColor(Node i) {
        int cMain = color(i);
        if (cMain == -1) cout << "resetting color of uncolored node!\n";
        unsetColor(i);
        for (auto& c : _nodeCountOfColor.keys()) {
            if (_adjacentNodesOfColorCount[i][c] && c != cMain) {
                setColor(i, c);
                break;
            }
        }
    }
  
    // if some color is already set
    void resetColor(int i, int c) {
        unsetColor(i);
        setColor(i, c);
    }

    // don't use with uncolored nodes
    // colored graph should be feasible for color of node i0 and color 
    void kempeChainSwap(Node i0, Color c1) {
        assert(color(i0) != COLOR_NONE && c1 != -1);
        static vector<bool> recoloredNodes;
        recoloredNodes.clear();
        recoloredNodes.resize(nodeCount(), false);

        static stack<Node> nodesToHandle;
        Color c0 = color(i0);
        nodesToHandle.push(i0);
        while (!nodesToHandle.empty()) {
            Node i = nodesToHandle.top();
            nodesToHandle.pop();
            // future color for this node
            Color c = color(i) == c0 ? c1 : c0;
            resetColor(i, c);
            recoloredNodes[i] = true;
            for (Node j : nextNodes(i)) {
                if (color(j) == c && !recoloredNodes[j]) {
                    nodesToHandle.push(j);
                }
            }
        }
    }
};


bool isFeasibleColoring(const ColoredGraph& c_gr) {
    if (c_gr.uncoloredNodeCount() > 0) return false;
    for (Node i = 0; i < c_gr.nodeCount(); i++) {
        if (c_gr.adjacentNodesOfColorCount(i, c_gr.color(i)) > 0) return false;
    }
    return true;
}
