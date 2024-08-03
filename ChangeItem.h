/* ChangeItem.h
description:
This is the module for maintenance of the change item objects.
version history:
ver4 -24/07/25 update by Nicolao
        -separation of the change item and request modules
ver3 -24/07/13, cleaning up errors by Nicolao
        -added fstream, and write function
            -necessary for interface, missing by error
        -constants moved to another module
ver2 -24/07/06, proposed and accepted rewrite by Nicolao
        -separation of classes and types
        -interface revisions 
ver1 -24/07/02, original by Wah Paw Hser and Puja Shah
*/

#ifndef CHANGE_H
#define CHANGE_H

//==================

#include <stdint.h>
#include <fstream>
#include "Constants.h"

//==================

enum ChangeItemStates{unreviewed = 1, reviewed = 2, inProgress = 4, done = 8, cancelled = 16}; // possible values for status
enum PriorityStates{lowest, low, middle, high, highest}; // possible values for priority

//==================

typedef struct
{
    int32_t id = -1;
    int8_t status = -1;
    int8_t priority = -1;
    char product[MAX_PRODUCT_NAME_SIZE] = "";
    char release[MAX_RELEASE_ID_SIZE] = "";
    char description[MAX_DESCRIPTION_SIZE] = "";
}change_item;

//==================

// class managing file interaction with change item database
// provides high level read and write with change_item objects
class ChangeItemDatabase
{
    public:
    static bool init();
    /* description:
        this function prepares the database for interaction.
    preconditions:
        the ChangeItemDatabase must currently be uninitialised.
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
        the ChangeItemDatabase must be currently initialised.
    postconditions:
        the user will not have access to the database.
    returns:
        return 0 on successful uninitialisation, return 1 on failure.
    */

    static bool writeElement(
        /* element to save to file
        used as input */
        change_item& readIn
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
        /* used to store the change item read in by getNext.
        used as output mutates */
        change_item& readInto
    );
    /* description:
        saves the next change item in the database to the change item referenced by "readInto".
    postconditions:
        position in file will increment by one.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool getNext(
        /* used to store the change item read in by getNext
        used as output, mutates */
        change_item& readInto, 
        /* used to filter, getNext will only get change items matching the defined paramatres of the passed change item
        used as input, does not mutate*/
        change_item& filter 
    );
    /* description:
        saves the next change item to the change item "readInto".
        will only get change items matching the paramatres of the change item referenced by "filter".
    postconditions:
        position in file will increase.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool select(
        /* used to store the change item read in by select
        used as output, mutates */
        change_item& readInto, 
        /* find and save the item accessed n times ago from the file.
        0 < n <= SIZE_OF_INDEX_CACHE
        used as input */
        int index,
        /* provide the number of items available for selection in menu printout
        used as input */
        int menuCount
    );
    /* description:
        seek and save an item from file previously retreived by getNext
    precondition:
        getNext has been called at least n times since the last call to select
    postcondition:
        cache of previously accessed item positions will be cleared.
        return to position 0 in the file.
    return 0 on successful read, return 1 on failure.
    */

    static bool seekToBeginning();
    /* description:
        seek to the beginning of the file.
    returns:
        return 0 on successful seek, return 1 on failure.
    */    

    static int getChangeItemCount();
    /* description:
        returns changeItemCount
    returns: number of change items in the database

    */
    private:

    // utilities for select
    static int64_t previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
    static int64_t previouslyAccessedPosition; // position in previously accessed

    // utilities for file interaction
    static const char* filename;
    static std::fstream itemData; // database elements
    static int64_t fileIndex; // currently viewed element in database file
    static int64_t changeItemCount; // this value is caculated at initialisation
};

#endif
