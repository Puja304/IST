/* Requester.h
description:
This is the module for interface of maintenance of requester objects in the requester 
database. 
version history:
ver3 -24/07/15, updated by Wah Paw Hser
    -declared the getRequesterCount() method
ver2 -24/07/14, proposed by Nicolao Baretto, rewrite by Wah Paw Hser
    -full revision of Requester.h interface
ver1 -24/07/02, original by Wah Paw Hser and Puja Shah

*/

//==================

#include <stdint.h>
#include <fstream>
#include "Constants.h"

//==================

typedef struct 
{
    char name[MAX_REQUESTER_NAME_SIZE] = "";     // name of requester
    char phone[PHONE_NUMBER_SIZE] = "";          // phone number of requester
    char email[MAX_EMAIL_SIZE] = "";             // email of requester
    char department[MAX_DEPARTMENT_SIZE] = "";   // department of requester
    int32_t requesterId = -1;         
}requester;

// class managing file interaction with requester database
// provides high level read and write with requester objects
class RequesterDatabase
{
    public:
    static bool init();
    /* description:
        initializes the requester database for interaction
    preconditions:
        the RequesterDatabase must currently be uninitialised.
    postconditions:
        the initialization will find the number of elements in the database.
        the initialization will give the user access to the database.
    returns:
        return 0 on successful initialization, return 1 on failure.
    */

    static bool uninit();
    /* description:
        uninitializes the requester database and cleans up
    preconditions:
        the RequesterDatabase must be currently initialized.
    postconditions:
        the user will not have access to the database.
    returns:
        return 0 on successful uninitialization, return 1 on failure.
    */

    static bool writeElement(
        /* the requester to be written to the file
        used as input*/
        requester& readIn
    );
    /* description:
        writes a requester to the database file
    preconditions:
        the requester is legal
    postconditions:
        requester count increments
        requester is saved to database
    returns:
        0 on successful write, 1 on failure
    */

    static bool getNext(
        /* used to store the requester read in by getNext.
        used as output, mutates */
        requester& readInto
    );
    /* description:
        saves the next requester in the database to the requester referenced by "readInto".
    postconditions:
        position in file will increment by one.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool getNext(
        /* used to store the requester read in by getNext
        used as output, mutates */
        requester& readInto, 
        /* used to filter, getNext will only get requesters matching the defined paramatres of the passed requester
        used as input, does not mutate*/
        requester& filter 
    );
    /* description:
        saves the next requester to the requester "readInto".
        will only get requesters matching the paramatres of the requester referenced by "filter".
    postconditions:
        position in file will increase.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool select(
        /* used to store the requester read in by select
        used as output, mutates */
        requester& readInto, 
        /* find and save the requester accessed n times ago from the file.
        0 < n <= 20 */
        int index,
        /* the number of selections available in the menu printout
        used as input */
        int menuCount
    );
    /* description:
        seek and save a requester from file previously retreived by getNext
    precondition:
        getNext has been called at least n times since the last call to select
    postcondition:
        cache of previously accessed requester positions will be cleared.
        return to position 0 in the file.
    returns:
        return 0 on successful select, return 1 on failure.
    */

    static bool seekToBeginning();
    /* description:
        seek to the beginning of the file.
    returns:
        return 0 on successful seek, return 1 on failure.
    */

    static int64_t getRequesterCount();
    /* description:
        returns the number of requesters in the database.
    returns:
        return the number of requesters in the database.
    */

    private:
    // utilities for select
    static int64_t previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
    static int64_t previouslyAccessedPosition; // position in previously accessed

    // utilities for file interaction
    static const char* filename; // name of requester file
    static std::fstream requesterData; // database elements
    static int64_t fileIndex; // currently viewed element in database file
    static int64_t requesterCount; // this value is caculated at initialisation
};
