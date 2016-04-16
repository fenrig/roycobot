pathplanner: pathplanner.o node.o dijkstra.o
	g++ -g -o pathplanner pathplanner.o node.o dijkstra.o

pathplanner.o: pathplanner.cpp
	g++ -g -c -D_REENTRANT -pthread pathplanner.cpp

node.o: node.cpp node.h
	g++ -g -c -D_REENTRANT -pthread node.cpp

dijkstra.o: dijkstra.cpp dijkstra.h
	g++ -g -c -D_REENTRANT -pthread dijkstra.cpp
