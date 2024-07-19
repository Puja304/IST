/* product.cpp
description:
This is the implementation of the product module
version history:
ver1 -24/07/14, original by Allan Hu
*/

//==================

#include <iostream>
#include <fstream>
#include <cstring>
#include "Product.h"
#include "Constants.h"

//==================

//product utility varibales
int64_t Product::previouslyAccessed[20] = {0};
int64_t Product::previouslyAccessedPosition = 0;

const char* Product::filename = "C:\\IssueTrackerSystem\\Product.dat";
std::fstream Product::productFile;
int64_t Product::fileIndex = 0;
int64_t Product::productCount = 0;

//release utility variables
int64_t Release::previouslyAccessed[20] = {0};
int64_t Release::previouslyAccessedPosition = 0;

const char* Release::filename = "C:\\IssueTrackerSystem\\Release.dat";
std::fstream Release::releaseFile;
int64_t Release::fileIndex = 0;
int64_t Release::releaseCount = 0;


//==================

/* function initProduct:
    this function is implemented to initialize the product database by opening the product file,
    calculate the number of reqeusters currently in the database, and intitialize the utility
    variables to 0. Returns 0 on successfuly initiazation, returns 1 elsewise. 
*/
bool Product::initProduct()
{
    
    productFile.open(filename, std::ios::in | std::ios::out | std::ios::binary); //open product file
    
    if(!productFile.is_open()) // return 1 if unable to open product file
    {
        return 1;
    }

    // initialize productCount
    productFile.seekg(0, std::ios::end);
    productCount = productFile.tellg()/sizeof(product);
    productFile.seekg(0, std::ios::beg);

    fileIndex = 0; // initialize fileIndex to 0
    previouslyAccessedPosition = 0; //initialize previouslyAccessedPosition to 0
    memset(previouslyAccessed, 0, sizeof(previouslyAccessed));  // initialize previouslyAccessed to 0
    return 0;
}

/* function uninitProduct:
    this function is implemented to uninitialize the product database by closing the file,
    if it is open.
*/
bool Product::uninitProduct()
{
    if (productFile.is_open()) // check the product file is open and if so close it 
    {
        productFile.close();
        return 0;
    }
    else
    {
        return 1;
    }
}

/* function writeProduct:
    this function is implemented to write a new product to the end of the product file and
    update the product count if the product is successfully written.
*/
bool Product::writeProduct( product& readIn)
{
    productFile.seekp(0, std::ios::end); // go to end of file
    productFile.write(reinterpret_cast<char*>(&readIn), sizeof(product)); // add new product to end of file

    if(productFile.fail())
    {
        return 1;
    }
    productCount++; // increment productCount if added successfully
    return 0;
}

/* function getNext:
    this function is implented to read the next product in the file (if there are still more
    products to read from) into readInto and update the file access pointers.
*/
bool Product::getNext(product& readInto)
{
    
    if (!productFile.is_open() || productCount == 0 ) // return 1 if product file isn't open or there are no proucts 
    { 
        return 1;
    }
    productFile.seekg(fileIndex * sizeof(product), std::ios::beg);  // find the next product
    productFile.read(reinterpret_cast<char*>(&readInto), sizeof(product)); // read the next product

    //return 1 if unable to read from file
    if (productFile.fail()) 
    {
        return 1;
    }

    // update file access indexes
    previouslyAccessed[previouslyAccessedPosition] = productFile.tellg();
    previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;

    return 0;
}

/* function getNext:
    this function is implemented to read the next product in the file (if there are still more
    products to read from) into readInto. if the filter matches the current product being read, 
    update the file access pointers.
*/
bool Product::getNext(product& readInto, product& filter )
{
    while (fileIndex < productCount) 
    {
        // read next product from file
        productFile.seekg(fileIndex * sizeof(release), std::ios::beg);
        productFile.read(reinterpret_cast<char*>(&readInto), sizeof(release));

        // return 1 if data cannot be read
        if (productFile.fail()) 
        {
           return 1;
        }

        // check if the current product record matches the filter
        if (strlen(filter.name) == 0 || strcmp(readInto.name, filter.name) == 0)
        {
           
           // update file access indexes
           previouslyAccessed[previouslyAccessedPosition] = fileIndex;
           previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;
           fileIndex++;
           return 0;
        }
        fileIndex++;// if no match, increment index and continue while loop
    }
    return 0;
}

/* function select:
    this function is implemented to take the index from the index parameter and select a product from
    the previously accessed ones based on the provided index using circular array calculations.
*/
bool Product::select(product& readInto,int index, int menuCount)
{
    if (index <= 0 || index > menuCount || index > 20 || index > fileIndex) 
    {
        return 1;
    }

    // find the index of the selected in the circular array
    int64_t position = previouslyAccessed[(previouslyAccessedPosition - index + 20) % 20];

    // seek and read the product
    productFile.seekg(position * sizeof(product), std::ios::beg);
    productFile.read(reinterpret_cast<char*>(&readInto), sizeof(product));

    // return 1 if data could not be read
    if (productFile.fail()) 
    {
        return 1;
    }

    previouslyAccessedPosition = 0;
    return 0;
}

/* function seekToBeginning:
    this function is implented to go back to the beggining of the product file
*/
bool Product::seekToBeginning()
{
    productFile.seekg(0, std::ios::beg); // go to the beginning of the product file
    fileIndex = 0; // update fileIndex to 0
    return 0;
}

//==================

/* function initRelease:
    this function is implemented to initialize the release database by opening the release file,
    calculate the number of releases currently in the database, and intitialize the utility
    variables to 0. Returns 0 on successfuly initiazation, returns 1 elsewise. 
*/
bool Release::initRelease()
{
    releaseFile.open(filename, std::ios::in | std::ios::out | std::ios::binary); //open release file
    if(!releaseFile.is_open()) // return 1 if unable to open release file
    {
        return 1;
    }
    // initialize releaseCount
    releaseFile.seekg(0, std::ios::end);
    releaseCount = releaseFile.tellg()/sizeof(release);
    releaseFile.seekg(0, std::ios::beg);

    fileIndex = 0; // initialize fileIndex
    previouslyAccessedPosition = 0; // initialize previouslyAccessedPosition
    memset(previouslyAccessed, 0, sizeof(previouslyAccessed));  // initialize previouslyAccessed to 0
    return 0;
}

/* function uninitRelease:
    this function is implemented to uninitialize the release database by closing the file,
    if it is open.
*/
bool Release::uninitRelease()
{
    if (releaseFile.is_open()) //check that the release file is open and close it if so
    {
        releaseFile.close();
        return 0;
    }
    else
    {
        return 1;
    }
}

/* function writeRelease:
    this function is implemented to write a new release to the end of the release file and
    update the release count if the release is successfully written.
*/
bool Release::writeRelease( release& readIn)
{
    releaseFile.seekp(0, std::ios::end); // go to end of file
    releaseFile.write(reinterpret_cast<char*>(&readIn), sizeof(release)); // add new release to the end of the file

    if(releaseFile.fail())
    {
        return 1;
    }
    releaseCount++; // increment releaseCount if added successfully
    return 0;
}

/* function getNext:
    this function is implented to read the next release in the file (if there are still more
    releases to read from) into readInto and update the file access pointers.
*/
bool Release::getNext(release& readInto)
{
    //if the release file isn't open or there are no releases return 1
    if (!releaseFile.is_open() || releaseCount == 0 ) {
        return 1;
    }
    releaseFile.seekg(fileIndex * sizeof(release), std::ios::beg); // go to the next release
    releaseFile.read(reinterpret_cast<char*>(&readInto), sizeof(release)); // read the next release

    // if data can't be read return 1
    if (releaseFile.fail()) {
        return 1;
    }

    // update file access indexes
    previouslyAccessed[previouslyAccessedPosition] = releaseFile.tellg(); 
    previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;

    return 0;
}

/* function getNext:
    this function is implemented to read the next release in the file (if there are still more
    releases to read from) into readInto. if the filter matches the current release being read, 
    update the file access pointers.
*/
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
        if ((strlen(filter.name) == 0 || strcmp(readInto.name, filter.name) == 0) &&
           (strlen(filter.date) == 0 || strcmp(readInto.date, filter.date) == 0) &&
           (filter.releaseId == 0 || readInto.releaseId == filter.releaseId)) 
        {
           
           // update file access indexes
           previouslyAccessed[previouslyAccessedPosition] = fileIndex;
           previouslyAccessedPosition = (previouslyAccessedPosition + 1) % 20;
           fileIndex++;
           return 0;
        }
        fileIndex++;// if no match, increment index and continue while loop
    }
    return 0;
}

/* function select:
    this function is implemented to take the index from the index parameter and select a release from
    the previously accessed ones based on the provided index using circular array calculations.
*/
bool Release::select(release& readInto,int index, int menuCount)
{
    if (index <= 0 || index > menuCount || index > 20 || index > fileIndex) 
    {
        return 1;
    }

    // find the index of the selected in the circular array
    int64_t position = previouslyAccessed[(previouslyAccessedPosition - index + 20) % 20];

    // seek and read the release
    releaseFile.seekg(position * sizeof(release), std::ios::beg);
    releaseFile.read(reinterpret_cast<char*>(&readInto), sizeof(release));

    // return 1 if data could not be read
    if (releaseFile.fail()) 
    {
        return 1;
    }

    previouslyAccessedPosition = 0;
    return 0;
}

/* function seekToBeginning:
    this function is implented to go back to the beggining of the product file
*/
bool Release::seekToBeginning()
{
    releaseFile.seekg(0, std::ios::beg); // go to the beggining of the release file
    fileIndex = 0; // update fileIndex to 0
    return 0;
}
