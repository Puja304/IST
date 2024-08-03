/* TestFileOps.cpp
description:
This is a bottom-up test driver that aims to test the functionality of reading from and writing to our Requester Database, accessed using our Requester module functions
The test tests multiple functions and returns a Pass/ Fail verdict based on whether or not they function as they are intended to. 
version history:
ver1 -24/07/16, original by Puja Shah
*/



/*
Unit Test 1: Reading form and Writing to the Requester Module Database
We aim to test the main reading and writing tests in our Requester.cpp module
We are going to assume the initializer works correctly/
For each test, we will print out a PASS or FAIL depending on the result
For this module : a function will return 0 if successfully executed and 1 otherwise
If a test fails, there will be a message showing it and the program will end. This is due to the interconnected nature of tests
*/

   
   
    #include "Requester.h"
    #include <iostream>
    #include <cstring>
/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Helper functions to assist with populating and printing the result statement:*/

requester createRequester(int id, const char* name, const char* phone, const char* email, const char* department) {

    //Create a requester object
    requester req;

    //assign values:
    req.requesterId = id;
    strncpy(req.name, name, sizeof(req.name));
    strncpy(req.phone, phone, sizeof(req.phone));
    strncpy(req.email, email, sizeof(req.email));
    strncpy(req.department, department, sizeof(req.department));

    //return object
    return req;
} 

/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



void unitTest() {

    // Create sample requesters
    requester req1 = createRequester(1, "John Doe", "1234567890", "john@example.com", "HR");
    requester req2 = createRequester(2, "Jane Smith", "0987654321", "jane@example.com", "Finance");

    /*
    Test 1 : Testing to see if initialization is successful
    Preconditions: The Database is not yet initialized
    Postcondition: The database is not initialized  (The file is open)
    */
    if (RequesterDatabase::init()) {
        std::cout << "Initialization Failed" << std::endl;
        std::cout << "Fail" << std::endl;
        return;
    } 

    /*
    Test 2: Write requesters to file
    Preoconditions: the requesters are have valid sizes
    Postcondition: The requesters remain unchanged and the file now has fixed length binary records with their information
    */
    if (RequesterDatabase::writeElement(req1) || RequesterDatabase::writeElement(req2)) {
        std::cout << "Write Failed" << std::endl;
        std::cout << "Fail" << std::endl;
        return;
    }


    /*
    Test 3: Reading and verifying the data
    Preconditions: Requesters are written to the file successfully
    Postcondition: Data read matches the data written

    testing for all of the following:
        1. Reading an entry
        2. Matching the read entry to the entry we wrote (component wise):
            - requester id
            - requester name
            - requester phone number
            - requester email
            - reqester department
    */


    // Read requesters from the file and check data for requester 1
    requester readReq; 
    if (RequesterDatabase::getNext(readReq) || 
        readReq.requesterId != req1.requesterId || 
        strcmp(readReq.name, req1.name) != 0 ||
        strcmp(readReq.phone, req1.phone) != 0 ||
        strcmp(readReq.email, req1.email) != 0 ||
        strcmp(readReq.department, req1.department) != 0) {
        std::cout << "Read/Verification Failed for Requester 1" << std::endl;
        std::cout << "Fail" << std::endl;
        return;
    }


// Read requesters from the file and check data for requester 1

    if (RequesterDatabase::getNext(readReq) || 
        readReq.requesterId != req2.requesterId || 
        strcmp(readReq.name, req2.name) != 0 ||
        strcmp(readReq.phone, req2.phone) != 0 ||
        strcmp(readReq.email, req2.email) != 0 ||
        strcmp(readReq.department, req2.department) != 0) {
        std::cout << "Read/Verification Failed for Requester 2" << std::endl;
        std::cout << "Fail" << std::endl;
        return;
    }


    // Test reading beyond end of file
    /*
    Test 4: Reading beyond end of file
    Preconditions: All entries in the file have been read
    Postcondition: Read operation correctly stops at end of file

    Since there are exactly 2 entreis, a getNext() call should not be successful. A successfull read returns a 0, that should lead to an error message. This function should ideally return 
    true when the reading fails.
    */
    // Test reading beyond end of file
    if (!RequesterDatabase::getNext(readReq)) {
        std::cout << "Read did not stop at end of file" << std::endl;
        std::cout << "Fail" << std::endl;
        return;
    }


    // Uninitialize database
    /*
    Test 5: Testing uninitialization
    Preconditions: Database is initialized
    Postcondition: Database is successfully uninitialized
    */
    if (RequesterDatabase::uninit()) {
        std::cout << "Uninitialization Failed" << std::endl;
        std::cout << "Fail" << std::endl;
        return;
    }

    // if all the tests have been passed, it is an overall PASS.
    std::cout << "Pass" << std::endl;
}

int main() {
    unitTest();
    return 0;
}
