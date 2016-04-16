#include "node.h"
#include "stdio.h"

nodeMap Node::nodelist;

Node::Node(std::string name){
	this->count = 0;
	this->_name = name;
	this->nodelist[name] = this;
	this->state = nodeFree;
}

Node::~Node(){
	this->nodelist.erase(this->_name);
}

int Node::getNeighbourDistance(int number){
	if(number < this->count){
		return 	this->_neighbours[number].distance;
	}else{
		return 0;	
	}
}

void Node::addNeighbournode(std::string nameNode, unsigned int distance){
	if(this->count == NUMBEROFNEIGHBOURS){
		return;
	}
	struct neighbournode* node = this->_neighbours + this->count;
	node->distance = distance;
	node->name = nameNode;
	this->count++;
}

Node* Node::getNeighbournodePtr(int number){
	struct neighbournode* node = this->getNeighbournodeStruct(number);
	if( node == NULL)
		return NULL;
	else
		return Node::getParticularNode(node->name);
}

struct neighbournode* Node::getNeighbournodeStruct(int number){
	if(number < this->count){
		return 	&(this->_neighbours[number]);
	}else{
		return NULL;	
	}
}

void Node::deleteAllNodes(void){
	nodeMap *ptr = Node::getNodeList();
	for(nodeMap::iterator it=ptr->begin(); it != ptr->end(); ++it){
		//delete it->second;		
		ptr->erase(it);
	} 
}

void Node::print(void){
	printf("ptr: %p\nname: %s\n" , this, this->_name.c_str());
}


