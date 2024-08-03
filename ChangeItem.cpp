/* ChangeItem.cpp
description:
Module implementing change items

orders elements by ID number, there are no deletions and new elements are always appended to the end
in this module the maintaining sort order does not cause any performance burden

elements are searched linearly for simplicity and assurance of functionality

version history:
ver6 -fix for update not allowing a change item to be changed from done or cancelled
ver5 -24/07/25 update by Nicolao
        -separated change item and request modules
        -updated init function
            -new method to find item count
        -updated write funtion
            -no longer allows update out of done or cancelled
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

#ifndef CHANGE_ITEM_CPP
#define CHANGE_ITEM_CPP

//==================

#include "ChangeItem.h"
#include "Constants.h"
#include <fstream>
#include <cstring>

//==================

// utilities for select
int64_t ChangeItemDatabase::previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
int64_t ChangeItemDatabase::previouslyAccessedPosition = 0; // position in previously accessed

// utilities for file interaction
const char* ChangeItemDatabase::filename = "Change.dat";
std::fstream ChangeItemDatabase::itemData; // database elements
int64_t ChangeItemDatabase::changeItemCount = 0; // this value is determined at initialisation

//==================

// long term storage is implemented through locally stored files
// gets item count by reading special first element in file
bool ChangeItemDatabase::init()
{
    // assert that the module is only initialised once
    static bool isInitialised = 0;
    if (isInitialised)
    {
        return 1;
    }
    
    // open database file 
    itemData.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!itemData.is_open()) // if file not found create the file
    {
        itemData.clear();
        std::ofstream createFile(filename, std::ios::out | std::ios::binary);
        createFile.close();
        itemData.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }

    if (!itemData.is_open()) // cannot create file
    {
        return 1;
    }

    itemData.seekg(0, std::ios::end);
    changeItemCount = ((itemData.tellg()) / (sizeof(change_item)));
    itemData.seekg(std::ios::beg);
    
    // successful run
    isInitialised = 1;
    return 0;
}

//========

// closes file
bool ChangeItemDatabase::uninit()
{
    // assert that module is only unintialised once
    static bool isUninitialised = 0;
    if (isUninitialised)
    {
        return 1;
    }

    // clean up
    // close file
    itemData.close();

    // successful run
    isUninitialised = 1;
    return 0;
}

//========

// save an element to database by copying it into a buffer of bytes and then writing those bytes to file
bool ChangeItemDatabase::writeElement(change_item& readIn)
{
    // fail if uninitialised
    if (!itemData.is_open())
    {
        return 1;
    }

    // case for create new
    if ((readIn.id == -1) || (readIn.id == changeItemCount + 1))
    {
        readIn.id = changeItemCount + 1;
        itemData.seekp(sizeof(change_item) * changeItemCount);
        changeItemCount++;
    }
    // case for update existing
    else if ((readIn.id > 0) && (readIn.id <= changeItemCount))
    {
        itemData.seekp(sizeof(change_item) * (readIn.id - 1));
        change_item temp;
        getNext(temp);
        if ((temp.status == done) || (temp.status == cancelled)) // cannot change an item of of done or cancelled state
        {
            return 1;
        }
        itemData.seekp(sizeof(change_item) * (readIn.id - 1));
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

//========

// loads a read from the file into passed item
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

//========

// loads a read from the file into passed item
// will only load elements that are similar to filter element
// searches elements linearly until finding something similar or reaching end of file
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

        bool idMatch = (filter.id == -1) || (readInto.id == filter.id);
        bool priorityMatch = (filter.priority == -1) || (readInto.priority == filter.priority);
        bool statusMatch = (filter.status == -1) || (readInto.status == (readInto.status & filter.status));
        bool productMatch = !strcmp(filter.product, "") || !strcmp(readInto.product, filter.product);
        bool releaseMatch = !strcmp(filter.release, "") || !strcmp(readInto.release, filter.release);
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

//========

// retrieve and load a recently accessed item from file
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
    
    return 0;
}

//========

// move file pointer to beginning
// recover from eof flag
bool ChangeItemDatabase::seekToBeginning()
{
    // fail if uninitialised
    if (!itemData.is_open())
    {
        return 1;
    }

    itemData.clear(); // clear eof error bit

    // move file pointer to the beginning of database
    // first element is info element and should be ignored
    // so move to the beginning offset by one element
    itemData.seekg(std::ios::beg);
    return 0;
}

//========

// reads and returns element count
int ChangeItemDatabase::getChangeItemCount(){
    return changeItemCount;
}

//========

#endif