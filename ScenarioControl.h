/* ScenarioControl.h
description:
The scenario control module is the module that prompts the user for input and calls the
functions from the other modules.
version history:
ver5 - 24/07/31, updated by Puja Shah, Wah Paw Hse, and Nicaolao Barreto
ver4 - 24/07/31, updated by Puja Shah, Wah Paw Hse, and Nicolao Barreto
ver3 - 24/07/30, update by Puja Shah, Wah Paw Hse, and Nicolao Barreto
ver2 -24/07/15, update by Puja Shah, Wah Paw Hser, and Nicolao Barreto
    -added helper functions 
ver1 -24/07/02, original by Wah Paw Hser
*/

//==================
#ifndef SCENARIO_CONTROL_H
#define SCENARIO_CONTROL_H

#include "ChangeItem.h"
#include "ChangeRequest.h"
#include "Product.h"
#include "Release.h"
#include "Requester.h"
#include "Constants.h"
#include <stdint.h>

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
// database management processes

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
// report processes

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

bool queryItemControl();
/*this is a function that allows the user to choose a change item and view all details for it.
preconditions: none
postconditions: none
exceptions raised: option not available if user enters anything other than what is valid
*/

//==============
// helper functions
//  used for reports and updating change items

int16_t selectProduct(bool a, bool b);
/* this is a function that helps the user select a product form a list of all available products and returns a value indicating selection and position in array during selection.
it has three modes: if a = 0, it will shows text relevant to a report. if a = 1, it will show text relevant to updating an item. Based on b, it will determine which item menu's header to print
precondition: none
postcondition: the database remains unchanged
exceptions raised: options does not exist and character type invalid
*/

void selectItem(change_item& readInto, change_item &filter, bool a);
/* this is a function that helps the user select a change_item form a  filtered list and returns a value indicating selection and position in array during selection
has two modes: each for a differnt kind of report
precondition: none
postcondition: the database remains unchanged
exceptions raised: options does not exist and character type invalid
*/

void selectItemUpdate(change_item& readInto, change_item &filter);

int listOfRequesters(change_request &filterId);
/* this function allow a user to see a list of all requesters associated with a given change item 16 at a time
precondition: none
postcondition: the database remains unchanged
exceptions raised: options does not exist and character type invalid
*/

bool updateStatus(change_item &item);
/* this function takes in a change item and guides the user through the process of changing its status and updating it in the database
returns true for go back to main menu
precondition: the change item must exist in the database
postcondition: none
exceptions raised: option does not exist and character type invalid
*/

void updateRelease(change_item &item);
/* this function takes in a change item and guides the user through the process of changing its release and updating it in the database
precondition: the change item must exist in the database
postcondition: none
exceptions raised: option does not exist and character type invalid
*/

bool updatePriority(change_item &item);
/* this function takes in a change item and guides the user through the process of changing its priority and updating it in the database
returns true for go back to main menu
precondition: the change item must exist in the database
postcondition: none
exceptions raised: option does not exist and character type invalid
*/

bool updateDescription(change_item &item);
/* this function takes in a change item and guides the user through the process of changing its description and updating it in the database
returns true for go back to main menu
precondition: the change item must exist in the database
postcondition: none
exceptions raised: option does not exist and character type invalid
*/

// types of print:
void printUpdateCIMenu(change_item &item);
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

void itemConfirmShow(change_item &selectedItem, bool a);
/* prints a given change_item in a specific layout
has two modes: each for separate reports
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

bool isValidNumber(const std::string& str);
/* verifies if an input is fully numeric. returns true if it is, false otherwise.
precondition:none
postcondition: none
exceptions raised: none
*/

void itemReportPrint(int number, change_item& item);
/* prints out the change_item in a certain format
preconditions: none
postconditions: the item remains unchanged
exceptions raised: none
*/


int listItems(change_item& filter);
/* generates a report of change items filtered by the user. Allows users to go to the next page, if there is one
preconditions: none
postconditions: the item remains unchanged
exceptions raised: options not available and invalid character type
*/

void selectItemUpdate(change_item& readInto, change_item &filter);
/* allows users to select a  change item to be updated and stores it in readInto
preconditions: none
postconditions: none
exceptions raised: options not available and invalid character type
*/

//==================
//  used for handling a change request and managing products

void printIndex(
    /* index is number to be printed in the index
    used as input, does not mutate */
    int index);
/* description:
    function to print the nth index in a list menu printout
*/

void printPhone(
    /* phone number to print
    used as input */
    char *phone);
/* description:
    function to print a phone number
*/

int makeSelection();
/* description:
    UI collection function for strictly defined menus
returns:
    value of multicharacter literal hashing to the user's input
*/

int getIndexInput(int menuCount);
/* description:
    UI collection for dynamic list menu optios
returns:
    1 to n for valid input
    -1 for 'back
    -2 for 'back to menu'
    -3 for 'next page'
    -4 for 'new element'
    0 on failure
*/

int getYNInput();
/* description:
    UI collection for yes no prompts
returns:
    1 on yes
    0 on no
    -1 for anything else
*/

int getStringInput(
    /* length of readin
    used as input */
    const int readInLength,
    /* variable to read to
    used as output mutates */
    char* readIn
);
/* description:
    UI collection for text input
returns:
    -1 on 'back'
    0 on failure
    1 on good input
*/

#endif
