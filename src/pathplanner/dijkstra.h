#include "node.h"
#include <list>

#define maxvalueint32 0xffffffff

typedef std::list<Node*> nodeVertex;
typedef std::map<Node*, Node*> nodeNodemap;
typedef std::map<Node*, unsigned int > nodeIntmap;

class dijkstra{
	private:
		nodeIntmap dist;
		nodeNodemap prev;
		nodeVertex nodeVertexSet;
		
		void prepareVertexSet(void);
		Node* getMinDistanceNode(void);	
	public:
		void calculateRoute(Node* source, Node* destination);
		void printpath(void);
};
