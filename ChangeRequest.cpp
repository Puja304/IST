/* ChangeRequest.cpp
description:
Module implementing change requests.

elements are unordered for add in O(1)
elements are searched linearly for simplicity and assurance of functionality

version history:
ver5 -24/07/16, update by Nicolao
        -separation of change item and change request modules
        -update to init function
            -new method to find item count
ver4 -24/07/16, update by Nicolao
        -updated write
        -updated filter logic
        -handles eof error
ver3 -24/07/14, update by Nicolao
        -implemented ChangeRequestDatabase
ver2 -24/07/14, small update by Nicolao
        -fixed string equality checking, and now saves infoElement on program termination
ver1 -24/07/12, original by Nicolao
        -untested and desiring more checking for data integrity
*/

#ifndef CHANGE_REQUEST_CPP
#define CHANGE_REQUEST_CPP

//==================

#include "ChangeRequest.h"
#include "Constants.h"
#include <fstream>
#include <cstring>

//==================

// utilities for select
int64_t ChangeRequestDatabase::previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
int64_t ChangeRequestDatabase::previouslyAccessedPosition = 0; // position in previously accessed

// utilities for file interaction
const char* ChangeRequestDatabase::filename = "Request.dat";
std::fstream ChangeRequestDatabase::requestData; // database elements
int64_t ChangeRequestDatabase::changeRequestCount = 0; // this value is determined at initialisation

//==================

// long term storage is implemented through locally stored files
// gets request count by dividing file content length by number size of entry
bool ChangeRequestDatabase::init()
{
    // assert that the module is only initialised once
    static bool isInitialised = 0;
    if (isInitialised)
    {
        return 1;
    }
    
    // open database file 
    requestData.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!requestData.is_open()) // if file not found create file
    {
        requestData.clear();
        std::ofstream createFile(filename, std::ios::out | std::ios::binary);
        createFile.close();
        requestData.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }

    if (!requestData.is_open()) // if file still cannot be opened fail to initialise
    {
        return 1;
    }

    // retrieve from file the first element. first element contains info about number of elements
    requestData.seekg(0, std::ios::end);
    changeRequestCount = ((requestData.tellg()) / (sizeof(change_request)));
    seekToBeginning();

    // successful run
    isInitialised = 1;
    return 0;
}

//========

// closes file
bool ChangeRequestDatabase::uninit()
{
    // assert that module is only unintialised once
    static bool isUninitialised = 0;
    if (isUninitialised)
    {
        return 1;
    }

    // clean up
    // close file
    requestData.close();

    // successful run
    isUninitialised = 1;
    return 0;
}

//========

// save an element to database by copying it into a buffer of bytes and then writing those bytes to file
bool ChangeRequestDatabase::writeElement(change_request& readIn)
{
    // fail if uninitialised
    if (!requestData.is_open())
    {
        return 1;
    }

    // case for create new
    if ((readIn.requesterId != -1) && (readIn.changeItemId != -1)) // the change request has some associated requester and item
    {
        requestData.seekp(sizeof(change_request) * changeRequestCount);
        changeRequestCount++;
    }
    else
    {
        return 1;
    }

    // write to file
    char buffer[sizeof(change_request)];
    memcpy(buffer, &readIn, sizeof(change_request));
    requestData.write(buffer, sizeof(change_request));
    return 0;
}

//========

// loads a read from the file into passed request
bool ChangeRequestDatabase::getNext(change_request& readInto)
{
    // fail if uninitialised
    if (!requestData.is_open())
    {
        return 1;
    }

    // record index
    int elementPosition = requestData.tellg();
    // buffer byte block for contentss
    char buffer[sizeof(change_request)];
    // read to buffer
    requestData.read(buffer, sizeof(change_request));

    // assert the read succeeded
    if (!requestData.good())
    {
        return 1;
    }

    // copy bytes of buffer into change_request
    memcpy(&readInto, buffer, sizeof(change_request));
    // cache the file index of this element
    previouslyAccessed[previouslyAccessedPosition] = elementPosition;
    previouslyAccessedPosition = (previouslyAccessedPosition + 1) % SIZE_OF_ACCESSED_INDEX_CACHE;
    return 0;
}

//========

// loads a read from the file into passed request
// will only load elements that are similar to filter element
// searches elements linearly until finding something similar or reaching end of file
bool ChangeRequestDatabase::getNext(change_request& readInto, change_request& filter)
{
    // fail if uninitialised
    if (!requestData.is_open())
    {
        return 1;
    }

    // buffer byte block for contents
    char buffer[sizeof(change_request)];
    int elementPosition;

    // read elements until finding one that meets filter requirements or until end of file is reached
    while (1)
    {
        // record index
        elementPosition = requestData.tellg();
        // read to buffer
        requestData.read(buffer, sizeof(change_request));

        // if no match is ever found and end of file is reached, exit
        if (!requestData.good())
        {
            return 1;
        }

        // copy bytes of buffer into change_request
        memcpy(&readInto, buffer, sizeof(change_request));

        bool idMatch = (filter.changeItemId == -1) || (readInto.changeItemId == filter.changeItemId);
        bool requesterMatch = (filter.requesterId == -1) || (readInto.requesterId == filter.requesterId);
        bool dateMatch = (!strcmp(filter.requestDate, "")) || (!strcmp(readInto.requestDate, filter.requestDate));
        bool releaseMatch = (!strcmp(filter.release, "")) || (!strcmp(readInto.release, filter.release));

        // if match is found finish filtering and deliver request satisfying filters
        if (idMatch && requesterMatch && dateMatch && releaseMatch)
        {
            // cache the file index of this element
            previouslyAccessed[previouslyAccessedPosition] = elementPosition;
            previouslyAccessedPosition = (previouslyAccessedPosition + 1) % SIZE_OF_ACCESSED_INDEX_CACHE;
            // finish, having found a element matching filter
            return 0;
        }
    }  
}

//========

// retrieve and load a recently accessed request from file
bool ChangeRequestDatabase::select(change_request& readInto, int index, int menuCount)
{
    // fail if uninitialised
    if (!requestData.is_open())
    {
        return 1;
    }
    
    // algebraically find the index of the position of the desired element
    // using a circular array
    // using the length of the array: SIZE_OF_INDEX_CACHE
    // using the number of options in a menu printout: menuCount
    // and using the selected index in that menu printout: index
    int desiredIndex = (previouslyAccessedPosition -1 + index - menuCount) % SIZE_OF_ACCESSED_INDEX_CACHE;

    // wrap around for negative indexes
    if (desiredIndex < 0)
    {
        desiredIndex = SIZE_OF_ACCESSED_INDEX_CACHE + desiredIndex;
    }

    // seek to index of desired request
    requestData.seekg(previouslyAccessed[desiredIndex]);

    // retrieve desired element
    // buffer byte block for contentss
    char buffer[sizeof(change_request)];
    // read to buffer
    requestData.read(buffer, sizeof(change_request));
    // copy bytes of buffer into change_request
    memcpy(&readInto, buffer, sizeof(change_request));
    
    previouslyAccessedPosition = 0;
    return 0;
}

//========

// move file pointer to beginning
// recover from eof flag
bool ChangeRequestDatabase::seekToBeginning()
{
    // fail if uninitialised
    if (!requestData.is_open())
    {
        return 1;
    }

    requestData.clear(); // clear eof error flag

    // move file pointer to the beginning of database
    // first element is info element and should be ignored
    // so move to the beginning offset by one element
    requestData.seekg(std::ios::beg);
    return 0;
}

#endif