#ifndef CHANGE_CPP
#define CHANGE_CPP

/* Change.cpp
description:
Module implementing change item and requests.
version history:
ver3 -24/07/14, update by Nicolao
        -implemented ChangeRequestDatabase
ver2 -24/07/14, small update by Nicolao
        -fixed string equality checking, and now saves infoElement on program termination
ver1 -24/07/12, original by Nicolao
        -untested and desiring more checking for data integrity
*/

//==================

#include "Change.h"
#include "Constants.h"
#include <fstream>
#include <cstring>

//==================

// utilities for select
int64_t ChangeItemDatabase::previouslyAccessed[20]; // records the indexes of the previous elements accessed, curcular array
int64_t ChangeItemDatabase::previouslyAccessedPosition = 0; // position in previously accessed

int64_t ChangeRequestDatabase::previouslyAccessed[20];
int64_t ChangeRequestDatabase::previouslyAccessedPosition = 0;

// utilities for file interaction
const char* ChangeItemDatabase::filename = "Change.dat";
std::fstream ChangeItemDatabase::itemData; // database elements
int64_t ChangeItemDatabase::fileIndex = 0; // currently viewed element in database file
int64_t ChangeItemDatabase::changeItemCount = 0; // this value is determined at initialisation

const char* ChangeRequestDatabase::filename = "Request.dat";
std::fstream ChangeRequestDatabase::requestData;
int64_t ChangeRequestDatabase::fileIndex = 0;
int64_t ChangeRequestDatabase::changeRequestCount = 0;

//==================

bool ChangeItemDatabase::init()
{
    // assert that the module is only initialised once
    static bool isInitialised = 0;
    if (isInitialised)
    {
        return 1;
    }
    
    // open database file 
    fileIndex = 0;
    itemData.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!itemData.is_open())
    {
        return 1;
    }

    // retrieve from file the first element. first element contains info about number of elements
    change_item infoElement;
    getNext(infoElement);
    changeItemCount = infoElement.id;

    // initialise private data members used for select
    previouslyAccessedPosition = 0;

    // successful run
    isInitialised = 1;
    return 0;
}

bool ChangeItemDatabase::uninit()
{
    // assert that module is only unintialised once
    static bool isUninitialised = 0;
    if (isUninitialised)
    {
        return 1;
    }

    // clean up

    // update info element
    itemData.seekg(std::ios::beg);
    change_item infoElement;
    infoElement.id = changeItemCount;
    // write to file
    char buffer[sizeof(change_item)];
    memcpy(buffer, &infoElement, sizeof(change_item));
    itemData.write(buffer, sizeof(change_item));

    // close file
    itemData.close();

    // successful run
    isUninitialised = 1;
    return 0;
}

bool ChangeItemDatabase::writeElement(change_item& readIn)
{
    // fail if uninitialised
    if (!itemData.is_open())
    {
        return 1;
    }

    // case for create new
    if (readIn.id == -1)
    {
        itemData.seekp(sizeof(change_item) * (changeItemCount + 1));
        changeItemCount++;
    }
    // case for update existing
    else if (readIn.id < changeItemCount)
    {
        itemData.seekp(sizeof(change_item) * (readIn.id + 1));
    }
    else
    {
        return 1;
    }

    // write to file
    char buffer[sizeof(change_item)];
    memcpy(buffer, &readIn, sizeof(change_item));
    itemData.write(buffer, sizeof(change_item));
    return 0;
}

bool ChangeItemDatabase::getNext(change_item& readInto)
{
    // fail if uninitialised
    if (!itemData.is_open())
    {
        return 1;
    }

    // record index
    int elementPosition = itemData.tellg();
    // buffer byte block for contentss
    char buffer[sizeof(change_item)];
    // read to buffer
    itemData.read(buffer, sizeof(change_item));

    // assert the read succeeded
    if (!itemData.good())
    {
        return 1;
    }

    // copy bytes of buffer into change_item
    memcpy(&readInto, buffer, sizeof(change_item));
    // cache the file index of this element
    previouslyAccessed[previouslyAccessedPosition] = elementPosition;
    previouslyAccessedPosition = (previouslyAccessedPosition + 1) % SIZE_OF_ACCESSED_INDEX_CACHE;
    return 0;
}

bool ChangeItemDatabase::getNext(change_item& readInto, change_item& filter)
{
    // fail if uninitialised
    if (!itemData.is_open())
    {
        return 1;
    }

    // buffer byte block for contents
    char buffer[sizeof(change_item)];
    int elementPosition;

    // read elements until finding one that meets filter requirements or until end of file is reached
    while (1)
    {
        // record index
        elementPosition = itemData.tellg();
        // read to buffer
        itemData.read(buffer, sizeof(change_item));

        // if no match is ever found and end of file is reached, exit
        if (!itemData.good())
        {
            return 1;
        }

        // copy bytes of buffer into change_item
        memcpy(&readInto, buffer, sizeof(change_item));

        bool idMatch = (readInto.id == -1) || (readInto.id == filter.id);
        bool priorityMatch = (readInto.priority == -1) || (readInto.priority == filter.priority);
        bool statusMatch = (readInto.status == -1) || (readInto.status == (readInto.status & filter.status));
        bool productMatch = !strcmp(readInto.product, "") || !strcmp(readInto.product, filter.product);
        bool releaseMatch = !strcmp(readInto.release, "") || !strcmp(readInto.release, filter.release);
        // no reason to implement filter by description

        // if match is found finish filtering and deliver item satisfying filters
        if (idMatch && priorityMatch && statusMatch && productMatch && releaseMatch)
        {
            // cache the file index of this element
            previouslyAccessed[previouslyAccessedPosition] = elementPosition;
            previouslyAccessedPosition = (previouslyAccessedPosition + 1) % SIZE_OF_ACCESSED_INDEX_CACHE;
            // finish, having found a element matching filter
            return 0;
        }
    }  
}

bool ChangeItemDatabase::select(change_item& readInto, int index, int menuCount)
{
    // fail if uninitialised
    if (!itemData.is_open())
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

    // seek to index of desired item
    itemData.seekg(previouslyAccessed[desiredIndex]);

    // retrieve desired element
    // buffer byte block for contentss
    char buffer[sizeof(change_item)];
    // read to buffer
    itemData.read(buffer, sizeof(change_item));
    // copy bytes of buffer into change_item
    memcpy(&readInto, buffer, sizeof(change_item));
    
    previouslyAccessedPosition = 0;
    return 0;
}

bool ChangeItemDatabase::seekToBeginning()
{
    // fail if uninitialised
    if (!itemData.is_open())
    {
        return 1;
    }

    // move file pointer to the beginning of database
    // first element is info element and should be ignored
    // so move to the beginning offset by one element
    itemData.seekg(sizeof(change_item));
    return 0;
}

// function that returns the number of change items when called
int ChangeItemDatabase::getChangeItemCount(){
    return changeItemCount;
}

//==================

bool ChangeRequestDatabase::init()
{
    // assert that the module is only initialised once
    static bool isInitialised = 0;
    if (isInitialised)
    {
        return 1;
    }
    
    // open database file 
    filename = "Request.dat";
    fileIndex = 0;
    requestData.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!requestData.is_open())
    {
        return 1;
    }

    // retrieve from file the first element. first element contains info about number of elements
    change_request infoElement;
    getNext(infoElement);
    changeRequestCount = infoElement.changeItemId;

    // initialise private data members used for select
    previouslyAccessedPosition = 0;

    // successful run
    isInitialised = 1;
    return 0;
}

bool ChangeRequestDatabase::uninit()
{
    // assert that module is only unintialised once
    static bool isUninitialised = 0;
    if (isUninitialised)
    {
        return 1;
    }

    // clean up

    // update info element
    requestData.seekg(std::ios::beg);
    change_request infoElement;
    infoElement.changeItemId = changeRequestCount;
    // write to file
    char buffer[sizeof(change_request)];
    memcpy(buffer, &infoElement, sizeof(change_request));
    requestData.write(buffer, sizeof(change_request));

    // close file
    requestData.close();

    // successful run
    isUninitialised = 1;
    return 0;
}

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
        requestData.seekp(sizeof(change_request) * (changeRequestCount + 1));
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

        bool idMatch = (readInto.changeItemId == -1) || (readInto.changeItemId == filter.changeItemId);
        bool requesterMatch = (readInto.requesterId == -1) || (readInto.requesterId == filter.requesterId);
        bool dateMatch = (!strcmp(readInto.requestDate, "")) || (!strcmp(readInto.requestDate, filter.requestDate));
        bool releaseMatch = (!strcmp(readInto.release, "")) || (!strcmp(readInto.release, filter.release));

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

bool ChangeRequestDatabase::seekToBeginning()
{
    // fail if uninitialised
    if (!requestData.is_open())
    {
        return 1;
    }

    // move file pointer to the beginning of database
    // first element is info element and should be ignored
    // so move to the beginning offset by one element
    requestData.seekg(sizeof(change_request));
    return 0;
}

#endif
