/* Product.h
description:
This module is for maintenance of products.
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
    char name[MAX_PRODUCT_NAME_SIZE] = "";  // name of product       
}product;

//==================

// class managing file interaction with change product database
// provides high level read and write with product objects
class Product {
public:
    
    static bool initProduct();
    /* description:
        initializes the Product module
    precondition: 
        none.
    postcondition: 
        the Product module is initialized.
    returns:
        0 on successful initialization, 1 on failure
    */

    static bool uninitProduct();
    /* description:
        cleans up the Product module to uninitialize.
    precondition: 
        the Product module must already be initialized.
    postcondition: 
        the Product module is uninitialized.
    returns:
        return 0 on successful uninitialization, return 1 on failure.
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
    returns:
        0 on successful select, 1 on failure
    */
    
    static bool writeProduct(
        /* element to save to file
            used as input */
        product& product
    );
    /* description: 
        writes a product object to the product file.
    precondition: 
        the product file must be open in 'std::ios::out' mode for writability
    postcondition: 
        the product object is written.
    returns:
        0 on successful write, 1 on failure
    */

    static bool getNext(
        /* used to store the product read in by getNext
        used as output, mutates */
        product& readInto
    );
    /*  description: 
        saves the next product in the database to the product referenced by "readInto".
    precondition: 
        the product file must be open in 'std::ios::in' mode for readability
    postcondition: 
        position in file will increment by one.
    returns:
        return 0 on successful read, return 1 on failure.
    */

   static bool getNext(
        /* used to store the product read in by getNext
        used as output, mutates */
        product& readInto, 
        /* used to filter, getNext will only get products matching the defined paramatres of the passed product
        used as input, does not mutate*/
        product& filter 
    );
    /* description:
        saves the next product to the product "readInto".
        will only get products matching the paramatres of the product referenced by "filter".
    postconditions:
        position in file will increase.
    returns:
        return 0 on successful read, return 1 on failure.
    */

    static bool seekToBeginning();
    /* description:
        postiion in the file goes back to the beginning of the product file.
    precondition: 
        the product file must already be open.
    postcondition: 
        postion in the file goes back to the beginning of the product file.
    returns:
        return 0 on successful seek, return 1 on failure.
    */

private:
    // utilities for select
    static int64_t previouslyAccessed[20]; // records the indexes of the previous elements accessed, circular array
    static int64_t previouslyAccessedPosition; // position in previously accessed

    // utilities for file interaction
    static const char* filename; // name of product file
    static std::fstream productFile; // the file stream for the product file
    static int64_t fileIndex; // currently viewed element in database file
    static int64_t productCount; // this value is caculated at initialisation
};
