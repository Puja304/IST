/* Product.h
description:
This module is for maintenance of products and product releases.
version history:
ver1 -24/07/02, original by Wah Paw Hser and Puja Shah
*/

//==================

#include <iostream>
#include <fstream>

//==================

const int MAX_PRODUCT_NAME_SIZE = 10;   // max length for product name
const int MAX_RELEASE_ID_SIZE = 8;      // max length for release ID
const int RELEASE_DATE_SIZE = 10;       // length of release date

//==================

class Product {
public:
    Product(
        /* description: name of the product
        used for: input */
        const char* name
    );
    /* constructs a Product object using the given name.
    precondition: 'name' must be 1-10 chars.
    postcondition: a Product object is successfully created.
    exceptions raised: throws an exception if 'name' is not 1-10 chars.
    */

    ~Product();
    /* destructor for Product objects.
    precondition: none.
    postcondition: the Product resources are cleaned up.
    exceptions raised: none.
    */
    
    //==================
    
    void initProduct();
    /* initializes the Product module
    precondition: none.
    postcondition: the Product module is initialized.
    exceptions raised: none.
    */

    void uninitProduct();
    /* cleans up the Product module to uninitialize.
    precondition: the Product module must already be initialized.
    postcondition: the Product module is uninitialized.
    exceptions raised: none.
    */

    //==================

    const char* getProduct() const;
    /* returns the name of the product.
    preconditions: none.
    postconditions: the product remains unchanged.
    */
    
    void showProduct() const;
    /* shows the details of the product to the user.
    preconditions: none.
    postconditions: the details of the product are shown to the user.
    */
    
    //==================

    static void openProductFile(
        /* description: the mode that the file is opened in
        (std::ios::in for writing) and (std::ios::out for reading)
        used for: input */
        std::ios_base::openmode mode
    );
    /* this function opens the product file in the specified 'mode'.
    precondition: none.
    postcondition: the product file is opened in the specified 'mode'.
    exceptions raised: throws an exception if the file does not open.
    */
    
    static void writeProduct(
        /* description: the product object that is to be written by this function
        used for: input */
        const Product& product
    );
    /* writes a product object to the product file.
    precondition: the product file must be open in 'std::ios::out' mode for writability
    postcondition: the product object is written.
    exceptions raised: throws an exception if file is not open in 'std::ios::out' mode for writability
    */

    static bool getNext(
        /* description: a reference to the product object to be read
        used for: output */
        Product& product
    );
    /* reads the next product object from the file. returns true, if read successfully.
    precondition: the product file must be open in 'std::ios::in' mode for readability
    postcondition: the next product object is read.
    exceptions raised: throws an exception if file is not open in 'std::ios::in' mode for readability
    */

    static void closeProductFile();
    /* this function closes the product file.
    precondition: the product file must already be open.
    postcondition: the product file is closed,
    exceptions raised: throws an exception if the product file is not already be open.
    */

    static void seekToBeginningOfFile();
    /* postion in the file goes back to the beginning of the product file.
    precondition: the product file must already be open.
    postcondition: postion in the file goes back to the beginning of the product file.
    exceptions raised: throws an exception if the product file is not already be open. 
    */

private:
    char name[MAX_PRODUCT_NAME_SIZE]; // name of product

    static std::fstream productFile;      // the file stream for the Requester file
};

//==================

class Release {
public:
    Release(
        /* description: name of the product
        used for: input */
        const char* name,
        /* description: release ID of the product
        used for: input */
        const char* releaseId,
        /* description: date of the release
        used for: input */
        const char* date
    );
    /* constructs a Release object using the given name, release id, and date.
    preconditions: 'name' must be 1-10 chars.
                   'releaseId' must be 1-8 chars.
                   'date' must be in the format 'YYYY-MM-DD' (10 chars)
    postcondition: a Release object is successfully created.
    exceptions raised: throws an exception if 'name', 'releaseId, or 'date' do
                       not meet the required format.
    */

    ~Release();
    /* destructor for Product objects.
    precondition: none.
    postcondition: the Product resources are cleaned up.
    exceptions raised: none.
    */
    
    //==================
    
    void initRelease();
    /* initializes the Product module
    precondition: none.
    postcondition: the Product module is initialized.
    exceptions raised: none.
    */

    void uninitRelease();
    /* cleans up the Product module to uninitialize.
    precondition: the Product module must already be initialized.
    postcondition: the Product module is uninitialized.
    exceptions raised: none.
    */

    //==================
    
    const char* getName() const;
    /* returns the name of the product.
    preconditions: none.
    postconditions: the product name remains unchanged.
    */   
    
    const char* getReleaseID() const;
    /* returns the release ID.
    preconditions: none.
    postconditions: the release ID remains unchanged.
    */   
    
    const char* getReleaseDate() const;
    /* returns the date of release.
    preconditions: none.
    postconditions: the release date remains unchanged.
    */ 
  
    void showRelease() const;
    /* shows the details of the product release to the user.
    preconditions: none.
    postconditions: the details of the product release are shown to the user.
    */

    //==================
    
    static void openReleaseFile(
        /* description: the mode that the file is opened in
        (std::ios::in for writing) and (std::ios::out for reading)
        used for: input */
        std::ios_base::openmode mode
    );
    /* this function opens the release file in the specified 'mode'.
    precondition: none.
    postcondition: the release file is opened in the specified 'mode'.
    exceptions raised: throws an exception if the file does not open.
    */
    
    static void writeRelease(
        /* description: the release object that is to be written by this function
        used for: input */
        const Release& release
    );
    /* writes a release object to the release file.
    precondition: the release file must be open in 'std::ios::out' mode for writability
    postcondition: the release object is written.
    exceptions raised: throws an exception if file is not open in 'std::ios::out' mode for writability
    */

    static bool getNext(
        /* description: a reference to the release object to be read
        used for: output */
        Release& release
    );
    /* reads the next release object from the file. returns true, if read successfully.
    precondition: the release file must be open in 'std::ios::in' mode for readability
    postcondition: the next release object is read.
    exceptions raised: throws an exception if file is not open in 'std::ios::in' mode for readability
    */

    static void closeReleaseFile();
    /* this function closes the release file.
    precondition: the release file must already be open.
    postcondition: the release file is closed,
    exceptions raised: throws an exception if the release file is not already be open.
    */

    static void seekToBeginningOfFile();
    /* postion in the file goes back to the beginning of the release file.
    precondition: the release file must already be open.
    postcondition: postion in the file goes back to the beginning of the product file.
    exceptions raised: throws an exception if the release file is not already be open. 
    */

private:
    char name[MAX_PRODUCT_NAME_SIZE];       // name of product
    char releaseId[MAX_RELEASE_ID_SIZE];    // release ID of product
    char date[RELEASE_DATE_SIZE];           // release date 

    static std::fstream productFile;      // the file stream for the Requester file
};

//==================

Product addProduct(
    /* description: name of the product
    used for: input */
    const char* name
);
/* creates a Product object using the given name and returns the Product.
precondition: 'name' must be 1-10 chars.
postcondition: a Product object is successfully created.
exceptions raised: throws an exception if 'name' is not 1-10 chars.
*/

Release addRelease(
    /* description: name of the product
    used for: input */
    char* product,
    /* description: release ID of the product
    used for: input */
    char* releaseId,
    /* description: the date of the release
    used for: input */
    char* date
);
/* creates a Release object using the given name, release id, and date and 
returns the Release.
preconditions: 'name' must be 1-10 chars.
               'releaseId' must be 1-8 chars.
               'date' must be in the format 'YYYY-MM-DD' (10 chars)
postcondition: a Release object is successfully created.
exceptions raised: throws an exception if 'name', 'releaseId, or 'date' do
                   not meet the required format.
*/
