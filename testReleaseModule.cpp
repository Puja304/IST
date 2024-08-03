/*
TestReleaseModule.cpp
Unit Test 2: Testing the Product's Release Database
his is a bottom-up test driver that aims to test the overall functionality of our Product Module's Release Database.
The test calls each of its functions at least once and checks for two things:
    1. that it executes without errors (as symbolised by returning 0)
    2. Its execution did what it was expected to (verification)
For each test, we will print out a PASS or FAIL depending on the result
For this module : a function will return 0 if successfully executed and 1 otherwise
Since each test is a function , the module will be initialized at the beginning and uninitialized at the end for each (these two functions will be tested first)

version history:
ver1 -24/07/16, original by Puja Shah
*/

#include <iostream>
#include <cassert>
#include "Product.h"
#include <cstring>

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Helper functions to assist with populating and printing the result statement
*/

// Utility function to print test results
void printTestResult(const std::string& testName, bool result) {
    // since 1 is fail and 0 is pass
    std::cout << (result ? "FAIL: " : "PASS: ") << testName << std::endl;
}

// Initialize release struct with sample data
release createSampleRelease(const char* name, const char* date, const char* releaseId) {
    release sampleRelease;
    strncpy(sampleRelease.name, name, MAX_PRODUCT_NAME_SIZE);
    strncpy(sampleRelease.date, date, RELEASE_DATE_SIZE);
    strncpy(sampleRelease.releaseId, releaseId, MAX_RELEASE_ID_SIZE);
    return sampleRelease;
}


/* -----------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*
Test 1: initialisation
Description: calls the initialization function and looks at its return value
Successfull initialization will return false or 0.
Preconditions: none
Postcondtions: the function has been initialized
*/
void testInitRelease() {
    bool result = Release::initRelease();
    printTestResult("testInitRelease", result);
}


/*
Test 2: Uninitialisation
Description: calls the iunnitialization function and looks at its return value
Successfull initialization will return false or 0.
Preconditions: none
Postcondtions: the function has been uninitialized
*/
void testUninitRelease() {
    bool result = Release::uninitRelease();
    printTestResult("testUninitRelease", result);
}


/*
Test 3: Writing a release to the database
Description: Creates 2 release objects with differing release IDs and product names. 
Successfull writing will return false or 0.
Preconditions: none
Postcondtions: the values will have been added to the database
*/
void testWriteRelease() {
    Release::initRelease();
    release newRelease = createSampleRelease("Product 1", "2023-01-01", "R001");
    release newRelease2 = createSampleRelease("Product 2", "2023-01-01", "R002");
    bool result = Release::writeRelease(newRelease);
    printTestResult("testWriteRelease", result);
    Release::uninitRelease();
}


/*
Test 4:Testing the get next function
Description: Calls get next twice, each time printing the previously written values in order
Successfull get will return false or 0.
Preconditions: the database must be populated with atleast two releases
Postcondtions: the database remains unchanged
*/
void testGetNext() {
    Release::initRelease();
    release readRelease;
    release readRelease2;
    //ensuring it works twice
    bool result = (Release::getNext(readRelease) && Release::getNext(readRelease2));
    printTestResult("testGetNext", result);
    Release::uninitRelease();
}

/*
Test 5: Testing the get next function with filter
Description: Calls get next with filter on the release with Product 2
Successfull get will return false or 0.
Preconditions: the database must be populated with a release for Product 2
Postcondtions: the database remains unchanged
*/
void testGetNextWithFilter() {
    Release::initRelease();
    //create a filter product
    release filter = createSampleRelease("Product 2", "", "");
    //create a release to read it in
    release readRelease;
    //call get next with filter
    bool result = Release::getNext(readRelease, filter);
    
    //print if getNext() was successful
    printTestResult("testGetNextWithFilter", result);

    //see if the returned release matched the product name
    bool releaseMatch = (strncmp(readRelease.name, filter.name, MAX_PRODUCT_NAME_SIZE) == 0);
    printTestResult("test filtered Get Next - Verify release", !releaseMatch);

    // if it does not match, print out what it is returning
    if (!releaseMatch){
        std::cout << "Expected name: " << filter.name << std::endl;
        std::cout << "Actual Name: " << readRelease.name << std::endl;
    }

    Release::uninitRelease();
}


/*
Test 6: Testing the select function
Description: Calls get next with filter on the release with release id R002
Successfull select will return false or 0.
Preconditions: the database must be populated with a release for release id R002
Postcondtions: the database remains unchanged
*/
void testSelect() {

    Release::initRelease();


    //create a filter
    release filter = createSampleRelease("","","R002");

    //create a release to store value
    release readRelease;

    // call getNext with filter 
    Release::getNext(readRelease,filter);

    // Select the first release that matches
    // since we only have one entry that matches and are calling getNext() only once, both index and menuCount == 1 
    int index = 1;
    int menuCount = 1; // We called getNext once 

    
    // call select with these results and see if it was successful
    bool result = Release::select(readRelease, index, menuCount);
    printTestResult("testSelect", result);

    //temp copy of what we want
    release expectedRelease = createSampleRelease("Product 2", "2023-01-01", "R002");

    // Expected values for the selected release (third release)
    bool releaseMatch = (strncmp(readRelease.name, expectedRelease.name, MAX_PRODUCT_NAME_SIZE) == 0 &&
                        strncmp(readRelease.date, expectedRelease.date, RELEASE_DATE_SIZE) == 0 &&
                        strncmp(readRelease.releaseId, expectedRelease.releaseId, MAX_RELEASE_ID_SIZE) == 0);

    // see if the correct release has been returned
    printTestResult("testSelect - Verify Release", !releaseMatch);

    // if not, print the expected and actual values.
    if(!releaseMatch){
        std::cout << "Expected Name: " << expectedRelease.name << ", Date: " << expectedRelease.date << ", Release ID: " << expectedRelease.releaseId << std::endl;
        std::cout << "Actual Name: " << readRelease.name << ", Date: " << readRelease.date << ", Release ID: " << readRelease.releaseId << std::endl;
    }   
    
    Release::uninitRelease();
}


/*
Test 7: Testing seektobeginnig();
Description: moves pointer to the beginning
Successfull move will return false or 0.
Preconditions:none
Postcondtions: the database remains unchanged
*/
void testSeekToBeginning() {
    Release::initRelease();
    //populate the file with some values and call getNext() to move the pointer
    release third = createSampleRelease("Product 3", "2024-01-01", "R003");
    release fourth = createSampleRelease("Product 2", "2024-01-04", "R001");
    release readRelease;

    Release::writeRelease(third);
    Release::writeRelease(fourth);

    Release::getNext(readRelease);
    
    //call the function and see if the pointer moves successfully
    bool result = Release::seekToBeginning();
    printTestResult("testSeekToBeginning", result);

    // our first value was: 
    release firstRelease = createSampleRelease("Product 1", "2023-01-01", "R001");


    // once the pointer has been moved to the beginning, a get next should return this value
    // read in first value from beginnign
    Release::getNext(readRelease);

    //match to actual first value
    bool releaseMatch = (strncmp(firstRelease.name, readRelease.name, MAX_PRODUCT_NAME_SIZE) == 0 &&
                    strncmp(firstRelease.date, readRelease.date, RELEASE_DATE_SIZE) == 0 &&
                    strncmp(firstRelease.releaseId, readRelease.releaseId, MAX_RELEASE_ID_SIZE) == 0);

    // if it matches
    printTestResult("test Seek - Verify release", !releaseMatch);

    if(!releaseMatch){
        std::cout << "Expected Name: " << firstRelease.name << ", Date: " << firstRelease.date << ", Release ID: " << firstRelease.releaseId << std::endl;
        std::cout << "Actual Name: " << readRelease.name << ", Date: " << readRelease.date << ", Release ID: " << readRelease.releaseId << std::endl;
    }

    Release::uninitRelease();
}


/*
The main function that calls each function in order
//PRECONDITION : DATABASE IS EMPTY BEFORE THIS TEST IS RUN
*/
int main() {
    testInitRelease();
    testUninitRelease();
    testWriteRelease();
    testGetNext();
    testGetNextWithFilter();
    testSelect();
    testSeekToBeginning();
    return 0;
}
