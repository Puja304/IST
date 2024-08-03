/* Requester.cpp
description:
This is the implementation of the Requester module
version history:
ver4 -24/07/25, update by Nicolao Barreto
     -select function fix
ver3 -24/07/25, updated by Wah Paw Hser
    -changed init() so that it creates the requester file if it doesn't already exist
ver2 -24/07/15, updated by Wah Paw Hser
    -implemented getRequesterCount() and fixed the filename
ver1 -24/07/14, original by Wah Paw Hser
*/

//==================

#include "Requester.h"
#include <iostream>
#include <cstring>

//==================

// define static utilities for select
int64_t RequesterDatabase::previouslyAccessed[20] = {0};
int64_t RequesterDatabase::previouslyAccessedPosition = 0;

// define static utilities for file interaction
const char* RequesterDatabase::filename = "Requester.dat";
std::fstream RequesterDatabase::requesterData;
int64_t RequesterDatabase::fileIndex = 0;
int64_t RequesterDatabase::requesterCount = 0;

//==================

/* function init:
    this function is implemented to initialize the requester database by opening the file, create
    a file, if it doesn't already exist, and calculate the number of reqeusters currently in the
    database, and intitialize the utility variables to 0. Returns 0 on successfuly initiazation,
    returns 1 elsewise. 
*/
bool RequesterDatabase::init() {
    // open requester file
    requesterData.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    // create file if it doesn't already exist
    if (!requesterData.is_open()) {
        requesterData.clear();
        std::ofstream createFile(filename, std::ios::out | std::ios::binary);
        createFile.close();
        requesterData.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    // return 1 if the file does not open
    if (!requesterData.is_open()) {
        return 1;
    }

    // calculate requesterCount
    requesterData.seekg(0, std::ios::end);
    requesterCount = requesterData.tellg()/sizeof(requester);
    requesterData.seekg(0, std::ios::beg);

    // initialize utilites to 0
    fileIndex = 0;
    previouslyAccessedPosition = 0;
    memset(previouslyAccessed, 0, sizeof(previouslyAccessed)); 

    return 0;
}

//==================

/* function uninit:
    this function is implemented to uninitialize the requester database by closing the file,
    if it is open.
*/
bool RequesterDatabase::uninit() {
    if (requesterData.is_open()) {
        requesterData.close();
        return 0;
    }
    return 1;
}

//==================

/* function writeElement:
    this function is implemented to write a new requester to the end of the requester file and
    update the requester count if the requester is successfully written.
*/
bool RequesterDatabase::writeElement(requester& readIn) {
    // write new element to the end of the file
    requesterData.seekp(0, std::ios::end);
    requesterData.write(reinterpret_cast<char*>(&readIn), sizeof(requester));

    // return 1 if writing the element is not successful
    if (requesterData.fail()) {
        return 1;
    }
    
    requesterCount++;
    return 0;
}

//==================

/* function getNext(requester):
    this function is implented to read the next requester in the file (if there are still more
    requesters to read from) into readInto and update the file access pointers.
*/
bool RequesterDatabase::getNext(requester& readInto) {
    // return 1 if the requester file is not open, or there are no more requesters to read into
    if (!requesterData.is_open() || fileIndex >= requesterCount) {
        return 1;
    }

    // read next requester from file
    requesterData.seekg(fileIndex * sizeof(requester), std::ios::beg);
    requesterData.read(reinterpret_cast<char*>(&readInto), sizeof(requester));

    // return 1 if the data cannot be read
    if (requesterData.fail()) {
        return 1;
    }

    // update file access indexes
    previouslyAccessed[previouslyAccessedPosition] = fileIndex;
    previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;
    fileIndex++;

    return 0;
}

//==================

/* function getNext(requester, filter):
    this function is implemented to read the next requester in the file (if there are still more
    requesters to read from) into readInto. if the filter matches the current requester being read, 
    update the file access pointers.
*/
bool RequesterDatabase::getNext(requester& readInto, requester& filter) {

    // while there are still requesters to read into, read the current requester and check if the 
    // filter matches
    while (fileIndex < requesterCount) {
        // read next requester from file
        requesterData.seekg(fileIndex * sizeof(requester), std::ios::beg);
        requesterData.read(reinterpret_cast<char*>(&readInto), sizeof(requester));

        // return 1 if data cannot be read
        if (requesterData.fail()) {
            return 1;
        }

        // check if the current requester record matches the filter
        bool match = true;
        if (strlen(filter.name) != 0 && strcmp(readInto.name, filter.name) != 0) {
            match = false;
        }
        if (strlen(filter.phone) != 0 && strcmp(readInto.phone, filter.phone) != 0) {
            match = false;
        }
        if (strlen(filter.email) != 0 && strcmp(readInto.email, filter.email) != 0) {
            match = false;
        }
        if (strlen(filter.department) != 0 && strcmp(readInto.department, filter.department) != 0) {
            match = false;
        }
        if (filter.requesterId != -1 && readInto.requesterId != filter.requesterId) {
            match = false;
        }

        // if the filter matches the current requester record, update the file access variables and return 0
        if (match) {
            previouslyAccessed[previouslyAccessedPosition] = fileIndex;
            previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;
            fileIndex++;
            return 0;
        }

        // if no match, increment index and continue while loop
        fileIndex++;
    }
    // return 1 if the whole file has been read and there are no more reqeusters to read from
    return 1;
}

//==================

/* function select:
    this function is implemented to take the index from the index parameter and select a requester from
    the previously accessed ones based on the provided index using circular array calculations.
*/
bool RequesterDatabase::select(requester& readInto, int index, int menuCount) {
    // error check the index, return 1 if not legal
    if (index <= 0 || index > menuCount || index > 20) {
        return 1;
    }

    // find the index of the selected in the circular array
    int64_t position = previouslyAccessed[(previouslyAccessedPosition - 1 + index - menuCount) % 20];

    // seek and read the requester
    requesterData.seekg(previouslyAccessed[position] * sizeof(requester), std::ios::beg);
    requesterData.read(reinterpret_cast<char*>(&readInto), sizeof(requester));

    // return 1 if data could not be read
    if (requesterData.fail()) {
        return 1;
    }

    return 0;
}

//==================

/* function seekToBeginning:
    this function is implemented to seek to the beginning of the file and reset the file
    access index.
*/
bool RequesterDatabase::seekToBeginning() {
    // return position in the file to the beginning
    requesterData.clear();
    requesterData.seekg(0, std::ios::beg);
    fileIndex = 0;
    return 0;
}

//==================

/* function getRequesterCount:
    this function is implemented to return the requesterCount variable in the RequesterDatabase class
*/
int64_t RequesterDatabase::getRequesterCount() {
    return requesterCount;
}
