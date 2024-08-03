g++ -O2 -Wall -Wpedantic -std=c++17 Main.cpp ChangeItem.cpp ChangeRequest.cpp ScenarioControl.cpp Requester.cpp Product.cpp Release.cpp -o ITS.exe
g++ -O2 -std=c++17 Prepop.cpp ChangeItem.cpp ChangeRequest.cpp Requester.cpp Product.cpp Release.cpp -o PREPOP.exe
g++ -O2 -std=c++17 testprepop.cpp ChangeItem.cpp ChangeRequest.cpp Requester.cpp Product.cpp Release.cpp -o TESTPREPOP.exe
