/* Release.h
description:
This module is for maintenance of product releases.
version history:
ver2 -24/07/14, proposed by Nicolao Baretto, rewrite by Allan Hu
    -full revision of product.h interface
ver1 -24/07/02, original by Wah Paw Hser and Puja Shah
*/

//==================

#include <iostream>
#include <fstream>
#include <stdint.h>
#include "Constants.h"

//struct for a release
typedef struct 
{
    char name[MAX_PRODUCT_NAME_SIZE] = "";  //name of product     
    char date[RELEASE_DATE_SIZE] = "";      //release fate of product
    char releaseId[ID_DIGITS] = "";         // release id
}release;

// class managing file interaction with release database
// provides high level read and write with release objects
class Release {
public:

    static bool initRelease();
    /* description:
        initializes the release module
    precondition: 
        none.
    postcondition: 
        the release module is initialized.
    returns:
        return 0 on successful initialization, return 1 on failure.
    */

    static bool uninitRelease();
    /* description:
        cleans up the release module to uninitialize.
    precondition: 
        the release module must already be initialized.
    postcondition: 
        the release module is uninitialized.
    returns:
        return 0 on successful uninitialization, return 1 on failure.
    */

    static bool writeRelease(
        /* element to save to file
            used as input */
        release& release
    );
    /* description:
        writes a release object to the release file.
    precondition: 
        the release file must be open in 'std::ios::out' mode for writability
    postcondition: 
        the release object is written.
    returns:
        0 on successful write, 1 on failure
    */

    static bool getNext(
        /* used to store the release read in by getNext
        used as output, mutates */
        release& release
    );
    /* description: 
        saves the next release in the database to the release referenced by "readInto".
    precondition: 
        the release file must be open in 'std::ios::in' mode for readability
    postcondition: 
        the next release object is read.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool getNext(
        /* used to store the release read in by getNext
        used as output, mutates */
        release& readInto, 
        /* used to filter, getNext will only get releases matching the defined paramatres of the passed release
        used as input, does not mutate*/
        release& filter 
    );
    /* description:
        saves the next release to the release "readInto".
        will only get release matching the paramatres of the release referenced by "filter".
    postconditions:
        position in file will increase.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool select(
        /* used to store the release read in by select
        used as output, mutates */
        release& readInto, 
        /* find and save the release accessed n times ago from the file.
        0 < n <= 20 */
        int index,
        /* the number of selections available in the menu printout
        used as input */
        int menuCount
    );
    /* description:
        seek and save a release from file previously retreived by getNext
    precondition:
        getNext has been called at least n times since the last call to select
    postcondition:
        cache of previously accessed release positions will be cleared.
        return to position 0 in the file.
    returns:
        return 0 on successful select, return 1 on failure.
    */    

    static bool seekToBeginning();
    /* description:
        postion in the file goes back to the beginning of the release file.
    precondition: 
        the release file must already be open.
    postcondition: 
        postion in the file goes back to the beginning of the release file.
    returns:
        return 0 on successful seek, return 1 on failure.
    */
    
private:
    static int64_t previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
    static int64_t previouslyAccessedPosition; // position in previously accessed

    static std::fstream releaseFile; // the file stream for the release file
    static const char* filename; // name of release file
    static int64_t fileIndex; // currently viewed element in database file
    static int64_t releaseCount; // this value is caculated at initialisation
};

