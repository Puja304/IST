/* Change.h
description:
This is the module for maintenance of the change request and change item objects.
version history:
ver1 -24/07/02, original by Wah Paw Hser and Puja Shah
*/

//==================

#include <iostream>
#include <fstream>

//==================

const int MAX_RELEASE_ID_SIZE = 8;      // max length for release ID
const int MAX_PRODUCT_NAME_SIZE = 10;   // max length for product name
const int DATE_SIZE = 10;               // length of date of request
const int MAX_DESCRIPTION_SIZE = 30;   // max length for description

//==================

class Request {
public:
    Request(
        /* description: release ID
        used for: input */
        const char* release,
        /* description: date of the request
        used for: input */
        const char* date,
    );
    /* constructs a Request object using the given releaseId and date.
    precondition: 'releaseId' must be 1-8 chars.
                  'date' must be in format 'YYYY-MM-DD' (10 chars).
    postcondition: a Request object is successfully created.
    exceptions raised: throws an exception if 'releaseId' or 'date' do not meet
                       the correct format.
    */

    ~Request();
    /* destructor for Request objects.
    precondition: none.
    postcondition: the Request resources are cleaned up.
    exceptions raised: none.
    */ 
    
    //==================
    
    void initRequest();
    /* initializes the Change module
    precondition: none.
    postcondition: the Change module is initialized.
    exceptions raised: none.
    */

    void uninitRequest();
    /* cleans up the Change module to uninitialize.
    precondition: the Change module must already be initialized.
    postcondition: the Change module is uninitialized.
    exceptions raised: none.
    */

    //==================
  
    const char* getRelease() const;
    /* returns the release id of the product.
       precondition: none.
       postcondition: the release id should remain unchanged.
    */
    
    const char* getDate() const;
    /* returns the date of request.
       precondition: none.
       postcondition: the date should renain unchanged.
    */
    
    const int getRequesterID() const;
    /* returns the id of the requester.
       precondition: none.
       postcondition: the requester id should remain unchanged.
    */
    
    const int getChangeID() const;
    /* returns the change id associated with the request.
       precondition: none.
       postcondition: the change id should remain unchanged.
    */
    
    void showRequest();
    /* shows the details of the change request to the user.
       precondition: none.
       postcondition: the details of change request are shown to the user.
    */

   //==================

    static void openRequestFile(
        /* description: the mode that the file is opened in
        (std::ios::in for writing) and (std::ios::out for reading)
        used for: input */
        std::ios_base::openmode mode
    );
    /* this function opens the request file in the specified 'mode'.
    precondition: none.
    postcondition: the request file is opened in the specified 'mode'.
    exceptions raised: throws an exception if the file does not open.
    */
    
    static void writeRequest(
        /* description: the request object that is to be written by this function
        used for: input */
        const Request& request
    );
    /* writes an request object to the request file.
    precondition: the request file must be open in 'std::ios::out' mode for writability
    postcondition: the request object is written.
    exceptions raised: throws an exception if file is not open in 'std::ios::out' mode for writability
    */

    static bool getNext(
        /* description: a reference to the request object to be read
        used for: output */
        Request& request
    );
    /* reads the next request object from the file. returns true, if read successfully.
    precondition: the request file must be open in 'std::ios::in' mode for readability
    postcondition: the next request object is read.
    exceptions raised: throws an exception if file is not open in 'std::ios::in' mode for readability
    */

    static void closeRequestFile();
    /* this function closes the request file.
    precondition: the request file must already be open.
    postcondition: the request file is closed,
    exceptions raised: throws an exception if the request file is not already be open.
    */

    static void seekToBeginningOfFile();
    /* postion in the file goes back to the beginning of the request file.
    precondition: the request file must already be open.
    postcondition: postion in the file goes back to the beginning of the request file.
    exceptions raised: throws an exception if the request file is not already be open.
    */

private:
    char release[MAX_RELEASE_ID_SIZE];  // release id of product
    char date[DATE_SIZE];               // date of request
    int requesterId;                    // requester ID
    int changeId;                       // change ID
    static int nextChangeId;            // the next change ID to be assigned
    static int nextRequesterId;         // the next requester ID to be assigned

    static std::fstream requestFile;      // the file stream for the Requester file
};

//==================

class Item {
public:
    Item(
        /* description: product name
        used for: input */
        const char* product,
        /* description: release ID
        used for: input */
        const char* release,
        /* description: description of the change item
        used for: input */
        const char* description,
        /* description: priority of the change item
        used for: input */
        int priority
    );
    /* constructs an Item object using the given product, release, description
    and priority. Will generate a change id and set the status ro unreviewed.
    precondition: 'product' must be 1-10 chars.
                  'release' must be 1-8 chars.
                  'description' must be 1-30 chars.
                  'priority' must be an int (1-5).
    postcondition: an Item object is successfully created.
    exceptions raised: throws an exception if 'relname', 'release', description'
                       or 'priority' do not meet the correct format.
    */

    ~Item();
    /* destructor for Item objects.
    precondition: none.
    postcondition: the Item resources are cleaned up.
    exceptions raised: none.
    */
    
    //==================
    
    void initItem();
    /* initializes the Change module
    precondition: none.
    postcondition: the Change module is initialized.
    exceptions raised: none.
    */

    void uninitItem();
    /* cleans up the Change module to uninitialize.
    precondition: the Change module must already be initialized.
    postcondition: the Change module is uninitialized.
    exceptions raised: none.
    */
    
    //==================
    
    void updStatus(
        /* description: the new status of the change item
        used for: input */
        int newStatus
    );
    /* updates the status of the change item
    precondition: Prior status cannot be 'Done' or 'Cancelled' (ints 3 or 4).
                  'newStatus' must be a valid status.
    postcondition: the status of the change item is successfully updated.
    exceptions raised: throws an exception if the user tries to update a status
                       out of a 'Done' or 'Cancelled' state (ints 3 or 4).
    */

    void updRelease(
        /* description: the new release ID of the change item
        used for: input */
        char* newRelease
    );
    /* updates the release ID of the change item
    precondition: 'newRelease' must be 1-8 chars.
    postcondition: the release ID of the change item is successfully updated.
    exceptions raised: throws an exception if 'newRelease' is not 1-8 chars.
    */
    
    void updPriority(
        /* description: the new priority of the change item
        used for: input */
        int newPriority
    );
    /* updates the priority of the change item
    precondition: 'newPriority' must be an int (1-5).
    postcondition: the priority of the change item is successfully updated.
    exceptions raised: throws an exception if 'newPriority' is not an int (1-5).
    */
    
    void updDesc(
        /* description: the new description of the change item
        used for: input */
        char* newDescription
    );
    /* updates the description of the change item
    precondition: 'newDescription' must be an 1-30 chars.
    postcondition: the description of the change item is successfully updated.
    exceptions raised: throws an exception if 'newDescription' is not 1-30 chars.
    */ 
    
    //==================

    const char* getProduct() const;
    /* returns the product name.
       precondition: none.
       postcondition: the name should remain unchanged.
    */
    
     const char* getRelease() const;
    /* returns the release ID of the product.
       precondition: none.
       postcondition: the release should remain unchanged.
    */
    
     const char* getDescription() const;
    /* returns the change item description.
       precondition: none.
       postcondition: the description must remain unchanged.
    */
    
    const int getStatus() const;
    /* returns the change item's status.
       precondition: none.
       postcondition: the status remains unchanged.
    */
    
     const int getPriority() const;
    /* returns the change item's priority.
       precondition: none.
       postcondition: the priority remains unchanged.
    */
    
     const int getChangeID() const;
    /* returns the change id.
       precondition: none.
       postcondition: the change id remains unchanged.
    */
    
    void showItem();
    /* shows the details of the change item to the user.
    precondition: none.
    postcondition: the details of the change item are shown to the user.
    */
   
    //==================

    static void openItemFile(
        /* description: the mode that the file is opened in
        (std::ios::in for writing) and (std::ios::out for reading)
        used for: input */
        std::ios_base::openmode mode
    );
    /* this function opens the item file in the specified 'mode'.
    precondition: none.
    postcondition: the item file is opened in the specified 'mode'.
    exceptions raised: throws an exception if the file does not open.
    */
    
    static void writeItem(
        /* description: the item object that is to be written by this function
        used for: input */
        const Item& item
    );
    /* writes an item object to the item file.
    precondition: the item file must be open in 'std::ios::out' mode for writability
    postcondition: the item object is written.
    exceptions raised: throws an exception if file is not open in 'std::ios::out' mode for writability
    */

    static bool getNext(
        /* description: a reference to the item object to be read
        used for: output */
        Item& item
    );
    /* reads the next item object from the file. returns true, if read successfully.
    precondition: the item file must be open in 'std::ios::in' mode for readability
    postcondition: the next item object is read.
    exceptions raised: throws an exception if file is not open in 'std::ios::in' mode for readability
    */

    static void closeItemFile();
    /* this function closes the item file.
    precondition: the item file must already be open.
    postcondition: the item file is closed,
    exceptions raised: throws an exception if the item file is not already be open.
    */

    static void seekToBeginningOfFile();
    /* postion in the file goes back to the beginning of the item file.
    precondition: the item file must already be open.
    postcondition: postion in the file goes back to the beginning of the item file.
    exceptions raised: throws an exception if the item file is not already be open.
    */
  
private:
    char product[MAX_PRODUCT_NAME_SIZE];    // product name
    char release[MAX_RELEASE_ID_SIZE];      // release ID of product
    char description[MAX_DESCRIPTION_SIZE]; // description of change item
    int status;                             // status of change item
    int priority;                           // priority of change item
    int changeId;                           // change ID
    static int nextChangeId;                // the next change ID to be assigned

    static std::fstream itemFile;           // the file stream for the Item file
};

//==================

Request addRequest(
    /* description: release ID
    used for: input */
    const char* release,
    /* description: date of the request
    used for: input */
    const char* date
);
/* creates a Request object using the given releaseId and date.
precondition: 'releaseId' must be 1-8 chars.
              'date' must be in format 'YYYY-MM-DD' (10 chars).
postcondition: a Request object is successfully created.
exceptions raised: throws an exception if 'releaseId' or 'date' do not meet
                   the correct format.
*/

Item addItem(
    /* description: product name
    used for: input */
    const char* product,
    /* description: release ID
    used for: input */
    const char* release,
    /* description: description of the change item
    used for: input */
    const char* description,
    /* description: priority of the change item
    used for: input */
    int priority
);
/* creates an Item object using the given product, release, description
and priority. Returns the Item.
precondition: 'product' must be 1-10 chars.
              'release' must be 1-8 chars.
              'description' must be 1-30 chars.
              'priority' must be an int (1-5).
postcondition: an Item object is successfully created.
exceptions raised: throws an exception if 'relname', 'release', description'
                   or 'priority' do not meet the correct format.
*/
