/* ChangeRequest.h
description:
This is the module for maintenance of the change request objects
version history:
ver4 -24/07/25, update by Nicolao
        -seperation of change request and change item modules
ver3 -24/07/13, cleaning up errors by Nicolao
        -added fstream, and write function
            -necessary for interface, missing by error
        -constants moved to another module
ver2 -24/07/06, proposed and accepted rewrite by Nicolao
        -separation of classes and types
        -interface revisions 
ver1 -24/07/02, original by Wah Paw Hser and Puja Shah
*/

#ifndef CHANGE_REQUEST_H
#define CHANGE_REQUEST_H

//==================

#include <stdint.h>
#include <fstream>
#include "Constants.h"

//==================

typedef struct {
    int32_t changeItemId = -1;
    int16_t requesterId = -1;
    char requestDate[DATE_SIZE]= "";
    char release[MAX_RELEASE_ID_SIZE] = "";
}change_request;

//==================

// class managing file interaction with change request file
// provides high level read write with change_request objects to the file 
class ChangeRequestDatabase
{
public:
    static bool init();
    /* description:
        this function prepares the database for interaction.
    preconditions:
        the ChangeRequestDatabase must currently be uninitialised.
    postconditions:
        the initialsation will find the number of elements in the database.
        the initialisation will give the user access to the database.
    returns:
        return 0 on successful initialisation, return 1 on failure.
    */

    static bool uninit();
    /* description:
        this function closes access to the database.
        to be used in cleanup.
    preconditions:
        the ChangeRequestDatabase must be currently initialised.
    postconditions:
        the user will not have access to the database.
    returns:
        return 0 on successful uninitialisation, return 1 on failure.
    */

    static bool writeElement(
        /* element to save to file
        used as input */
        change_request& readIn
    );
    /* description:
        writes an element to database file
        can be used to update an exiting element or create a new element
    preconditions:
        element is legal
    postconditions:
        element count increments
        element is saved to database
    returns:
        0 on successfule write, 1 on failure
    */

    static bool getNext(
        /* used to store the change request read in by getNext.
        used as output mutates */
        change_request& readInto
    );
    /* description:
        saves the next change request in the database to the change request referenced by "readInto".
    postconditions:
        position in file will increment by one.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool getNext(
        /* used to store the change request read in by getNext
        used as output, mutates */
        change_request& readInto, 
        /* used to filter, getNext will only get change requests matching the defined paramatres of the passed change request
        used as input, does not mutate*/
        change_request& filter 
    );
    /* description:
        saves the next change request to the change request "readInto".
        will only get change requests matching the paramatres of the change request referenced by "filter".
    postconditions:
        position in file will increase.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool select(
        /* used to store the change reqeust read in by select.
        used as output, mutates */
        change_request& readInto, 
        /* find and save the request accessed n times ago from the file.
        0 < n <= 20 */
        int index,
        /* provide the number of requests available for selection in menu printout
        used as input */
        int menuCount
    );
    /* description:
        seek and save a request from file previously retreived by getNext.
    precondition:
        getNext has been called at least n times since the last call to select.
    postcondition:
        cache of previously accessed request positions will be cleared.
        return to position 0 in the file.
    return 0 on successful read, return 1 on failure.
    */

    static bool seekToBeginning();
    /* description:
        seek to the beginning of the file.
    */    

private:
    // utilities for select
    static int64_t previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
    static int64_t previouslyAccessedPosition; // position in previously accessed

    // utilities for file interaction
    static const char* filename;
    static std::fstream requestData; // database elements
    static int64_t changeRequestCount; // this value is determined at initialisation
};

#endif
