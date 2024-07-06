/* ScenarioControl.h
description:
The scenario control module is the module that prompts the user for input and calls the
functions from the other modules. 
version history:
ver1 -24/07/02, original by Wah Paw Hser
*/

//==================

#include "Change.h"
#include "Product.h"
#include "Requester.h"

//==================

class ScenarioControl {
public:
    void initControl();
    /* initializes the Scenario Control module
    precondition: none.
    postcondition: the Scenario Control module is initialized.
    exceptions raised: none.
    */

    void uninitControl();
    /* cleans up the Scenario Control module to uninitialize.
    precondition: the Scenario Control module must already be initialized.
    postcondition: the Scenario Control module is uninitialized.
    exceptions raised: none.
    */
    
    //==================
    
    void addProductControl();
    /* prompts the user for inputs to create a product and calls Product::addProduct() to
    createa new product and write it to disk.
    precondition: none.
    postcondition: a product is successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    void addReleaseControl();
    /* prompts the user for inputs to create a product release and calls Release::addRelease()
    to create a new product release and write it to disk.
    postcondition: a product release is successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    void addRequestControl();
    /* prompts the user for inputs to create a request. this involves calling
    Requester::addRequester if the requester does not already exist, calling Request::addRequest
    and calling Item::addItem.
    precondition: none.
    postcondition: a request and item (and requester, if the requester does not exist yet) is
    successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    void updItemControl();
    /* allows the user to update the attributes of a change item.
    precondition: none.
    postcondition: the change item is successfully updated.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    //==================

    void generateReportRequesters();
    /* this is a function that generates a report of users that need to be informed about a
    specific change item.
    precondition: none.
    postcondition: none.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    void generateReportItems();
    /* this is a function that generates a report of change items for a specific product that are unresolved
    precondition: none.
    postcondition: none.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */
};
