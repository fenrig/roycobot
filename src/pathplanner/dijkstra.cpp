#include "dijkstra.h"
#include <limits.h>
#include <stdio.h>

#define DEBUG 0

void dijkstra::prepareVertexSet(void){
	Node *ptr;	
	nodeMap *maplist = Node::getNodeList();
	for(nodeMap::iterator it=maplist->begin(); it!=maplist->end(); ++it){
		ptr = it->second;
		this->dist[ptr] = UINT_MAX;
		this->prev[ptr] = NULL;
		this->nodeVertexSet.push_back(ptr);
	}
}

Node* dijkstra::getMinDistanceNode(void){
/*
	Node* minNode = NULL;
	unsigned int distance;
	for(nodeIntmap::iterator it=this->dist.begin(); it!=this->dist.end(); ++it){
		if(minNode == NULL){
			minNode = it->first;
			distance = it->second;		
		}else{
			if(it->second < distance){
				minNode = it->first;
				distance = it->second;			
			} 
		}
	}
	return minNode;
*/
	Node* minNode = NULL;
	unsigned int distance, hulp;
	for(nodeVertex::iterator it=this->nodeVertexSet.begin(); it != this->nodeVertexSet.end(); ++it){
		if(minNode == NULL){
			minNode = *it;
			distance = this->dist[*it];			
		}else{
			hulp = this->dist[*it];
			if(hulp < distance){
				minNode = *it;
				distance = hulp;				
			}			
		}
	}
	return minNode;
}

void dijkstra::printpath(void){
	printf("---------------\n");
	printf("Node \t\t prevNode \t distance\n");
	for(nodeNodemap::iterator it=this->prev.begin(); it != this->prev.end(); ++it){
		printf("%s \t\t ", it->first->getName().c_str());
		if(it->second == NULL)
			printf("NULL");
		else
			printf("%s", it->second->getName().c_str());
		printf("\t\t %u\n", this->dist[it->first]);	
	}
	printf("---------------\n");
}

void dijkstra::calculateRoute(Node* source, Node* destination){
	Node *helpNode, *neighbourNode;
	unsigned int countNeighbours, altdist, i;

	this->prepareVertexSet();
	this->dist[source] = 0;	
#if (DEBUG == 1)
	unsigned int counter = 0;
	this->printpath();
#endif

	while(! this->nodeVertexSet.empty()){

		helpNode = this->getMinDistanceNode();
		this->nodeVertexSet.remove(helpNode);

#if (DEBUG == 1)
		counter++;
		printf("helpNode: %s\n", helpNode->getName().c_str());
#endif

		if(helpNode == destination){
			this->nodeVertexSet.clear();
			return;		
		}

		countNeighbours = helpNode->getNeighbourCount();
		for(i = 0; i < countNeighbours; i++){
			neighbourNode = helpNode->getNeighbournodePtr(i);
			
			if(neighbourNode->getState() != nodeFree) continue;
			
			altdist = this->dist[helpNode] + helpNode->getNeighbourDistance(i);
#if (DEBUG == 1)
	printf("altdist: %u & this->dist[neighbourNode]: %u\n", altdist, this->dist[neighbourNode]);
#endif
			if(altdist < this->dist[neighbourNode]){
				this->dist[neighbourNode] = altdist;
				this->prev[neighbourNode] = helpNode;	
#if (DEBUG == 1)
				this->printpath();
#endif			
			}
		}
#if (DEBUG == 1)
		printf("*************\n");
		if(counter == 2)
			return;	
#endif
	}
	

}
