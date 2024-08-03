/* Release.cpp
description:
This is the implementation of the Release module
version history:
ver2 -24/07/28, select function and filtered getnext fix by Nicolao Barreto
ver1 -24/07/14, original by Allan Hu
*/

//==================

#include <iostream>
#include <fstream>
#include <cstring>
#include "Release.h"
#include "Constants.h"

//==================

//release utility variables
int64_t Release::previouslyAccessed[20] = {0};  // records the indexes of the previous elements accessed, curcular array
int64_t Release::previouslyAccessedPosition = 0;  // position in previously accessed

const char* Release::filename = "Release.dat";
std::fstream Release::releaseFile;  // release elements
int64_t Release::fileIndex = 0;   // currently viewed element in release file
int64_t Release::releaseCount = 0;  // amount of releases

//==================

bool Release::initRelease()
{
    // open release file 
    releaseFile.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    // if file not found create file
    if (!releaseFile.is_open()) 
    {
        releaseFile.clear();
        std::ofstream createFile(filename, std::ios::out | std::ios::binary);
        createFile.close();
        releaseFile.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }

    // if file still cannot be opened fail to initialise
    if (!releaseFile.is_open()) 
    {
        return 1;
    }

    // initialize releaseCount
    releaseFile.seekg(0, std::ios::end);
    releaseCount = releaseFile.tellg()/sizeof(release);
    releaseFile.seekg(0, std::ios::beg);

    // initialize fileIndex to 0
    fileIndex = 0; 
    //initialize previouslyAccessedPosition to 0
    previouslyAccessedPosition = 0; 
    // initialize previouslyAccessed to 0
    memset(previouslyAccessed, 0, sizeof(previouslyAccessed));  

    return 0;
}

//==================

bool Release::uninitRelease()
{
    //check that the release file is open and close it if so
    if (releaseFile.is_open()) 
    {
        releaseFile.close();
        return 0;
    }
    else
    {
        return 1;
    }
}

//==================

bool Release::writeRelease( release& readIn)
{
    // go to end of file
    releaseFile.seekp(0, std::ios::end); 
    // add new release to the end of the file
    releaseFile.write(reinterpret_cast<char*>(&readIn), sizeof(release)); 

    //return 1 if unable to write to release file
    if(releaseFile.fail())
    {
        return 1;
    }

    // increment releaseCount if added successfully
    releaseCount++; 
    return 0;
}

//==================

bool Release::getNext(release& readInto)
{
    // return 1 if the requester file is not open, or there are no more requesters to read into
    if (!releaseFile.is_open() || fileIndex >= releaseCount) 
    {
        return 1;
    }
        // read next release from file
        releaseFile.seekg(fileIndex * sizeof(release), std::ios::beg);
        releaseFile.read(reinterpret_cast<char*>(&readInto), sizeof(release));

        // return 1 if data cannot be read
        if (releaseFile.fail()) 
        {
            return 1;
        }
        // update file access indexes
        previouslyAccessed[previouslyAccessedPosition] = fileIndex;
        previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;
        fileIndex++;
        return 0;
}

//==================

bool Release::getNext(release& readInto, release& filter )
{
    while (fileIndex < releaseCount) 
    {
        // read next release from file
        releaseFile.seekg(fileIndex * sizeof(release), std::ios::beg);
        releaseFile.read(reinterpret_cast<char*>(&readInto), sizeof(release));

        // return 1 if data cannot be read
        if (releaseFile.fail()) 
        {
           return 1;
        }

        // check if the current release record matches the filter
        if (((strlen(filter.name) == 0 || (strcmp(readInto.name, filter.name) == 0))) &&
           ((strlen(filter.date) == 0 || (strcmp(readInto.date, filter.date) == 0))) &&
           ((strlen(filter.releaseId) == 0) || !strncmp(filter.releaseId, readInto.releaseId, MAX_RELEASE_ID_SIZE)))
        {
           
           // update file access indexes
           previouslyAccessed[previouslyAccessedPosition] = fileIndex;
           previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;
           fileIndex++;
           return 0;
        }

        // if no match, increment index and continue while loop
        fileIndex++;
    }
    return 1;
}

//==================

bool Release::select(release& readInto,int index, int menuCount)
{
    // error check the index, return 1 if not legal
    if (index <= 0 || index > menuCount || index > 20) 
    {
        return 1;
    }

    // return 1 if release file not open
    if (releaseFile.fail()) 
    {
        std::cout << "fails here";
        return 1;
    }

    // find the index of the selected in the circular array
    int64_t position = (previouslyAccessedPosition -1 + index - menuCount) % SIZE_OF_ACCESSED_INDEX_CACHE;

    // seek and read the release
    releaseFile.seekg(previouslyAccessed[position] * sizeof(release), std::ios::beg);
    releaseFile.read(reinterpret_cast<char*>(&readInto), sizeof(release));

    previouslyAccessedPosition = 0;
    return 0;
}

//==================

bool Release::seekToBeginning()
{
    //return 1 if release file is not open
    if (!releaseFile.is_open())
    {
        return 1;
    }

    // go to the beggining of the release file
    releaseFile.clear();
    releaseFile.seekg(0, std::ios::beg); 
    // update fileIndex to 0
    fileIndex = 0; 
    return 0;
}