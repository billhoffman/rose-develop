#ifndef BACKSTROKE_PATHNUMGENERATOR_H
#define	BACKSTROKE_PATHNUMGENERATOR_H

#include "types.h"

namespace Backstroke
{


//! Given a DAG with its entry and exit, this class generates path numbers on its edges and vertices.
class PathNumGenerator
{
	typedef boost::graph_traits<DAG>::vertex_descriptor Vertex;
	typedef boost::graph_traits<DAG>::edge_descriptor Edge;

    typedef std::vector<Edge> Path;

    // The path set on each vertex contains what path number this vertex can see
    // and the corresponding path set of this number. The union of all path sets
    // for all numbers is also given.
    struct PathSetOnVertex
    {
        // Each visible incomplete path number and its corresponding path set.
        std::map<int, PathSet> numToPath;

        // The path set on this vertex, which should be the union of all path set above.
        PathSet allPath;
    };

    //! The DAG on which we'll add path numbers.
    const DAG& dag_;

    //! The entry of the dag. This may be the entry of the CFG, or the header of a loop.
    Vertex entry_;

    //! The exit of the dag. This may be the exit of the CFG, or the exit of a loop.
    //! An exit of a loop is not a node in the CFG, but a virtual node representing outside the loop.
    Vertex exit_;

    //! A table mapping each vertex to the number of paths on it.
    std::map<Vertex, int> pathNumbers_;

    //! A table mapping each edge to the number attached to it.
    std::map<Edge, int> edgeValues_;

    //! All paths in this DAG. The i_th path has the path number i.
    std::vector<Path> paths_;

    std::map<Vertex, PathSetOnVertex> pathsForNode_;
    std::map<Edge,   PathSet> pathsForEdge_;
    //std::map<Vertex, std::set<int> > pathsForNode_;

public:
    PathNumGenerator(const DAG& dag, Vertex entry, Vertex exit)
        : dag_(dag), entry_(entry), exit_(exit)
    {}

    //! Returns the visible incomplete path numbers and their corresponding complete 
    //! paths set of the given DAG node.
    std::map<int, PathSet> getVisibleNumAndPaths(Vertex v) const
    {
        ROSE_ASSERT(pathsForNode_.count(v) > 0);
        return pathsForNode_.find(v)->second.numToPath;
    }

    PathSet getPaths(Vertex v) const
    {
        ROSE_ASSERT(pathsForNode_.count(v) > 0);
        return pathsForNode_.find(v)->second.allPath;
    }

    PathSet getPaths(const Edge& e) const
    {
        ROSE_ASSERT(pathsForEdge_.count(e) > 0);
        return pathsForEdge_.find(e)->second;
    }
    
    void generatePathNumbers()
    {
        getEdgeValues();
        getAllPaths();
        getAllPathNumForNodesAndEdges();
    }
    
    size_t getPathNum() const { return paths_.size(); }

private:
    //! Assign values to edges then each path has a unique number.
    void getEdgeValues();

    //! Find all paths in this DAG.
    void getAllPaths();

    //! For each vertex in the DAG, find all paths which contain it.
    void getAllPathNumForNodesAndEdges();
};

class PathNumManager
{
	typedef boost::graph_traits<DAG>::vertex_descriptor DAGVertex;
	typedef boost::graph_traits<DAG>::edge_descriptor   DAGEdge;
    typedef BackstrokeCFG::Vertex CFGVertex;
    typedef BackstrokeCFG::Edge   CFGEdge;

    const BackstrokeCFG& cfg_;

    //! The first DAG is about the function, and others are all loops.
    std::vector<DAG> dags_;

    //! A table from each CFG vertex to the index of the DAG and the DAG vertex.
    std::map<CFGVertex, std::pair<int, DAGVertex> > vertexToDagIndex_;

    //! A table from each CFG edge to the index of the DAG and the DAG vertex.
    std::map<CFGEdge, std::pair<int, DAGEdge> > edgeToDagIndex_;

    std::vector<PathNumGenerator*> pathNumGenerators_;

    std::map<SgNode*, BackstrokeCFG::Vertex> nodeCFGVertexMap_;
    
    //! Each element consists of the path index of the parent 
    //! region, and the number of paths.
    std::vector<std::pair<int, int> > pathInfo_;
    
    //std::map<SgNode*, std::pair<int, Vertex> > sgNodeToVertexMap_;

    //std::map<SgNode*, std::pair<int, DAGVertex> > sgNodeToVertexMap_;
    
public:
    PathNumManager(const BackstrokeCFG& cfg);
    ~PathNumManager();

    //! Get path numbers from a AST node.
    std::pair<int, PathSet> getPathNumbers(SgNode* node) const;

    //! Get path numbers from two AST nodes, which form a CFG edge.
    std::pair<int, PathSet> getPathNumbers(SgNode* node1, SgNode* node2) const;

    //! Get visible incomplete path numbers and their cooresponding complete path numbers.
    //! The first member in the returned pair is the DAG index, and the second is a table
    //! from each incomplete visible path number to its corresponding path set.
    std::pair<int, std::map<int, PathSet> >
    getVisiblePathNumbers(SgNode* node) const;

    //! Given the DAG index, return how many paths it has.
    size_t getPathNum(int index) const
    { return pathInfo_[index].second; }

private:
    //! Use path number generator to generate path numbers.
    void generatePathNumbers();

    //! Build a table from each SgNode to its belonging CFG vertex.
    void buildNodeCFGVertexMap();

    //! Given a AST node, find its belonged CFG node.
    CFGVertex getCFGNode(SgNode* node) const;
};




} // end of Backstroke

#endif	/* BACKSTROKE_PATHNUMGENERATOR_H */

