/* Product.cpp
description:
This is the implementation of the Product module
version history:
ver2 -24/07/28 update by Nicolao and Allan Hu
     -fixed select function and filtered getnext behaviour
     -init now creates file when none exists
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
int64_t Product::previouslyAccessed[20] = {0};  // records the indexes of the previous elements accessed, curcular array
int64_t Product::previouslyAccessedPosition = 0;  // position in previously accessed

const char* Product::filename = "Product.dat";
std::fstream Product::productFile; // product elements
int64_t Product::fileIndex = 0;  // currently viewed element in product file
int64_t Product::productCount = 0;  // amount of products

//==================

//creates product file if needed
//opens product file to allow reads and writes
bool Product::initProduct()
{
    // open product file 
    productFile.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    // if file not found create file
    if (!productFile.is_open()) 
    {
        productFile.clear();
        std::ofstream createFile(filename, std::ios::out | std::ios::binary);
        createFile.close();
        productFile.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }

    // if file still cannot be opened fail to initialise
    if (!productFile.is_open()) 
    {
        return 1;
    }

    // initialize productCount
    productFile.seekg(0, std::ios::end);
    productCount = productFile.tellg()/sizeof(product);
    productFile.seekg(0, std::ios::beg);

    // initialize fileIndex to 0
    fileIndex = 0; 
    //initialize previouslyAccessedPosition to 0
    previouslyAccessedPosition = 0; 
    // initialize previouslyAccessed to 0
    memset(previouslyAccessed, 0, sizeof(previouslyAccessed));  

    return 0;
}

//==================

bool Product::uninitProduct()
{
    // check the product file is open and if so close it 
    if (productFile.is_open())
    {
        productFile.close();
        return 0;
    }
    else
    {
        return 1;
    }
}

//==================

bool Product::writeProduct( product& readIn)
{
    // go to end of file
    productFile.seekp(0, std::ios::end); 
    // add new product to end of file
    productFile.write(reinterpret_cast<char*>(&readIn), sizeof(product)); 

    //return 1 if unable to write to product file
    if(productFile.fail())
    {
        return 1;
    }

    // increment productCount if added successfully
    productCount++; 
    return 0;
}

//==================

bool Product::getNext(product& readInto)
{
    // read next product from file
    productFile.seekg(fileIndex * sizeof(product), std::ios::beg);
    productFile.read(reinterpret_cast<char*>(&readInto), sizeof(product));

    // return 1 if data cannot be read
    if (productFile.fail()) 
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

bool Product::getNext(product& readInto, product& filter )
{
    while (fileIndex < productCount) 
    {
        // read next product from file
        productFile.seekg(fileIndex * sizeof(product), std::ios::beg);
        productFile.read(reinterpret_cast<char*>(&readInto), sizeof(product));

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
        // if no match, increment index and continue while loop
        fileIndex++;
    }
    return 1;
}

//==================

bool Product::select(product& readInto, int index, int menuCount)
{
    // error check the index, return 1 if not legal
    if ((index <= 0) || (index > menuCount) || (index > 20)) 
    {
        return 1;
    }

    // return 1 if product file not open
    if (productFile.fail()) 
    {
        return 1;
    }

    // find the index of the selected in the circular array
    int64_t position = (previouslyAccessedPosition -1 + index - menuCount) % SIZE_OF_ACCESSED_INDEX_CACHE;
    // seek and read the product
    productFile.seekg(previouslyAccessed[position] * sizeof(product));
    productFile.read(reinterpret_cast<char*>(&readInto), sizeof(product));

    previouslyAccessedPosition = 0;
    return 0;
}

//==================

bool Product::seekToBeginning()
{
    //return 1 if product file is not open
    if (!productFile.is_open())
    {
        return 1;
    }

    // go to the beginning of the product file
    productFile.clear();
    productFile.seekg(0, std::ios::beg);
    // update fileIndex to 0
    fileIndex = 0; // 
    return 0;
}
