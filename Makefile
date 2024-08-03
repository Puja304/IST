all: ITS

ITS: Main.o ScenarioControl.o ChangeItem.o ChangeRequest.o Product.o Release.o Requester.o
	g++ -O2 -Wall -Wpedantic -std=c++17 Main.cpp ChangeItem.cpp ChangeRequest.cpp ScenarioControl.cpp Requester.cpp Product.cpp Release.cpp -o ITS.exe
	