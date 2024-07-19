/* Product.h
description:
This module is for maintenance of products and product releases.
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

//==================

//struct for a product
typedef struct 
{
    char name[MAX_PRODUCT_NAME_SIZE] = "";     // name of product       
}product;


//struct for a release
typedef struct 
{
    char name[MAX_PRODUCT_NAME_SIZE] = "";     //name of product     
    char date[RELEASE_DATE_SIZE] = "";         //release fate of product
    char releaseId[ID_DIGITS] = "";                        // release id
}release;

//==================

class Product {
public:
    
    static bool initProduct();
    /* initializes the Product module
    precondition: none.
    postcondition: the Product module is initialized.
    exceptions raised: none.
    */

    static bool uninitProduct();
    /* cleans up the Product module to uninitialize.
    precondition: the Product module must already be initialized.
    postcondition: the Product module is uninitialized.
    exceptions raised: none.
    */

    static bool select(
        /* used to store the product read in by select
        used as output, mutates */
        product& readInto, 
        /* find and save the product accessed n times ago from the file.
        0 < n <= 20 */
        int index,
        /* the number of selections available in the menu printout
        used as input */
        int menuCount
    );
    /* description:
        seek and save a product from file previously retreived by getNext
    precondition:
        getNext has been called at least n times since the last call to select
    postcondition:
        cache of previously accessed product positions will be cleared.
        return to position 0 in the file.
    return
    */
    
    static bool writeProduct(
        /* description: the product object that is to be written by this function
        used for: input */
        product& product
    );
    /* writes a product object to the product file.
    precondition: the product file must be open in 'std::ios::out' mode for writability
    postcondition: the product object is written.
    exceptions raised: throws an exception if file is not open in 'std::ios::out' mode for writability
    */

    static bool getNext(
        /* description: a reference to the product object to be read
        used for: output */
        product& product
    );
    /* reads the next product object from the file. returns true, if read successfully.
    precondition: the product file must be open in 'std::ios::in' mode for readability
    postcondition: the next product object is read.
    exceptions raised: throws an exception if file is not open in 'std::ios::in' mode for readability
    */

   static bool getNext(
        /* used to store the product read in by getNext
        used as output, mutates */
        product& readInto, 
        /* used to filter, getNext will only get products matching the defined paramatres of the passed release
        used as input, does not mutate*/
        product& filter 
    );

    static bool seekToBeginning();
    /* postion in the file goes back to the beginning of the product file.
    precondition: the product file must already be open.
    postcondition: postion in the file goes back to the beginning of the product file.
    exceptions raised: throws an exception if the product file is not already be open. 
    */

private:
    static int64_t previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
    static int64_t previouslyAccessedPosition; // position in previously accessed

    static const char* filename; // name of product file
    static std::fstream productFile; // the file stream for the product file
    static int64_t fileIndex; // currently viewed element in database file
    static int64_t productCount; // this value is caculated at initialisation
};

//==================

class Release {
public:

    static bool initRelease();
    /* initializes the Product module
    precondition: none.
    postcondition: the Product module is initialized.
    exceptions raised: none.
    */

    static bool uninitRelease();
    /* cleans up the Product module to uninitialize.
    precondition: the Product module must already be initialized.
    postcondition: the Product module is uninitialized.
    exceptions raised: none.
    */

    static bool writeRelease(
        /* description: the release object that is to be written by this function
        used for: input */
        release& release
    );
    /* writes a release object to the release file.
    precondition: the release file must be open in 'std::ios::out' mode for writability
    postcondition: the release object is written.
    exceptions raised: throws an exception if file is not open in 'std::ios::out' mode for writability
    */

    static bool getNext(
        /* description: a reference to the release object to be read
        used for: output */
        release& release
    );
    /* reads the next release object from the file. returns true, if read successfully.
    precondition: the release file must be open in 'std::ios::in' mode for readability
    postcondition: the next release object is read.
    exceptions raised: throws an exception if file is not open in 'std::ios::in' mode for readability
    */

    static bool getNext(
        /* used to store the release read in by getNext
        used as output, mutates */
        release& readInto, 
        /* used to filter, getNext will only get release matching the defined paramatres of the passed release
        used as input, does not mutate*/
        release& filter 
    );

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
    return
    */    

    static bool seekToBeginning();
    /* postion in the file goes back to the beginning of the release file.
    precondition: the release file must already be open.
    postcondition: postion in the file goes back to the beginning of the product file.
    exceptions raised: throws an exception if the release file is not already be open. 
    */
    
private:
    static int64_t previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
    static int64_t previouslyAccessedPosition; // position in previously accessed

    static std::fstream releaseFile; // the file stream for the release file
    static const char* filename; // name of release file
    static int64_t fileIndex; // currently viewed element in database file
    static int64_t releaseCount; // this value is caculated at initialisation
};

