#include <map>
#include <string>
#define NUMBEROFNEIGHBOURS 4
#define NODENAMELENGTH 3

enum nodeState { nodeUsed, nodeFree };

class Node;

typedef std::map<std::string, Node*> nodeMap;

struct neighbournode{
	std::string name;
	unsigned int distance;
};

class Node {
private:
	struct neighbournode _neighbours[NUMBEROFNEIGHBOURS];
	unsigned int count; 
	std::string _name;
	static nodeMap nodelist; 
	nodeState state; 
public:
	Node(std::string name);
	~Node();

	void addNeighbournode(std::string nameNode, unsigned int distance);

	std::string getName(){return this->_name;};
	
	unsigned int getNeighbourCount(void) {return this->count;};
	struct neighbournode* getNeighbournodeStruct(int number);
	int getNeighbourDistance(int number);
	Node* getNeighbournodePtr(int number);
	nodeState getState(void){return this->state;};
	void setState(nodeState newstate){this->state = newstate;};
	
	static Node* getParticularNode(std::string name){return nodelist[name];};
	static nodeMap* getNodeList(void){return &(Node::nodelist);};
	static void deleteAllNodes(void);
	static int getNodeListSize(void){return Node::nodelist.size();};

	void print(void);
};
