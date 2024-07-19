/* ScenarioControl.h
description:
The scenario control module is the module that prompts the user for input and calls the
functions from the other modules. 
version history:
ver 2- 
    24/07/16, original by Puja Shah and Wah Paw Hser
ver1 -
    24/07/02, original by Wah Paw Hser
*/

//==================
#ifndef SCENARIO_CONTROL_H
#define SCENARIO_CONTROL_H

#include "Change.h"
#include "Product.h"
#include "Requester.h"
#include <iomanip>
#include <cstring>
#include "Constants.h"
#include <limits>

//==================


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
    
    bool addProductControl();
    /* prompts the user for inputs to create a product and calls Product::addProduct() to
    createa new product and write it to disk.
    precondition: none.
    postcondition: a product is successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    bool addReleaseControl();
    /* prompts the user for inputs to create a product release and calls Release::addRelease()
    to create a new product release and write it to disk.
    postcondition: a product release is successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    bool addRequestControl();
    /* prompts the user for inputs to create a request. this involves calling
    Requester::addRequester if the requester does not already exist, calling Request::addRequest
    and calling Item::addItem.
    precondition: none.
    postcondition: a request and item (and requester, if the requester does not exist yet) is
    successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    bool updItemControl();
    /* allows the user to update the attributes of a change item.
    precondition: none.
    postcondition: the change item is successfully updated.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    //==================

    bool generateReportRequesters();
    /* this is a function that generates a report of users that need to be informed about a
    specific change item.
    precondition: none.
    postcondition: none.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    bool generateReportItems();
    /* this is a function that generates a report of change items for a specific product that are unresolved
    precondition: none.
    postcondition: none.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    // is set to 
    static bool initialized = 0;

    int16_t selectProduct();
    /* this is a function that helps the user select a product form a list of all available products and returns a value indicating selection and position in array during selection
    precondition: none
    postcondition: the database remains unchanged
    exceptions raised: options does not exist and character type invalid
    */


    int16_t selectItem(change_item& filter);
        /* this is a function that helps the user select a change_item form a  filtered list and returns a value indicating selection and position in array during selection
    precondition: none
    postcondition: the database remains unchanged
    exceptions raised: options does not exist and character type invalid
    */


    int listOfRequesters(change_request& filterId);
    /* this function allow a user to see a list of all requesters associated with a given change item 16 at a time
    precondition: none
    postcondition: the database remains unchanged
    exceptions raised: options does not exist and character type invalid
    */

    void updateStatus(change_item& item);
    /* this function takes in a change item and guides the user through the process of changing its status and updating it in the database
    precondition: the change item must exist in the database
    postcondition: none
    exceptions raised: option does not exist and character type invalid
    */

    void updateRelease(change_item& item);
    /* this function takes in a change item and guides the user through the process of changing its release and updating it in the database
    precondition: the change item must exist in the database
    postcondition: none
    exceptions raised: option does not exist and character type invalid
    */

    void updatePriority(change_item& item);
    /* this function takes in a change item and guides the user through the process of changing its priority and updating it in the database
    precondition: the change item must exist in the database
    postcondition: none
    exceptions raised: option does not exist and character type invalid
    */

    void updateDescription(change_item& item);
    /* this function takes in a change item and guides the user through the process of changing its description and updating it in the database
    precondition: the change item must exist in the database
    postcondition: none
    exceptions raised: option does not exist and character type invalid
    */

    //types of print:
    void printUpdateCIMenu(change_item& item);
    /* prints a given change_item in a specific layout
    precondition: none
    postcondition: none
    exceptions raised: none
    */
    void requesterReportShow(int number, requester& requester);
    /* prints a given requester in a specific layout with its associated number in the list it will be printed in
    precondition: none
    postcondition: none
    exceptions raised: none
    */
    void productReportShow(int number, product& product);
    /* prints a given product in a specific layout with its associated number in the list it will be printed in
    precondition: none
    postcondition: none
    exceptions raised: none
    */

    void itemReportShow(int number, change_item& item);
    /* prints a given change_item in a specific layout with its associated number in the list it will be printed in
    precondition: none
    postcondition: none
    exceptions raised: none
    */

    void itemConfirmShow(change_item& selectedItem);
    /* prints a given change_item in a specific layout
    precondition: none
    postcondition: none
    exceptions raised: none
    */
    void releaseUpdateShow(int number, release& release);
    /* prints a given product release in a specific layout with its associated number in the list it will be printed in
    precondition: none
    postcondition: none
    exceptions raised: none
    */

#endif
