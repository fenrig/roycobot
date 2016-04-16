#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "dijkstra.h"

#define h_nodes 20
#define v_nodes 20

int main(void){
	// hier komt de test
/*
	int i = 0, j;
	char letter = 'a';
	char buffer[3];
	std::string* hulp;
	for(; i < h_nodes; i++){
		for(j = 0; j < v_nodes; j++){
			sprintf(buffer, "%c%d" , letter, j);
			hulp = new std::string(buffer);	
			new Node(*hulp);
			delete hulp;	
		}
		letter++;
		if(letter > 'z'){
			letter = 'a';
		}	
	}
	printf("%p\n", Node::getParticularNode(std::string("a1")));
	Node::getParticularNode(std::string("a1"))->print();
	printf("%p\n", Node::getParticularNode(std::string("b1")));
	Node::getParticularNode(std::string("b1"))->print();

	Node::deleteAllNodes();
	printf("%p\n", Node::getParticularNode(std::string("a1")));
	Node::getParticularNode(std::string("a1"))->print();
*/
	Node A1("A1");
	A1.addNeighbournode("B1", 4);
	A1.addNeighbournode("B2", 2);

	Node B1("B1");
	B1.addNeighbournode("C1", 2);

	Node B2("B2");
	B2.addNeighbournode("C3", 1);
	B2.setState(nodeUsed);

	Node C1("C1");
	C1.addNeighbournode("D1", 1);
	
	Node C2("C2");
	C2.addNeighbournode("D2", 3);
	C2.addNeighbournode("D3", 6);
	
	Node C3("C3");
	C3.addNeighbournode("C2", 1);
	C3.addNeighbournode("D4", 3);

	Node D1("D1");
	D1.addNeighbournode("D2", 9);
	
	Node D2("D2");
	D2.addNeighbournode("D1", 9);
	D2.addNeighbournode("C2", 3);

	Node D3("D3");
	D3.addNeighbournode("D4", 2);

	Node D4("D4");
	D4.addNeighbournode("D3", 2);

	dijkstra planner;
	planner.calculateRoute(&A1, &D4);
	planner.printpath();
		
		
	return 0;
}




