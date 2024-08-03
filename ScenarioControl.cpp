/* ScenarioControl.cpp
description:
implementation of scenario control module
provides the logical control of movement between menus
provides input checking and delivery to lower level modules
guides the user through the process of completing a main menu selection
version history:
ver9 -24/07/31 by Puja Shah and Nicolao Barreto
    - fixed print formatting and error in report type 2.
ver8 -24/07/31 by Puja Shah and Nicolao Barreto
    - fixed bugs in requester and release prints + not done or cancelled logic, isvalid function, and reports (stb addition).
    - removed the selection label
ver7 -24/07/31 by Puja Shah and Nicolao Barreto
     -assert file read pointers are at beginning of files for handle change requests and managing products
ver6 -24/07/30 by Puja Shah and Nicolao Barreto
     -added Query Control
ver5 -24/07/30 by Puja Shah and Nicolao Barreto
     -thorough debugging of report generation and change item update process
     -ensured print formatting matches user manual
     -added better comments for newly added functions
     -modified code to allow multiple report generations and fixed selector logic for products.
ver4 -24/07/29 by Nicolao Barreto
     -thorough debugging of product managment, and change request handling
     -now prevents requester from requesting the same change multiple times
ver3 -24/07/28 by Puja Shah and Nicolao Barreto
     -fixed logical errors with continues and back
ver2 -24/07/24 fixed code producing compiler warnings by Puja Shah, and Nicolao Barreto
ver1 -24/07/15 implementation by Puja Shah, and Nicolao Barreto
*/

#include "ScenarioControl.h"
#include "Constants.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>
using std::cout;
using std::cin;
using std::endl;
using std::string;

//==================
// implementation of error messages
static bool initialized;

const char *OPTION_NOT_AVAILABLE = "The option that you have entered does not exist in the system.\n";
const char *EXCEEDS_BOUNDS = "The value that you have entered exceeds the bounds set by the data format requirements.\n\nEnter Y to try again. Enter N to return to the Main Menu.\n";
const char *BELOW_BOUNDS = "The value that you have entered is below the bounds set by the data format requirements.\n\nEnter Y to try again. Enter N to return to the Main Menu.\n";
const char *INCORRECT_CHARACTER_TYPE = "The value that you have entered does not meet the character type requirements for the given field.\n\nEnter Y to try again. Enter N to return to the Main Menu.\n";
const char *CHANGE_ITEM_STATUS_ERROR = "The change item's status cannot be changed out of its status.\n\nEnter N to return to the Main Menu.\n";
const char *REQEUSTER_UNIQUENESS_ERROR = "The requester already exists.\n\nEnter Y to continue creating a change request. Enter N to return to the main menu.\n";
const char *PRODUCT_UNIQUENESS_ERROR = "The product already exists.\n\nEnter Y to try again. Enter N to return to the main menu.\n";
const char *PRODUCT_RELEASE_UNIQUENESS_ERROR = "The product release already exists.\n\nEnter Y to try again. Enter N to return to the main menu.\n";
const char *SAVE_ERROR = "The file could not be saved at this time.\n\nEnter Y to try again. Enter N to return to the Main Menu.\n";

//==================
// implementation of init and uninit

//========

/*
Description: Initializes the Scenario Control module by initializing each of the lower level modules it calls
*/
void initControl()
{
    // initializes all lower level modules
    Product::initProduct();
    Release::initRelease();
    RequesterDatabase::init();
    ChangeItemDatabase::init();
    ChangeRequestDatabase::init();

    // sets initialized to true
    initialized = true;
}

//========

/*
Uninitializes the Scenario Control Module by uninitializing each of the lower level modules it used
*/
void uninitControl()
{
    // uninitializes all lower level modules
    Product::uninitProduct();
    Release::uninitRelease();
    RequesterDatabase::uninit();
    ChangeItemDatabase::uninit();
    ChangeRequestDatabase::uninit();

    // set initialize to false
    initialized = false;
}

//==================
// implementation of helper functions for updateChangeItemControl and reports


/*
Description: function that allows the user to select a product from the product database
returns a 16-bit integer whose first 8 bits are the product selection and lower 8 bits are the number of prints in that that run (out of 16)
returns selection value 0 for back, -1 for back to main, and between 1 - 16 for anything valid.
return values are meant to be viable arguments for an database::select() function that may be called outside the function
takes in a bool to select mode (headers to print)
*/

int16_t selectProduct(bool a, bool b){
    //the value to be returned:
    int16_t selNind = 0;

    //create a product
    product readInto;
    std::string selection;
    bool lastPage = false;
    int reqSelection = 0;  // default is back
    int entries = 0;
    bool fileEnded = false;
    bool continues = false;

    // loop runs until a selection has been made: 
    Product::seekToBeginning();

    while(!fileEnded){ 
        //store the selection as a character array (maximum 2 values and a null)
        entries = 0;

        // make sure the file has not reached the end / is not empty before looping 15 more times. check each time for file reaching the end
        cout << endl << endl;
        if(a == false){
        std::cout << "Generate Reports:" << std::endl ;
        std::cout << "Generate Report for What Product?" << std::endl ;
        }
        else{
        std::cout << "Update Change Item:" << std::endl ;
        if (b){
            std::cout << "Select a Change Item From What Product?:" << std::endl;
        } else{
            std::cout << "Show Change Items From What Product?: " << std::endl ;
        }
        }
        
        std:: cout << "     Product Name:" << std::endl;

        while (!Product::getNext(readInto)) {  
            // if the file has more entries: 
                // initialize to 0 before every run 

            if(continues){
                cout << endl << endl;
                if(a == false){
                std::cout << "Generate Reports:" << std::endl ;
                std::cout << "Generate Report for What Product?" << std::endl ;
                }
                else{
                std::cout << "Update Change Item:" << std::endl ;
                if (b){
                    std::cout << "Select a Change Item From What Product?:" << std::endl;
                } else{
                    std::cout << "Show Change Items From What Product?: " << std::endl ;
                }
                }
                
                std:: cout << "    Product Name:" << std::endl;
            }
            entries= 0;
            productReportShow(entries+1, readInto);
            entries++;


            // loop 15 times or until the file ends
            while (!Product::getNext(readInto) && entries < MAX_PRINTS) {
                productReportShow(entries+1, readInto);
                entries++;
            }

            std::cout << "[0]  Back    [00] Back to Main Menu    ";
            // if the loop ended when before 16 entries had been printed, it ended because the file ended. So then this is the last page of the file
            if(entries < MAX_PRINTS-1) {
                lastPage = true;
                std::cout  << std::endl;
            }
            else{
                std::cout << "[C]  Next Page" << std::endl;
            }

            // will be true until the response needs to keep being processed (is invalid)
            bool responseProcessed = true;

            // while user input needs to keep getting taken in :
            while(responseProcessed){
                //ask for selection
                std::cout << std::endl;
                //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
                std::getline(std::cin, selection);

                // if they wish to go back to the main menu, return -1. 
                if(selection == "00"){
                // back to the main menu : first 8 bits are -1 and the next 8 bits (irrelevant  in this case) are the number of entries printed
                    selNind |= (static_cast<int16_t>(-1) & 0xFF);
                    selNind |= (static_cast<int16_t>(entries) & 0xFF << 8);
                    return selNind;
                        } 
                else if(isValidNumber(selection)){
                    reqSelection = stoi(selection);
                    if(reqSelection >= 0 && reqSelection <= entries){
                        selNind |= (static_cast<int16_t>(reqSelection) & 0xFF);
                        selNind |= (static_cast<int16_t>(entries) & 0xFF) << 8;
                        return selNind;
                    }
                    else{
                        std::cout << OPTION_NOT_AVAILABLE << std::endl;
                    }
                } 
                else if ((selection == "C" || selection == "c") && !lastPage){
                    responseProcessed = false; // rerun printing loop, exit response loop
                    fileEnded = false;
                    continues = true;
                }
                else{
                    std::cout << OPTION_NOT_AVAILABLE << std::endl;
                }

            }
                    
                
        }

        fileEnded = true;
    } 
        // if the file could never get a single entry read, the database must be empty. show that nessaged and return 0 to show -- same as back
        std::cout << "[0]  Back    [00] Back to Main Menu" << std::endl;
        while(true){
        std::cout << std::endl;
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
        std::getline(std::cin, selection);
        if(selection == "0"){
            selNind |= (static_cast<int16_t>(reqSelection) & 0xFF) << 8 ;
            selNind |= (static_cast<int16_t>(entries) & 0xFF) ;
            return selNind;
        } 
        else if(selection == "00"){
            reqSelection = -1;
            selNind |= (static_cast<int16_t>(reqSelection) & 0xFF) << 8 ;
            selNind |= (static_cast<int16_t>(entries) & 0xFF) ;
            return selNind;
        }
        else{
            std::cout << OPTION_NOT_AVAILABLE << std::endl;
        }
        }
}


//========

/*
function that takes in a filter item and allows user to choose products from a list of all products that match the filter
takes in a change_item that holds the selection and a filter to show relevant change items by
readInto's changeid of 0 and -1 indicate back and main menu respectively.
this change item modified by this function can then be used further in the program
*/
void selectItem(change_item& readInto, change_item &filter, bool a)
{
    ChangeItemDatabase::seekToBeginning(); // reset with every use
    // the change item into which each read item will be stored:
    readInto.id = 0;
    // value to be returned:

    // while loop conditions
    bool selected = false;
    std::string selection;
    bool lastPage = false;
    // default return value = back
    int reqSelection = 0;
    int entries = 0;
    bool continues = false;

    // loop that runs until a selection has been made.
    while (!selected)
    {
        entries = 0;
        // make sure the file has not reached the end before looping 15 more times
        cout << endl << endl;
        if(a){
            std::cout << "Generate Reports:" << std::endl;
            std::cout << "Generate Report for Which Change Item?:" << std::endl;
            std::cout << "     ID       Description                   Status      Release" << std::endl;
        }
        else{
            std::cout << "Generate Reports:" << std::endl;
            std::cout << "Select Item to View:" << std::endl;
            std::cout << "     ID       Description                   Status      Release" << std::endl;
        }


        while (!ChangeItemDatabase::getNext(readInto, filter))
        {
            if(continues){
                cout << endl << endl;
                if(a){
                    std::cout << "Generate Reports:" << std::endl;
                    std::cout << "Generate Report for Which Change Item?:" << std::endl;
                    std::cout << "     ID       Description                   Status      Release" << std::endl;
                }
                else{
                    std::cout << "Generate Reports:" << std::endl;
                    std::cout << "Select Item to View:" << std::endl;
                    std::cout << "     ID       Description                   Status      Release" << std::endl;
                }
            }
            entries = 0;
            itemReportShow(entries+1, readInto);
            entries++;

            // populate menu
            while (!ChangeItemDatabase::getNext(readInto, filter) && entries < MAX_PRINTS)
            {
                itemReportShow(entries+1, readInto);
                entries++;
            }

            std::cout << "[0]  Back    [00] Back to Main Menu    ";
            // if it stops printing but has not reached 16 values yet, the file has ended.
            if (entries < MAX_PRINTS){
                lastPage = true;
                std::cout << std::endl;
            }
            else{
                std::cout << "[C]  Next Page" << std::endl;
            }

            bool responseProcessed = true;
            // while responseProcessed is true, this loops keeps running

            while (responseProcessed)
            {
                std::cout << std::endl;
                //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
                std::getline(std::cin, selection);

                // If they select going back to the main menu, return -1 as the selection
                if (selection == "00") {
                    readInto.id = -1;
                    return; // back to the main menu;
                } else {
                    std::string input(selection);

                    if (isValidNumber(input)) {
                        reqSelection = std::stoi(input); // Safe to convert

                        if (reqSelection == 0) { // if user entered 0
                            readInto.id = 0;
                            return; // back;
                        } else if (reqSelection > 0 && reqSelection <= entries) { // if user entered a value between 0 and entries
                            selected = true;
                            responseProcessed = false;
                            ChangeItemDatabase::seekToBeginning();
                            ChangeItemDatabase::select(readInto, reqSelection, entries);
                            return;
                        } else { // if user enters a different number
                            responseProcessed = true;
                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                        }
                    } else {
                        if ((selection == "C"  || selection == "c" )&& !lastPage) { // if it's a C and there are more entries, it's valid
                            responseProcessed = false;
                            continues = true;
                        } else { // any other character is invalid
                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                            
                        }
                    }
                }
            }
        }

            // if the loop never runs, the database is empty
            readInto.id = 0;
            std::cout << "[0]  Back    [00] Back to Main Menu" << std::endl;
            while(true){
            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
            std::getline(std::cin, selection);
            std::cout << std::endl;
            if(selection == "0"){
                readInto.id = 0;
                return;
            } 
            else if(selection == "00"){
                readInto.id = -1;
                return;
            }
            else{
                std::cout << OPTION_NOT_AVAILABLE << std::endl;
            }
        }
    }

    // once selection has been made, return it:
    return;
}

//========

/*
function that takes in a filter item and allows user to choose products from a list of all products that match the filter
takes in a change_item that holds the selection and a filter to show relevant change items by
readInto's changeid of 0 and -1 indicate back and main menu respectively.
this change item modified by this function can then be used further in the program
specifically formatted for update menu
*/

void selectItemUpdate(change_item& readInto, change_item &filter)
{
    ChangeItemDatabase::seekToBeginning(); // reset with every use
    // the change item into which each read item will be stored:
    readInto.id = 0;
    // value to be returned:

    // while loop conditions
    bool selected = false;
    std::string selection;
    bool lastPage = false;
    // default return value = back
    int reqSelection = 0;
    int entries = 0;
    bool continues = false;

    // loop that runs until a selection has been made.
    while (!selected)
    {
        entries = 0;
        // make sure the file has not reached the end before looping 15 more times
        cout << endl << endl;
        std::cout << "Update Change Items:" << std::endl;
        std::cout << "Select Items to Update:" << std::endl;
        std::cout << "     ID      Description                     Status       Product Name" << std::endl;

        while (!ChangeItemDatabase::getNext(readInto, filter))
        {
            if(continues){
                cout << endl << endl;
                std::cout << "Update Change Items:" << std::endl;
                std::cout << "Select Items to Update:" << std::endl;
                std::cout << "     ID      Description                     Status       Product Name" << std::endl;
            }
            entries = 0;
            itemReportPrint(entries+1, readInto);
            entries++;

            // populate menu
            while (!ChangeItemDatabase::getNext(readInto, filter) && entries < MAX_PRINTS)
            {
                itemReportPrint(entries+1, readInto);
                entries++;
            }

            std::cout << "[0]  Back    [00] Back to Main Menu    ";
            // if it stops printing but has not reached 16 values yet, the file has ended.
            if (entries < MAX_PRINTS){
                lastPage = true;
                std::cout << std::endl;
            }
            else{
                std::cout << "[C]  Next Page" << std::endl;
            }

            bool responseProcessed = true;
            // while responseProcessed is true, this loops keeps running

            while (responseProcessed)
            {
                std::cout << std::endl;
                //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
                std::getline(std::cin, selection);

                if (selection == "00") {
                    readInto.id = -1;
                    return; // back to the main menu;
                } else {
                    std::string input(selection);

                    if (isValidNumber(input)) {
                        reqSelection = std::stoi(input); // Safe to convert

                        if (reqSelection == 0) { // if user entered 0
                            readInto.id = 0;
                            return; // back;
                        } else if (reqSelection > 0 && reqSelection <= entries) { // if user entered a value between 0 and entries
                            selected = true;
                            responseProcessed = false;
                            ChangeItemDatabase::seekToBeginning();
                            ChangeItemDatabase::select(readInto, reqSelection, entries);
                            return;
                        } else { // if user enters a different number
                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                        }
                    } else {
                        if ((selection == "C" || selection == "c") && !lastPage) { // if it's a C and there are more entries, it's valid
                            selected = false;
                            responseProcessed = false;
                            continues = true;
                        } else { // any other character is invalid
                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                        }
                    }
                }
            }
        }

            // if the loop never runs, the database is empty
            std::cout << "[0]  Back    [00] Back to Main Menu" << std::endl;
            while (true) {
            std::getline(std::cin, selection);
            if (selection == "0") {
                readInto.id = 0;
                return;
            } else if (selection == "00") {
                readInto.id = -1;
                return;
            } else {
                std::cout << OPTION_NOT_AVAILABLE << std::endl;
            }
        }
            
    }

    // once selection has been made, return it:
    return;
}

/*
Desription: Function that shows users a list of all requesters for change request with a given change item id. (filtered by chage id)
returns 0 for back and -1 for back to main menu
*/
int listOfRequesters(change_request &filterId)
{
    // create a requester
    change_request aRequest;
    bool fileEnded = false;
    std::string selection;
    int count = 0;
    bool continues = false;
    ChangeRequestDatabase::seekToBeginning();
    RequesterDatabase::seekToBeginning();
    // repeat process until user provides viable input or file is exhausted
    while (!fileEnded)
    {

        // print up to 16 reports:
        cout << endl << endl;
        std::cout << "Requester Name                Phone            Email" << std::endl;
        while (!ChangeRequestDatabase::getNext(aRequest, filterId))
        {
            if(continues) {
                cout << endl << endl;
                std::cout << "Requester Name                Phone            Email" << std::endl;
            }
            requester a;
            requester afil;
            afil.requesterId = aRequest.requesterId;
            RequesterDatabase::getNext(a, afil);
            count = 0;
            requesterReportShow(count + 1, a);
            RequesterDatabase::seekToBeginning();
            count++;

            // print 15 more times or until eof is reached
            while (count < 16 && !ChangeRequestDatabase::getNext(aRequest, filterId))
            {
                requester a;
                requester afil;
                afil.requesterId = aRequest.requesterId;
                RequesterDatabase::getNext(a, afil);
                RequesterDatabase::seekToBeginning();
                requesterReportShow(count + 1, a);
                count++;
            }

            std::cout << "[0]  Back    [00] Back to Main Menu    ";
            // Check if the inner loop ended due to reaching the end of the file:
            if (count < 16)
            {
                fileEnded = true;
                std::cout << std::endl;
            }
            else{
                std::cout << "[C]  Next Page" << std::endl;
            }

            bool responseProcessed = true;

            // remains true until response needs to be processed
            while (responseProcessed)
            {

                std::cout << std::endl;
                std::cin >> selection;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (selection == "00")
                {
                    // back to the main menu
                    return -1;
                }
                else if(isValidNumber(selection)){
                    if(selection == "0"){
                        return 0;
                    }else{
                        std::cout << OPTION_NOT_AVAILABLE << std::endl;
                    }
                }
                else if((selection == "C"  || selection == "c" )&& !fileEnded){
                    responseProcessed = false;
                    continues = true;
                }
                else{
                    std::cout << OPTION_NOT_AVAILABLE << std::endl;
                }
            }
        }
        fileEnded = true;
    }
    // if the database was empty, back to the main menu  (cannot generate a report for requesters if there are no requesters)
    std::cout << "[0]  Back    [00] Back to Main Menu" << std::endl;
    
    while(true){
    std::cout << std::endl;
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
    std::getline(std::cin, selection);
    if(selection == "0"){
        return 0;
    } 
    else if(selection == "00"){
        return -1;
    }
    else{
        std::cout << OPTION_NOT_AVAILABLE << std::endl;
    }
    }

}

//========

/*
Description: function that formats a requester for when listing them in a report
Fixed length prints that include their name, email, and phone number
Also display the item number on the left -- so as to make it easy for the user to choose a number when displayed in a list
*/
void requesterReportShow(int number,requester& requester) {
    if(strlen(requester.name) != 0){   // making sure only valid entries are being printed
    char nameTruncated[MAX_REQUESTER_NAME_SIZE];
    strncpy(nameTruncated, requester.name, MAX_REQUESTER_NAME_SIZE - 1);
    nameTruncated[MAX_REQUESTER_NAME_SIZE - 1] = '\0'; // Ensure null-termination

    char phoneTruncated[PHONE_NUMBER_SIZE + 2];
    strncpy(phoneTruncated, requester.phone, PHONE_NUMBER_SIZE);
    phoneTruncated[PHONE_NUMBER_SIZE + 1] = '\0'; // Ensure null-termination

    char emailTruncated[MAX_EMAIL_SIZE];
    strncpy(emailTruncated, requester.email, MAX_EMAIL_SIZE - 1);
    emailTruncated[MAX_EMAIL_SIZE - 1] = '\0'; // Ensure null-termination

    // Print formatted output with number in square brackets
             std::cout 
              << std::left << std::setw(MAX_REQUESTER_NAME_SIZE) << nameTruncated;
              printPhone(phoneTruncated);
              std::cout
              << std::setw(2) << ""
              << std::setw(MAX_EMAIL_SIZE) << emailTruncated << std::endl;
    }
   
}

//========

/*
Description: function that formats a product before printing it in a list for users to choose from
*/
void productReportShow(int number, product& product)
{
    // formatting for printing

    char nameTruncated[MAX_PRODUCT_NAME_SIZE];
    strncpy(nameTruncated, product.name, MAX_PRODUCT_NAME_SIZE-1);
    nameTruncated[MAX_PRODUCT_NAME_SIZE-1] = '\0'; // Ensure null-termination

    int bigNum = 0;
    if(number <= 9){
        bigNum++;
    }

    std::cout << "[" << number << "] "
              << std::left << std::setw(bigNum) << ""
              << std::setw(MAX_REQUESTER_NAME_SIZE) << nameTruncated << std::endl;
}

//========

/*
Description: function that formats a change item it takes in as an argument and prints out its change id, description, status, and release id
Allows for a number to the left to help with printing as a list a user can select from in a list
*/
void itemReportShow(int number, change_item &item)
{
    int value = item.status;
    int power = 0;
    while(value > 1){
        value /= 2;
        power++;
    }

    value = 1;
    if(number <= 9) value = 2;

    string states[] = {"unreviewed", "reviewed", "inProgress", "done", "cancelled"};
    char descTruncated[MAX_DESCRIPTION_SIZE];
    strncpy(descTruncated, item.description, MAX_DESCRIPTION_SIZE - 1);
    descTruncated[MAX_DESCRIPTION_SIZE - 1] = '\0'; // Ensure null-termination

    char relTruncated[MAX_RELEASE_ID_SIZE];
    strncpy(relTruncated, item.release, MAX_RELEASE_ID_SIZE - 1);
    relTruncated[MAX_RELEASE_ID_SIZE - 1] = '\0'; // Ensure null-termination

    // Print formatted output
    std::cout << "[" << number << "] "
              << std::left << std::setw(value) << ""
              << std::right << std::setw(6) << item.id
              << std::setw(2) << ""
              << std::left << std::setw(MAX_DESCRIPTION_SIZE) << descTruncated
              << std::setw(12) << states[power]
              << std::setw(MAX_RELEASE_ID_SIZE) << relTruncated << std::endl;
}

//========
void itemConfirmShow(change_item &selectedItem, bool a)

{
    string states[] = {"unreviewed", "reviewed", "inProgress", "done", "cancelled"};

    // formatting for printing
    int value = selectedItem.status;
    int power = 0;
    while(value > 1){
        value /= 2;
        power++;
    }
    cout << endl << endl;
    if (a){
        std::cout << "Generate Reports:" << std::endl;
    }
    else{
        std::cout << "View Change Item:" << std::endl;
    }
    
    std::cout << "Change Item:  " << selectedItem.id << std::endl;
    std::cout << "Product:      " << selectedItem.product << std::endl;
    std::cout << "Status:       " << states[power] << std::endl;
    std::cout << "Description:  " << selectedItem.description << std::endl;
    std::cout << "Priority:     " << 0 + selectedItem.priority << std::endl;
    std::cout << "Release:      " << selectedItem.release << std::endl;
    if(a){
        release corresponding;
        release filtering;
        strncpy(filtering.name, selectedItem.release, MAX_RELEASE_ID_SIZE);
        Release::getNext(corresponding, filtering);
        std::cout << "Release Date: " << corresponding.date << std::endl;
        std::cout << "Create a Report for Change Item " << selectedItem.id << "?" << std::endl;
    }
    else{
        std::cout << "[0]  Back    [00] Back to Main Menu" << std::endl;
    }

}

//========

/*
function that verifies if a given string input is fully numerical or has alphabets in it
returns true for fully numerical and false otherwise
helps evaluate user input
*/
bool isValidNumber(const std::string& str) {
    if (str.empty()) return false;

    // Check if the string contains only digits
    for (char c : str) {
        if (!isdigit(c)) return false;
    }

    // Convert the string to an integer
    int number;
    std::istringstream iss(str);

    if (!(iss >> number)) {
        return false; // Conversion failed
    }

    // Check if the number is within the desired range
    return number >= 0;
}

/*
Description: updates status of a given change item
no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
no back to main menu option
*/
bool updateStatus(change_item &item)
{
    int yn;
    std::string selection;
       // repeat step until user selects back or system failure
    while (1)
    {
        // status options:
        if(item.status != 8 && item.status != 16){
            cout << endl << endl;
            std::cout << "Update Change Item:" << std::endl;
            std::cout << "Update Status: " << std::endl;
            std::cout << "[1]  Reviewed " << std::endl;
            std::cout << "[2]  In Progress " << std::endl;
            std::cout << "[3]  Done " << std::endl;
            std::cout << "[4]  Cancelled " << std::endl;
            std::cout << "[0]  Back " << std::endl;

            // get new status from user.
            int newStatus;

            // ask for an integer selection
            while (true)
            {
                std::cout << std::endl;
                std::cin >> newStatus;  

                // Check if the input is a valid integer
                if (std::cin.fail()) {
                    std::cin.clear(); // Clear the error state
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                    std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                    yn = getYNInput();
                    if(yn){
                        continue;
                    }
                    else{ 
                        // if they enter n , N , or an invalid value, go back to main menu
                        return true;
                    }
                }  
                else {  
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard any remaining input
                    if(newStatus >= 0 && newStatus <= 4){
                        break;
                    }
                    else{
                        std::cout << OPTION_NOT_AVAILABLE << std::endl;
                    }
                }
            }

            // if they select back, exit function
            if (newStatus == 0)
            {
                return false;
            }

            // if the option is valid:
            char *states[4];
            char one[] = "Reviewed";
            states[0] = one;
            char two[] = "In Progress";
            states[1] = two;
            char three[] = "Done";
            states[2] = three;
            char four[] = "Cancelled";
            states[3] = four;


            if(newStatus >= 3){
                // ask for confirmation
                std::cout << "Update Change Item:" << std::endl;
                std::cout << "Update Status:" << std::endl;
                std::cout << "Confirm change to " << states[newStatus - 1] << "?" << std::endl;
                // if confirmed, update it and end
                while(true){
                std::string confirmation;
                std::cout << "[y/n]: " << std::endl;

                std::getline(std::cin, confirmation);
                if (confirmation == "y" || confirmation == "Y")
                {
                    break;
                    // break loop and go to the next step
                }
                // if they say no, go back to showing all the options again
                else if(confirmation == "N" || confirmation == "n" )
                {
                    return false;
                }
                else{
                    std::cout << OPTION_NOT_AVAILABLE << std::endl;
                }   // continue the loop
                }

            }
    

                uint8_t stat = 1;
                // change status to the selected value
                item.status = (stat << newStatus);

                // write it back to the database
                ChangeItemDatabase::writeElement(item);
                std::cout << "Status Updated" << std::endl;
                return false;
        }
        else { 
        std::cout << CHANGE_ITEM_STATUS_ERROR << std::endl;  // cannot change out of done or cancelled
        std::getline(std::cin, selection);  // enter N to return to main menu
        if(selection == "N" || selection == "n"){
            return true;
        }
        else{
            return false;
        }
        }
    
    }      
return false;
}
//========

/*
Description: updates release of a given change item
no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
no back to main menu option
*/
void updateRelease(change_item &item)
{
    // finding associated released with the product
    release toChange;
    release filler;
    strncpy(filler.name, item.product, MAX_PRODUCT_NAME_SIZE);
    std::string selection;
    bool continues = false;

    // bool fileEnded = (changeItemCount >= 0);           // CHANGE TO THIS ONCE FUNCTION IS READY
    bool fileEnded = false;
    // while we haven't reached the end of the file:
    Release::seekToBeginning();
    cout << endl << endl;
    std::cout << "Update Change Item:" << std::endl;
    std::cout << "Select Change Item's Release" << std::endl;
    std::cout << "     Release Name:" << std::endl;
    while (!fileEnded)
    {

        int count = 0;
        // print up to 16 reports
            if(continues){
            cout << endl << endl;
            std::cout << "Update Change Item:" << std::endl;
            std::cout << "Select Change Item's Release" << std::endl;
            std::cout << "     Release Name:" << std::endl;

            }

        while (count < 16 && !Release::getNext(toChange, filler))
        {
            releaseUpdateShow(count+1, toChange);
            count++;
        }

        // Check if the inner loop ended due to reaching the end of the file:
        std::cout << "[0]  Back    ";
        if (count < 16)
        {
            std::cout << std::endl;
            fileEnded = true;
        }
        else{
            std::cout << "[C]  Next Page" << std::endl;
        }

        int reqSelection;
        bool responseProcessed = true;

        // response processed must be true while responses are being processed.
        while (responseProcessed)
        {

            std::cout << std::endl;
            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
            std::getline(std::cin, selection);
            // ask user for selection

            if(isValidNumber(selection)){  // go back
                reqSelection = stoi(selection);
                if(reqSelection == 0){
                    return;
                }
                else if (reqSelection > 0 && reqSelection <= count) {
                    //Release::seekToBeginning();
                    Release::select(toChange, reqSelection, count);
                    strncpy(item.release, toChange.releaseId, MAX_RELEASE_ID_SIZE);
                    ChangeItemDatabase::writeElement(item);
                    std::cout << "Release Updated" << std::endl;
                    return;
                }
                else{
                    std::cout << OPTION_NOT_AVAILABLE << std::endl;
                }
            }
            else if(selection == "C" || selection == "c"){
                if(!fileEnded){
                    responseProcessed = false;
                    continues = true;
                }
                else{
                    responseProcessed = true;
                }
            }
            else{
                std::cout << OPTION_NOT_AVAILABLE << std::endl;

            }
        }
    }
    // exit function if file ended was true
    std::cout << "[0]  Back";
    while(true){
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
        std::getline(std::cin, selection);
        if(selection == "0"){
            return;
        }
       else{
            std::cout << OPTION_NOT_AVAILABLE << std::endl;
    }

    }
    return;
}

//========

/*
Description: updates priority of a given change item
no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
no back to main menu option
*/
bool updatePriority(change_item &item)
{
    std::string selection;
        // repeat step until user selects back or system failure
    while (1)
    {
        // menu options for priority
        cout << endl << endl;
        std::cout << "Update Change Priority" << std::endl;
        std::cout << "Update Priority" << std::endl;
        std::cout << "[1] Lowest" << std::endl;
        std::cout << "[2] Low" << std::endl;
        std::cout << "[3] Middle" << std::endl;
        std::cout << "[4] High" << std::endl;
        std::cout << "[5] Highest" << std::endl;

        // take in selection
        int selected;

        // in the process of making a selection
        bool selecting = true;

        //collect a selection
        while (selecting) {
            std::cout << "Select: ";
            std::cin >> selected;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (std::cin.fail()) {
                // Input was not an integer
                std::cin.clear(); // Clear the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the input
                std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
                std::getline(std::cin, selection);
                if(selection == "Y" || selection == "y"){
                    selecting = true;
                }
                else{
                    return true; // go back
                }
            } 
            else {
                selecting = false; // Valid number, finished selecting
            }
    }

        // case for back : exit function
        if (selected == 0)
        {
            return false;
        }

        // case for valid selection: update and exit function
        if (selected <= 5 && selected >= 0)
        {
            item.priority = selected;
            // write back to file
            ChangeItemDatabase::writeElement(item);
            std::cout << "Priority Updated" << std::endl;
            // exit
            return false;
        }
        // all other cases, show error and reprompt
        else
        {
            std::cout << OPTION_NOT_AVAILABLE << std::endl;
        }
    }
}

//========

 /*
Description: updates description of a given change item
ensures the description is less than the maximum allowed length
no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
no back to main menu option
*/
bool updateDescription(change_item &item)
{
    // repeat step until user selects back or system failure
    while (1)
    {
        // prompt
        cout << endl << endl;
        std::cout << "Update Change Item:" << std::endl;
        std::cout << "Update Description:" << std::endl;
        std::cout << "[0]  Back" << std::endl
                  << std::endl;
        std::cout << "Field Requirements: " << std::endl;
        std::cout << "(1-30 alphanumeric characters)" << std::endl;
        std::cout << "Enter Description: " << std::endl;

        char input[MAX_DESCRIPTION_SIZE];

        // get user input
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
        std::cin.getline(input, MAX_DESCRIPTION_SIZE);

        // Check if input exceeded maxSize - 1 characters
        if (std::cin.fail() && std::cin.gcount() == MAX_DESCRIPTION_SIZE - 1)
        {
            std::cin.clear();                                                   // Clear error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore rest of line
            std::cout << EXCEEDS_BOUNDS << std::endl;
            int a = getYNInput();
            if(a == 1){
                continue;
            }
            else{
                 return true;
            }
           
        }
        else
        {
            // update and write to file before exiting the function
            strncpy(item.description, input, MAX_DESCRIPTION_SIZE);
            ChangeItemDatabase::writeElement(item);
            std::cout << "Description Updated" << std::endl;
            return false;
        }
    }
}

//========

/*
Description: prints change item in a multi-line format to make it easier for users to see attributes they would like to update
Associates numbers with associated attributes that allow users to make a selection
*/
void printUpdateCIMenu(change_item &item)
{
    string states[] = {"unreviewed", "reviewed", "inProgress", "done", "cancelled"};

    int value = item.status;
    int power = 0;
    while(value > 1){
        value /= 2;
        power++;
    }
    cout << endl << endl;
    std::cout << "Update Change Item:" << std::endl;
    std::cout << "    Change Item:     " << item.id << std::endl;
    std::cout << "    Product:         " << item.product << std::endl;
    std::cout << "[1] Status:          "      << states[power] << std::endl;
    std::cout << "[2] Description:     " << item.description << std::endl;
    std::cout << "[3] Priority:        " << 0 + item.priority << std::endl;
    std::cout << "[4] Release:         " << item.release << std::endl;
    release corresponding;
    release filtering;
    strncpy(filtering.name, item.release, MAX_RELEASE_ID_SIZE);
    Release::getNext(corresponding, filtering);
    std::cout << "    Release Date:    " << corresponding.date << std::endl;
    std::cout << "[0]  Back  [00] Back to Main Menu" << std::endl;
}

//========

/*
Description: Displays release names for the user to select from to update to
only displays its id
*/
void releaseUpdateShow (int number, release& release)
{
    char idTruncated[MAX_RELEASE_ID_SIZE];
    strncpy(idTruncated, release.releaseId, MAX_RELEASE_ID_SIZE - 1);
    idTruncated[MAX_RELEASE_ID_SIZE - 1] = '\0'; // Ensure null-termination

    int bigNum = 0;
    if(number <= 9){
        bigNum++;
    }
    // Print formatted output
    std::cout << "[" << number << "] "
    <<std::left << std::setw(bigNum) << ""
    <<std::setw(ID_DIGITS) << release.releaseId << std::endl;
    return;
}

//==============
// implementation of updating change item

/* allows the user to update the attributes of a change item.
precondition: none.
postcondition: the change item is successfully updated.
exceptions raised: throws an exception if the user enters an option that does not exist
during the input collection process.
*/
bool updItemControl()
{
  int yn;
        // make sure the module has been initialized
    if (initialized)
    {   
        // get input
        while (1)
        {
            // ask user to select a product from a list of all products and rememeber the selection value and elements printed in that selection
            cout << endl << endl;
            std::cout << "Update Change Item: " << std::endl;
            std::cout << "[1]  View Items for All Products" << std::endl;
            std::cout << "[2]  View Items for Product" << std::endl;
            std::cout << "[0]  Back" <<std::endl;
            // initialie a blank product
            product selectedProduct;
            bool filterByProduct = false;
            bool selecting = true;
            int itemSelected;
            std::string selection;
            int prodFilter;
            // take in user input:
            while (selecting)
            { 
                std::cout << std::endl;
                std::getline(std::cin, selection);

                if(selection == "0"){
                    return false;
                }
                else if(isValidNumber(selection)){
                    prodFilter = stoi(selection);
                    if(prodFilter == 1){
                        selecting = false;
                    } 
                    else if(prodFilter == 2){
                        filterByProduct = true;
                        selecting = false;

                    }
                    else{
                        std::cout << OPTION_NOT_AVAILABLE << std::endl;
                    }
                }
                else{
                    std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                    yn = getYNInput();
                    if(yn){
                        selecting = true;
                    }
                    else{ 
                        // if they enter n , N , or an invalid value, go back to main menu
                        return true;
                    }
                }
            }

            if (filterByProduct)
            {
                int16_t selNind = selectProduct(true, true);
                int8_t productSelected = static_cast<int8_t>(selNind & 0xFF);
                int8_t arrayIndex = static_cast<int8_t>((selNind >> 8) & 0xFF);

                // processing selections:

                // back to main menu:
                if (productSelected == -1)
                {
                    return true;
                    // back:
                }
                else if (productSelected == 0)
                {
                    continue;; // start again
                    // option selected:
                }
            
                // if a valid selection has been made, update selectedProduct accordingly
                Product::seekToBeginning();
                Product::select(selectedProduct, productSelected, arrayIndex + 1);
            }
                    
                        // get input
                        while (1)
                        {
                            // ask user which items they want to see
                            cout << endl << endl;
                            std::cout << "Update Change Items: " << std::endl;
                            std::cout << "Show what change item?: " << std::endl;
                            std::cout << "[1] New Change Items" << std::endl;
                            std::cout << "[2] Reviewed Change Items" << std::endl;
                            std::cout << "[3] In Progress Change Items" << std::endl;
                            std::cout << "[4] All Not Done Or Cancelled" << std::endl;
                            std::cout << "[5] All" << std::endl;
                            std::cout << "[0] Back" << std::endl;
                            int updateX;
                            // take in their input
                            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
                            while (true) {
                                std::cout << std::endl;
                                std::cin >> updateX;

                                // Check if the input is a valid integer
                                if (std::cin.fail()) {
                                    std::cin.clear(); // Clear the error state
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                                    std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                                    yn = getYNInput();
                                    if(yn){
                                        continue;
                                    }
                                    else{ 
                                        // if they enter n , N , or an invalid value, go back to main menu
                                        return true;
                                    }
                                } 
                                else {  
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard any remaining input
                                    if(updateX >= 0 && updateX <= 5){
                                        break;
                                    }
                                    else{
                                        std::cout << OPTION_NOT_AVAILABLE << std::endl;
                                    }
                                }
                            }

                            if(updateX == 0) break; // exist from here as well if they selected back 

                            // decalre a change_item that will be updated
                            change_item mainSelection;

                            // control conditions for while loops
                            bool toContinue = true;
                            bool askAgain = true;

                            // a filter change item to filter by the selected input from the above menu
                            change_item filter;
                            // make sure it is for the selected product
                            strncpy(filter.product, selectedProduct.name, MAX_PRODUCT_NAME_SIZE);
                            switch (updateX)
                            {
                                // filtered by new status
                            case 1:
                                filter.status = unreviewed;
                                // make a selection from that filtered list
                                selectItemUpdate(mainSelection, filter);
                                itemSelected = mainSelection.id;

                                // if user selected to go back, exit this loop and go not move on to the next step
                                if (itemSelected == 0)
                                {
                                    toContinue = false;
                                    break;
                                    // if they select to go back to the main menu, exit the function
                                }
                                else if (itemSelected == -1)
                                {
                                    return false;
                                    // in all other cases, exit the switch
                                }
                                else
                                {
                                    break;
                                }

                            case 2:
                                // filter and select
                                filter.status = reviewed;
                                selectItemUpdate(mainSelection, filter);
                                itemSelected = mainSelection.id;
                                // go back
                                if (itemSelected == 0)
                                {
                                    toContinue = false;
                                    break;
                                    // main menu
                                }
                                else if (itemSelected == -1)
                                {
                                    return false;
                                    // continue ahead
                                }
                                else
                                {
                                    break;
                                }
                                break;

                            case 3:
                                // filter and select
                                filter.status = inProgress;
                                selectItemUpdate(mainSelection, filter);
                                itemSelected = mainSelection.id;
                                // go back
                                if (itemSelected == 0)
                                {
                                    toContinue = false;
                                    break;
                                    // main menu
                                }
                                else if (itemSelected == -1)
                                {
                                    return false;
                                    // continue ahead
                                }
                                else
                                {
                                    break;
                                }

                            case 4:
                                // filter and select
                                filter.status = (unreviewed | reviewed | inProgress);
                                selectItemUpdate(mainSelection, filter);
                                itemSelected = mainSelection.id;

                                // go back
                                if (itemSelected == 0)
                                {
                                    toContinue = false;
                                    break;
                                    // back to the main menu
                                }
                                else if (itemSelected == -1)
                                {
                                    return false;
                                    // continue ahead
                                }
                                else
                                {
                                    break;
                                }

                            case 5:
                                // no filter select (other than product, if filtered by it)
                                selectItemUpdate(mainSelection, filter);
                                itemSelected = mainSelection.id;
                                // back case
                                if (itemSelected == 0)
                                {
                                    toContinue = false;
                                    break;
                                    // main menu
                                }
                                else if (itemSelected == -1)
                                {
                                    return false;
                                    // continue ahead
                                }
                                else
                                {
                                    break;
                                }

                            case 0:
                                // if they go back, exit switch and while loop
                                toContinue = false;
                                askAgain = false;
                                break;

                            default:
                                std::cout << OPTION_NOT_AVAILABLE << std::endl;
                                continue;

                            }

                            // if they are not to contiue
                            if (!toContinue)
                            {
                                // if they said back in the switch, exit this while loop and go back to previous menu
                                if (!askAgain)
                                    break;
                                // if they said back in the function, restart loop
                                continue;
                            }
                            else
                            {
                                // otherwise, continue ahead
                                while (1)
                                {
                                    // ask user what they would like to update and display details of the change_item they selected
                                    cout << endl << endl;
                                    std::cout << "What do you want to do for this item? " << std::endl;
                                    printUpdateCIMenu(mainSelection);
                                    // take in response
                                    std::string responseC;
                                    std::getline(std::cin, responseC);

                                    // go back to main menu
                                    int response = 0;
                                    if (responseC == "00")
                                    {
                                        return false;
                                    } else if(isValidNumber(responseC)){
                                         response = stoi(responseC);
                                        if(response > 4 || response < 0){
                                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                                            continue;
                                        }
                                    } else{
                                        std::cout << OPTION_NOT_AVAILABLE << std::endl;

                                    }
                                    // convert to number and process selection
                                    toContinue = true;
                                    // call function based on selection
                                    switch (response)
                                    {
                                    case 1:
                                        if(updateStatus(mainSelection)) return true;
                                        break;
                                    case 2:
                                        if(updateDescription(mainSelection)) return true;
                                        break;
                                    case 3:
                                        if(updatePriority(mainSelection)) return true;
                                        break;
                                    case 4:
                                        updateRelease(mainSelection);
                                        break;
                                    // if they select back, exit this loop and go back to selecting change item
                                    case 0:
                                        toContinue = false;
                                        break;
                                    }
                                    // break out of this loop
                                    if (!toContinue)
                                    {
                                        break;
                                    }
                                    // once an update has been done, rerun the loop and ask for updates
                                    else
                                    {
                                        continue;
                                    }
                                }
                            }
                        }

                }
            
        }
    return false;
}



/*
a different printing format for when the user wishes to print an item inside a report 
*/
void itemReportPrint(int number, change_item& item){
    int value = item.status;
    int power = 0;
    while(value > 1){
        value /= 2;
        power++;
    }

    string states[] = {"unreviewed", "reviewed", "inProgress", "done", "cancelled"};
    char descTruncated[MAX_DESCRIPTION_SIZE];
    strncpy(descTruncated, item.description, MAX_DESCRIPTION_SIZE - 1);
    descTruncated[MAX_DESCRIPTION_SIZE - 1] = '\0'; // Ensure null-termination

    char prodTruncated[MAX_PRODUCT_NAME_SIZE];
    strncpy(prodTruncated, item.product, MAX_PRODUCT_NAME_SIZE - 1);
    prodTruncated[MAX_PRODUCT_NAME_SIZE - 1] = '\0'; // Ensure null-termination

    char relTruncated[MAX_RELEASE_ID_SIZE];
    strncpy(relTruncated, item.release, MAX_RELEASE_ID_SIZE - 1);
    relTruncated[MAX_RELEASE_ID_SIZE - 1] = '\0'; // Ensure null-termination


    int idSpace = 0;
    if(number <= 9){
        idSpace++;
    } 
    // Print formatted output
    if(number > 0){
        std::cout << "[" 
              << std::left << std::setw(1) << number << "] "
              << std::setw(idSpace) << ""
              << std::right << std::setw(6) <<item.id
              << std::left << std::setw(2) << ""
              << std::setw(MAX_DESCRIPTION_SIZE-1) << descTruncated
              << std::setw(2) << ""
              << std::setw(11) << states[power]
              << std::setw(2) << ""
              << std::setw(MAX_PRODUCT_NAME_SIZE) << prodTruncated << std::endl;
    }
    else{
        std::cout
        << std::right << std::setw(6) << item.id
        << std::left << std::setw(2) << ""
        << std::setw(MAX_DESCRIPTION_SIZE-1) << descTruncated
        << std::setw(2) << ""
        << std::setw(11) << states[power]
        << std::setw(2) << ""
        << std::setw(8) << (int)item.priority
        << std::setw(2) << ""
        << std::setw(MAX_RELEASE_ID_SIZE) << relTruncated << std::endl;
    }
}

/*
function that prints out all relevant items in a list , based on the filter
*/

int listItems(change_item& filter){
    change_item readInto;
    std::string selected;
    int entries = 0;
    bool fileEnded = false;
    bool processing = true;
    bool continues = false;
    ChangeItemDatabase::seekToBeginning();
    while(true){
    cout << endl << endl;
    std::cout << "Product: " << filter.product << std::endl;
    std::cout << "ID      Description                     Status       Priority  Release" << std::endl;
    while(!ChangeItemDatabase::getNext(readInto, filter)){
        if(continues){
        cout << endl << endl;
        std::cout << "Product: " << filter.product << std::endl;
        std::cout << "ID      Description                     Status       Priority  Release" << std::endl;
        }
        entries = 0;
        itemReportPrint(-1, readInto);
        entries++;

        while(!ChangeItemDatabase::getNext(readInto, filter) && entries < MAX_PRINTS){
            itemReportPrint(-1, readInto);
            entries++;
        }

        std::cout << "[0]  Back    [00] Back to Main Menu    " ;
        if(entries < MAX_PRINTS){
            std::cout << std::endl;
            fileEnded = true;
        }
        else{
            std::cout << "[C]  Next Page" << std::endl;
        }

        while(processing){
            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
            std::getline(std::cin, selected);

            if(selected == "00"){
                return -1;
            }
            else if (selected == "0"){
                return 0;
            }
            else if ((selected == "C"  || selected == "c" )&& !fileEnded){
                processing = false;
                continues = true;
            }
            else{
                std::cout << OPTION_NOT_AVAILABLE << std::endl;
            }
        }


    } 
        std::cout << "[0]  Back    [00] Back to Main Menu" << std::endl;
        while(true){
            std::cout << std::endl;
            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
            std::getline(std::cin, selected);
            if(selected == "00"){
                return -1;
            }
            else if (selected == "0"){
                return 0;
            }
            else{
                std::cout << OPTION_NOT_AVAILABLE << std::endl;
            }
        }
    }
}

//==================
// implementations of reports

/* this is a function that generates a report of users that need to be informed about a
specific change item.
precondition: none.
postcondition: none.
exceptions raised: throws an exception if the user enters an option that does not exist
during the input collection process.
returns: false to go back to main menu, true otherwise
*/
bool generateReportRequesters()
{
    // if module has been initialized
    if (initialized)
    {
        // get input
        while (1)
        {

            bool toContinue = true;
            // ask user to select a product from a list of products
            int selNind = selectProduct(false, true);
            int productSelected = static_cast<int8_t>(selNind & 0xFF);
            int arrayIndex = static_cast<int8_t>((selNind >> 8) & 0xFF);

            // back to main menu:
            if (productSelected == -1)
            {
                return true;
                // back:
            }
            else if (productSelected == 0)
            {
                return false;
            }
            // option selected:
            else
            {
                // get input
                while (1)
                {
                    // store product
                    toContinue = true;
                    product selectedProduct;
                    // Call select with added logging
                    Product::seekToBeginning();
                    Product::select(selectedProduct, productSelected, arrayIndex+1);

                    // create a change item filter to filter by product
                    change_item filter;
                    strncpy(filter.product, selectedProduct.name, MAX_PRODUCT_NAME_SIZE);

                    // prompt user to select from filtered change_items
                    change_item itemSelect;
                    selectItem(itemSelect, filter, true);
                    int itemSelected = itemSelect.id;

                    // process selection:
                    // main menu
                    if (itemSelected == -1)
                    {
                        return true;

                    } // back
                    else if (itemSelected == 0)
                    {
                        toContinue = false;
                        break;
                    }  // all other cases (if it is to be continue (not if they selected back))
                     while (toContinue)
                    {
                        // prompt user to confirm selected change_item
                        change_item selectedItem;
                        std::string confirmation;
                        selectedItem = itemSelect;
                        itemConfirmShow(selectedItem, true);
                        std::cout << "[y/n]:  ";
                        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clearing up the stream
                        std::getline(std::cin,confirmation);
                        // if they confrim : print it and rerun loop to show all change_items filtered by product again
                        if (confirmation == "Y" || confirmation == "y")
                        {
                            change_request filterId;
                            filterId.changeItemId = selectedItem.id;
                            int requestersSeen = listOfRequesters(filterId); // if they select to go back to the main menu, exit function
                            if (requestersSeen == -1)
                            {
                                return true;
                            }
                            toContinue = false;
                            // if they say no, show all filtered items again
                        }
                        else if (confirmation == "N" || confirmation == "n"){
                            toContinue = false;
                        } 
                        else{
                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                        }
    
                    }
                }
            }
        }
    }
    // back to main menu if not initialized
    else
    {
        return true;
    }
}

//==================

/* this is a function that generates a report of change items for a specific product that are unresolved
precondition: none.
postcondition: none.
exceptions raised: throws an exception if the user enters an option that does not exist
during the input collection process.
returns : false for going back to main menu, true otherwise
*/
bool generateReportItems()
{
     // only runs if the module has been initialized
    if (initialized)
    {
        // get input
        while (1)
        {
            // prompt user to select a product from a list of all products
            int selNind = selectProduct(false, false);
            int productSelected = static_cast<int8_t>(selNind & 0xFF);
            int arrayIndex = static_cast<int8_t>((selNind >> 8) & 0xFF);

            // processing selection

            // main menu
            if (productSelected == -1)
            {
                return false;
            }
            // back
            else if (productSelected == 0)
            {
                return true;
            }
            // all other cases (valid selection)
            else
            {
            // store selected product
            product selectedProduct;
            //Product::seekToBeginning();
            Product::select(selectedProduct, productSelected, arrayIndex+1);
            Product::seekToBeginning();
            // filter change items by it
            change_item filter;
            strncpy(filter.product, selectedProduct.name, MAX_PRODUCT_NAME_SIZE);

            // make sure it only shows ones that are not done or cancelled
            filter.status = (unreviewed | reviewed | inProgress);
            int listResponse = listItems(filter);

            if(listResponse == -1){
                return true;
            }    
            // only other case is 0, so we loop again. 

            }
    }
    return false;
    }
    return true;
}


/*
function that displays all change items and allow user to query any that they wish to
returns true if they want to go back to main menu and false if they want to go back
*/
bool queryItemControl(){

    while(true){
        // create two blank change items, one to filter and one to store
        change_item filter;
        change_item basic;
        selectItem(basic, filter, false); // store the selected item in basic.
        if(basic.id == 0){  // back
            return false;
        } 
        else if (basic.id == -1){ // back to main menu
            return true;
        }
        else{  // valid change item selected
            while(true){
            std::string selection;
            itemConfirmShow(basic, false);
                while(true){
                    std::cout << std::endl;
                    std::getline(std::cin, selection);
                    if(selection == "00"){
                        return true;        // back to main menu
                    } 
                    else if(selection == "0"){
                        break;
                    }
                    else{
                        std::cout << OPTION_NOT_AVAILABLE << std::endl;
                    }
                }
                if(selection == "0") break;
            }

        }
    }

}

//==================
// implementation of product, and product release management

/* prompts the user for inputs to create a product and calls Product::addProduct() to
carries out the process to add a new product to the database
only allows unique entries 
*/
bool addProductControl()
{
    RequesterDatabase::seekToBeginning();
    ChangeItemDatabase::seekToBeginning();
    ChangeRequestDatabase::seekToBeginning();
    Product::seekToBeginning();
    Release::seekToBeginning();
    // variables for control abstraction
    enum steps{InputProductName, Confirm, Complete};
    steps step = InputProductName;
    int returnFlag;

    // containers for user input and menu items
    char productName[MAX_PRODUCT_NAME_SIZE];
    product newProduct;
    product tempProduct;

    // control structure for menu tree
    // whenever user completes a menu evaluates and deliver user to the next desired menu
    // exits after completion menu or if user chooses to go back to main menu
    while (1)
    {
        switch (step)
        {
        case (InputProductName):
            Product::seekToBeginning();
            cout << endl << endl;
            cout << "Add a Product:" << endl;
            cout << "Define Product Name:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(1-10 alphanumeric characters)" << endl;
            cout << "Enter Product name:" << endl;

            // get user input
            returnFlag = getStringInput(MAX_PRODUCT_NAME_SIZE, productName); 
            if (returnFlag == -1) // if user selects back exit to previous menu
            {
                return 0; // complete without returning to main menu
            }

            // check for uniqueness
            strncpy(newProduct.name, productName, MAX_PRODUCT_NAME_SIZE);
            if (Product::getNext(tempProduct, newProduct) == 0) // if we find an element with same key in database
            {
                if (strlen(productName) != 0)
                {
                cout << PRODUCT_UNIQUENESS_ERROR << endl; // encounter error
                returnFlag = 0;
                }
            }
            
            if (returnFlag == 0) // if error encountered
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0)// if n return to main menu
                    {
                        step = Complete;
                        break;
                    }
                }
                break;
            }
            else // input is good and we can continue to next step
            {
                step = Confirm;
                break;
            }     
            break;

        case (Confirm):
            Product::seekToBeginning();
            cout << endl << endl;
            cout << "Would you like to create the product \"" << productName << "\"?" << endl <<
            "[y/n] : ";
            returnFlag = getYNInput(); // prompt for y/n repsonse

            if (returnFlag == 1) // if y save release and complete process
            {
                if (Product::writeProduct(newProduct) == 1) // save element
                {
                    // if write fails
                    cout << SAVE_ERROR << endl; // encounter error
                    while (1) // handle error
                    {
                        returnFlag = getYNInput(); // prompt for y/n repsonse
                        if (returnFlag == 1) // if y repeat current step
                        {
                            break; 
                        }
                        else if (returnFlag == 0)// if n return to main menu
                        {
                            step = Complete;
                        }
                    }
                    break;
                }
                else
                {
                    cout << endl << "Product Added" << endl << endl;
                    step = Complete;
                }
                break;
            }
            else if (returnFlag == 0)// if n return to main menu
            {
                step = Complete;
            }
            break;

        case (Complete):
            cout << endl << endl;
            Product::seekToBeginning();
            return 1;
        }
    }
}

//==================

/* prompts the user for inputs to create a product release
carries out the process to add a new product to the database
only allows unique entries 
*/

bool addReleaseControl()
{
    RequesterDatabase::seekToBeginning();
    ChangeItemDatabase::seekToBeginning();
    ChangeRequestDatabase::seekToBeginning();
    Product::seekToBeginning();
    Release::seekToBeginning();
    // variables for control abstraction
    enum steps{ChooseProduct, InputReleaseName, InputReleaseDate, Confirm, Complete};
    steps step = ChooseProduct;
    int menuIndex;
    int returnFlag;
    bool nextPageLegal;

    // containers for user input and menu items
    int userSelection;
    product relatedProduct;
    product tempProduct;
    release newRelease;
    release tempRelease;
    char releaseID[MAX_RELEASE_ID_SIZE];
    char releaseDate[DATE_SIZE];

    // control structure for menu tree
    // whenever user completes a menu evaluates and deliver user to the next desired menu
    // exits after completion menu or if user chooses to go back to main menu
    while (1)
    {
        switch (step)
        {
        case (ChooseProduct):
            nextPageLegal = 0;
            menuIndex = 0;
            cout << endl << endl;
            cout << "Add a Product Release:" << endl;
            cout << "Add a Release for Which Product?:" << endl;
            cout << "     Product Name:" << endl;
            // prints menu
            while (1)
            {
                // print list of products. on failure to print, or filling a page, get user input
                if (Product::getNext(tempProduct) == 0)
                {
                    menuIndex++;
                    printIndex(menuIndex);
                    printf("%-*s\n", MAX_PRODUCT_NAME_SIZE-1, tempProduct.name);
                }
                else // failed to get an element
                {
                    cout << "[0]  Back" << endl;
                    break;
                }
                if (menuIndex == 16) // page is full
                {
                    cout << "[0]  Back    [C]  Next Page" << endl;
                    nextPageLegal = 1; // next page is a legal menu option for user selection
                    break;
                }
            }

            // collect valid input
            userSelection = getIndexInput(menuIndex);
            if (userSelection >= 1) // valid input
            {
                // able to continue to next step
                Product::seekToBeginning();
                Product::select(relatedProduct, userSelection, menuIndex);
                strncpy(newRelease.name, relatedProduct.name, MAX_PRODUCT_NAME_SIZE); // save foreign key linking product and release
                step = InputReleaseName;
                Product::seekToBeginning(); // clean up for future calls
                break;
            }
            else if (userSelection < 1) // special cases
            {
                if (userSelection == -1) // user selected back return to previous menu
                {
                    Product::seekToBeginning(); // clean up for future calls
                    return 0;
                }
                if ((userSelection == -3) && nextPageLegal)
                {
                    // restart this step printing a new page of the menu
                    break;
                }
            }

            // if invalid input, reset menu
            cout << OPTION_NOT_AVAILABLE << endl;
            Product::seekToBeginning();
            break;

        case (InputReleaseName):
            Release::seekToBeginning();
            cout << endl << endl;
            cout << "Add a Product Release:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(1-8 alphanumeric characters)" << endl;
            cout << "Enter Release name:" << endl;

            // get user input
            returnFlag = getStringInput(MAX_RELEASE_ID_SIZE, releaseID);
            if (returnFlag == -1) // user selected back return to previous menu
            {
            step = ChooseProduct;
            break;
            }
 
            // check uniqueness
            strncpy(newRelease.releaseId, releaseID, MAX_RELEASE_ID_SIZE);
            if (Release::getNext(tempRelease, newRelease) == 0) // if we find an element with same name in database
            {
                if (strlen(releaseID) != 0)
                {
                cout << PRODUCT_RELEASE_UNIQUENESS_ERROR << endl; // encounter error
                returnFlag = 0;
                }
            }

            if (returnFlag == 0) // if error encountered
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0)// if n return to main menu
                    {
                        step = Complete;
                    }
                }
                break;
            }
            else //input is good and we can continue to next step
            {
                step = InputReleaseDate;
                break;
            }     
            break;

        case (InputReleaseDate):
            cout << endl << endl;
            cout << "Add a Product Release:" << endl;
            cout << "Get Release Date:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(YYYY-MM-DD: Y, M, D, are numeric characters, include dashes)" << endl;
            cout << "Enter Date:" << endl;

            returnFlag = getStringInput(DATE_SIZE, releaseDate);
            if (returnFlag == -1) // user selected back return to previous menu
            {
                step = InputReleaseName;
                break;
            }
            if (returnFlag != 0) // if no error encountered check formatting
            {
                 for (int i = 0; i < DATE_SIZE - 1; i++) // assert date is formatted correctly
                {
                    if ((i == 4) || (i == 7))
                    {
                        if (releaseDate[i] != '-')
                        {
                            cout << INCORRECT_CHARACTER_TYPE << endl;
                            returnFlag = 0;
                        }
                    }
                    else
                    {
                        if (!isdigit(releaseDate[i]))
                        {
                            cout << INCORRECT_CHARACTER_TYPE << endl;
                            returnFlag = 0;
                        }
                    }
                }
            }
            if (returnFlag == 0) // if error encountered
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0)// if n return to main menu
                    {
                        step = Complete;
                    }
                }
                break;
            }
            else
            {
                strncpy(newRelease.date, releaseDate, DATE_SIZE);
                step = Confirm;
                break;
            }
            break;

        case (Confirm):
            Release::seekToBeginning();
            cout << endl << endl;
            cout << "Would you like to create the \"" << relatedProduct.name << "\" release \"" << newRelease.releaseId << "\" with the release date: "
            << newRelease.date << endl << "[y/n] : ";
            returnFlag = getYNInput(); // prompt for y/n repsonse

            if (returnFlag == 1) // if y save release and complete program
            {
                if (Release::writeRelease(newRelease) == 1) // save element
                {
                    // if write fails
                    cout << SAVE_ERROR << endl; // encounter error
                    while (1) // handle errors
                    {
                        returnFlag = getYNInput(); // prompt for y/n repsonse
                        if (returnFlag == 1) // if y repeat current step
                        {
                            break; 
                        }
                        else if (returnFlag == 0)// if n return to main menu
                        {
                            step = Complete;
                            break;
                        }
                    }
                    break;
                }
                else
                {
                    cout << endl << "Release Added" << endl << endl;
                    step = Complete;
                    break;
                }
            }
            else if (returnFlag == 0)// if n return to main menu
            {
                step = Complete;
                break;
            }
            break;

        case (Complete): // process completed successfully
            Product::seekToBeginning();
            Release::seekToBeginning();
            return 1;
        }
    }
}

//==================

/* prompts the user for inputs to create a request
prompts for and creates a new requester if needed
requesters must be unique
prompts for and creates a new item if needed
carries out the process to add a new request
*/
bool addRequestControl()
{
    RequesterDatabase::seekToBeginning();
    ChangeItemDatabase::seekToBeginning();
    ChangeRequestDatabase::seekToBeginning();
    Product::seekToBeginning();
    Release::seekToBeginning();
    // variables for control abstraction
    enum steps{GetRequester, InputRequesterName, InputRequesterPhone, InputRequesterEmail,
    DepartmentOptionality, InputRequesterDepartment, ConfirmRequester, ChooseProduct, ChooseRelease,
    InputRequestDate, InputRequestDescription, ChooseChangeItem, InputPriority, Complete};
    steps step = GetRequester;
    int menuIndex;
    int returnFlag;
    bool nextPageLegal;

    // containers for user input and menu items
    int userSelection;
    product relatedProduct;
    product tempProduct;
    release relatedRelease;
    release tempRelease;
    requester relatedRequester;
    requester tempRequester;
    change_request newRequest;
    change_request requestFilter;
    change_item newItem;
    change_item pairedItem;
    change_item tempItem;
    bool savedItem = false;
    bool savedRequest = false;

    // input collection to save to new objects
    char requesterName[MAX_REQUESTER_NAME_SIZE];
    char requesterMail[MAX_EMAIL_SIZE];
    char requesterPhone[PHONE_NUMBER_SIZE];
    char requesterDepartment[MAX_DEPARTMENT_SIZE];
    char requestDate[DATE_SIZE];
    char requestDescription[MAX_DESCRIPTION_SIZE];

    // control structure for menu tree
    // whenever user completes a menu evaluates and deliver user to the next desired menu
    // exits after completion menu or if user chooses to go back to main menu
    while (1)
    {
        switch (step)
        {
        case (GetRequester):
            nextPageLegal = 0;
            menuIndex = 0;
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Select Requester:" << endl;
            cout << "     NAME                            Phone            Email" << endl;
            // prints list menu
            while (1)
            {
                // print list of requesters. on failure to print, or filling a page, get user input
                if (RequesterDatabase::getNext(tempRequester) == 0)
                {
                    menuIndex++;
                    printIndex(menuIndex);
                    printf("%-*s  ", MAX_REQUESTER_NAME_SIZE-1, tempRequester.name);
                    printPhone(tempRequester.phone);
                    printf("  %-*s\n", MAX_EMAIL_SIZE-1, tempRequester.email);
                }
                else // failed to get an element
                {
                    cout << "[0]  Back    [N]  Create New Requester" << endl;
                    break;
                }
                if (menuIndex == 16) // page is full
                {
                    cout << "[0]  Back    [C]  Next Page    [N] Create New Requester" << endl;
                    nextPageLegal = 1; // next page is a legal menu option for user selection
                    break;
                }
            }

            // get user input
            userSelection = getIndexInput(menuIndex);
            if (userSelection >= 1) // valid input
            {
                // able to continue to next step
                RequesterDatabase::seekToBeginning();
                RequesterDatabase::select(relatedRequester, userSelection, menuIndex);
                newRequest.requesterId = relatedRequester.requesterId; // save foreign key linking product and release
                step = ChooseProduct;
                RequesterDatabase::seekToBeginning(); // clean up for future calls
                break;
            }
            else if (userSelection < 1) // special cases
            {
                if (userSelection == -1) // user selected back return to previous menu
                {
                    RequesterDatabase::seekToBeginning(); // clean up for future calls
                    return 0;
                }
                if ((userSelection == -3) && nextPageLegal)
                {
                    // restart this step printing a new page of the menu
                    break;
                }
                if (userSelection == -4)
                {
                    // user selected create new 
                    step = InputRequesterName;
                }
            }

            // if invalid input, reset menu
            cout << OPTION_NOT_AVAILABLE << endl;
            RequesterDatabase::seekToBeginning();
            break;

        case (InputRequesterName):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Get Requester Name:" << endl; 
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(1-30 alphanumeric characters)" << endl;
            cout << "Enter Requester's Name:" << endl;

            returnFlag = getStringInput(MAX_REQUESTER_NAME_SIZE, requesterName);
            if (returnFlag == -1) // user selected back
            {
                step = GetRequester;
                break;
            }
            if (returnFlag == 0) // if error encountered
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0)// if n return to main menu
                    {
                        step = Complete;
                    }
                }
                break;
            }
            else // if data in is good, save and continue to next step
            {
                strncpy(relatedRequester.name, requesterName, MAX_REQUESTER_NAME_SIZE);
                step = InputRequesterPhone;
            }
            break;

        case (InputRequesterPhone):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Get Requester Phone Number:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(exactly 10 numeric characters)" << endl;
            cout << "Enter Requester's Phone Number" << endl;

            returnFlag = getStringInput(PHONE_NUMBER_SIZE, requesterPhone);
            if (returnFlag == -1) // user selected back
            {
                step = InputRequesterName;
                break;
            }
            if (returnFlag != 0) // if no error encountered verify string is correctly formatted
            {
                for (int i = 0; i < PHONE_NUMBER_SIZE - 1; i++)
                {
                    if (!isdigit(requesterPhone[i]))
                    {
                        cout << INCORRECT_CHARACTER_TYPE << endl; 
                        returnFlag = 0;
                    }
                }
            }
            
            if (returnFlag == 0) // if errors encountered
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0)// if n return to main menu
                    {
                        step = Complete;
                    }
                }
                break;
            }
            else // if data in is good, save and continue to next step
            {
                strncpy(relatedRequester.phone, requesterPhone, PHONE_NUMBER_SIZE);
                step = InputRequesterEmail;
            }
            break;

        case (InputRequesterEmail):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Get Requester Email:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(1-24 alphanumeric characters)" << endl;
            cout << "Enter Requester's Email:" << endl;

            returnFlag = getStringInput(MAX_EMAIL_SIZE, requesterMail);
            if (returnFlag == -1) // user selected back
            {
                step = InputRequesterPhone;
                break;
            }
            if (returnFlag == 0) // if errors encountered
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0)// if n return to main menu
                    {
                        step = Complete;
                    }
                }
                break;
            }
            else // if data in is good, save and continue to next step
            {
                strncpy(relatedRequester.email, requesterMail, MAX_EMAIL_SIZE);
                step = DepartmentOptionality;
            }
            break;

        case (DepartmentOptionality):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Get Department?:" << endl;
            cout << "[1]  No Department" << endl;
            cout << "[2]  Enter Department" << endl;
            cout << "[0]  Back" << endl;

            userSelection = makeSelection();

            switch (userSelection)
            {
            case ('1'): // no department
                step = ConfirmRequester;
                strncpy(relatedRequester.department, "", MAX_DEPARTMENT_SIZE);
                break;
            
            case ('2'): // enter department
                step = InputRequesterDepartment;
                break;

            case ('0'): // user selected back
                step = InputRequesterEmail; // return to previous menu
                break;

            default:
                cout << OPTION_NOT_AVAILABLE << endl;
                break;
            }
            break;

        case (InputRequesterDepartment):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Get Department:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements" << endl;
            cout << "(1-12 alphanumeric characters)" << endl;
            cout << "Enter Requester's Department:" << endl;

            returnFlag = getStringInput(MAX_DEPARTMENT_SIZE, requesterDepartment);
            if (returnFlag == -1) // user selected back
            {
                step = DepartmentOptionality;
                break;
            }
            if (returnFlag == 0) // if errors encountered
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0)// if n return to main menu
                    {
                        step = Complete;
                    }
                }
                break;
            }
            else // if data in is good, save and continue to next step
            {
                strncpy(relatedRequester.department, requesterDepartment, MAX_DEPARTMENT_SIZE);
                step = ConfirmRequester;
            }
            break;

        case (ConfirmRequester):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Confirm Requester Information:" << endl;
            cout << "Name:        " << relatedRequester.name << endl;
            cout << "Phone:       ";
            printPhone(relatedRequester.phone);
            cout << endl;
            cout << "EMAIL:       " << relatedRequester.email << endl;
            if (strncmp(requesterDepartment,"", MAX_DEPARTMENT_SIZE))
            {
            cout << "Department:  " << relatedRequester.department << endl;
            }

            // only repeat the yes no printout of this menu if user give bad input
            while (step == ConfirmRequester)
            {
                cout << endl << endl;
                cout << "Would you like to add the requester?" << endl;
                cout << "[y/n] : "; 

                returnFlag = getYNInput(); // prompt for y/n repsonse

                if (returnFlag == 1) // user confirms creation
                {
                    // check uniqueness
                    returnFlag = RequesterDatabase::getNext(tempRequester, relatedRequester);
                    RequesterDatabase::seekToBeginning();
                    if (returnFlag == 0) // requester already exists
                    {
                        cout << REQEUSTER_UNIQUENESS_ERROR << endl; // encounter error
                        while (1) // handle error
                        {
                            returnFlag = getYNInput(); // prompt for y/n repsonse
                            if (returnFlag == 1) // if y continue to next step with existing requester
                            {
                                newRequest.requesterId = tempRequester.requesterId; // get requesterId from existing element
                                step = ChooseProduct;
                                break;
                            }
                            else if (returnFlag == 0) // if n return to main menu
                            {
                                step = Complete;
                                break;
                            }
                        }
                        break;
                    }
                    else if (returnFlag == 1) // requester is new
                    {
                        // get Id number
                        relatedRequester.requesterId = RequesterDatabase::getRequesterCount() + 1;
                        newRequest.requesterId = relatedRequester.requesterId;
                        // save requester
                        returnFlag = RequesterDatabase::writeElement(relatedRequester);
                        if (returnFlag == 1) // if write failed
                        {
                            cout << SAVE_ERROR << endl;
                            while (1) // handle error
                            {
                                returnFlag = getYNInput(); // prompt for y/n repsonse
                                if (returnFlag == 1) // if y retry
                                {
                                    break;
                                }
                                else if (returnFlag == 0) // if n return to main menu
                                {
                                    step = Complete;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            cout << "Requester Added" << endl;
                            step = ChooseProduct;
                        }
                    }
                }
                else if (returnFlag == 0)// if user denies creation of requester
                {
                    step = GetRequester;
                    break;
                }
            }
            newRequest.requesterId = relatedRequester.requesterId; // save foreign key to new request
            break;

        case (ChooseProduct):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Select Product Needing Change:" << endl;
            cout << "     Product Name" << endl;

            nextPageLegal = 0;
            menuIndex = 0;
            // prints menu
            while (1)
            {
                // print list of products. on failure to print, or filling a page, get user input
                if (Product::getNext(tempProduct) == 0)
                {
                    menuIndex++;
                    printIndex(menuIndex);
                    printf("%-*s\n", MAX_PRODUCT_NAME_SIZE-1, tempProduct.name);
                }
                else // failed to get an element
                {
                    cout << "[0]  Back" << endl;
                    break;
                }
                if (menuIndex == 16) // page is full
                {
                    cout << "[0]  Back    [C]  Next Page" << endl;
                    nextPageLegal = 1; // next page is a legal menu option for user selection
                    break;
                }
            }

            // collect valid input
            userSelection = getIndexInput(menuIndex);
            if (userSelection >= 1) // valid input
            {
                // able to continue to next step
                Product::seekToBeginning();
                Product::select(relatedProduct, userSelection, menuIndex);
                strncpy(newItem.product, relatedProduct.name, MAX_PRODUCT_NAME_SIZE); // save foreign key linking item and product
                step = ChooseRelease;
                Product::seekToBeginning(); // clean up for future calls
                break;
            }
            else if (userSelection < 1) // special cases
            {
                if (userSelection == -1)
                {
                    // user selected back return to previous menu
                    Product::seekToBeginning(); // clean up for future calls
                    step = GetRequester;
                    break;
                }
                if ((userSelection == -3) && nextPageLegal)
                {
                    // restart this step printing a new page of the menu
                    break;
                }
            }

            // if invalid input, reset menu
            cout << OPTION_NOT_AVAILABLE << endl;
            Product::seekToBeginning();
            break;

        case (ChooseRelease):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Select Release Needing Change:" << endl;
            cout << "     Release Name" << endl;

            nextPageLegal = 0;
            menuIndex = 0;
            // prints menu
            while (1)
            {  
                release filter;
                strncpy(filter.name, relatedProduct.name, MAX_PRODUCT_NAME_SIZE);
                // print list of products. on failure to print, or filling a page, get user input
                if (Release::getNext(tempRelease, filter) == 0)
                {
                    menuIndex++;
                    printIndex(menuIndex);
                    printf("%-*s\n", MAX_RELEASE_ID_SIZE-1, tempRelease.releaseId);
                }
                else // failed to get an element
                {
                    cout << "[0]  Back" << endl;
                    break;
                }
                if (menuIndex == 16) // page is full
                {
                    cout << "[0]  Back    [C]  Next Page" << endl;
                    nextPageLegal = 1; // next page is a legal menu option for user selection
                    break;
                }
            }

            // collect valid input
            userSelection = getIndexInput(menuIndex);
            if (userSelection >= 1) // valid input
            {
                // able to continue to next step
                step = InputRequestDate;
                Release::seekToBeginning();
                Release::select(relatedRelease, userSelection, menuIndex);
                strncpy(newItem.release, relatedRelease.releaseId, MAX_RELEASE_ID_SIZE); // save foreign key linking item and product
                strncpy(newRequest.release, relatedRelease.releaseId, MAX_RELEASE_ID_SIZE); // save release change request is for 
                Release::seekToBeginning(); // clean up for future calls
                break;
            }
            else if (userSelection < 1) // special cases
            {
                if (userSelection == -1)
                {
                    // user selected back return to previous menu
                    Release::seekToBeginning(); // clean up for future calls
                    step = ChooseProduct;
                    break;
                }
                if ((userSelection == -3) && nextPageLegal) 
                {
                    // restart this step printing a new page of the menu
                    break;
                }
            }

            // if invalid input, reset menu
            cout << OPTION_NOT_AVAILABLE << endl;
            Release::seekToBeginning();
            break;

        case (InputRequestDate):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Get Date:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(YYYY-MM-DD: Y, M, D, are numeric characters, include dashes)" << endl;
            cout << "Enter Date:" << endl;

            returnFlag = getStringInput(DATE_SIZE, requestDate);
            if (returnFlag == -1) // user selected back return to previous menu
            {
                step = ChooseRelease;
                break;
            }
            if (returnFlag != 0) // if no error encountered check formatting
            {
                 for (int i = 0; i < DATE_SIZE - 1; i++) // assert date is formatted correctly
                {
                    if ((i == 4) || (i == 7))
                    {
                        if (requestDate[i] != '-')
                        {
                            cout << INCORRECT_CHARACTER_TYPE << endl;
                            returnFlag = 0;
                        }
                    }
                    else
                    {
                        if (!isdigit(requestDate[i]))
                        {
                            cout << INCORRECT_CHARACTER_TYPE << endl;
                            returnFlag = 0;
                        }
                    }
                }
            }
            if (returnFlag == 0) // if encountered errors
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0) // if n return to main menu
                    {
                        step = Complete;
                        break;
                    }
                }
            }
            else // if input is good
            {
                strncpy(newRequest.requestDate, requestDate, DATE_SIZE); // save data
                step = InputRequestDescription; // continue to next step
            }
            break;

        case (InputRequestDescription):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Get Description:" << endl;
            cout << "[0]  Back" << endl;
            cout << endl;
            cout << "Field Requirements:" << endl;
            cout << "(1-30 alphanumeric characters)" << endl;
            cout << "Enter Description:" << endl;

            returnFlag = getStringInput(MAX_DESCRIPTION_SIZE, requestDescription);
            if (returnFlag == -1) // user selected back return to previous menu
            {
                step = InputRequestDate;
                break;
            }

            if (returnFlag == 0) // if encountered errors
            {
                while (1) // handle errors
                {
                    returnFlag = getYNInput(); // prompt for y/n repsonse
                    if (returnFlag == 1) // if y repeat current step
                    {
                        break; 
                    }
                    else if (returnFlag == 0) // if n return to main menu
                    {
                        step = Complete;
                        break;
                    }
                }
            }
            else // if input is good
            {
                strncpy(newItem.description, requestDescription, MAX_DESCRIPTION_SIZE); // save data
                step = ChooseChangeItem; // continue to next step
            }
            break;

        case (ChooseChangeItem):
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Does a Change Item described by \"" << requestDescription << "\" Exist?:" << endl;
            cout << "     Description                     ID" << endl;

            nextPageLegal = 0;
            menuIndex = 0;
            // prints menu
            while (1)
            {
                change_item filter;
                strncpy(filter.product, relatedProduct.name, MAX_PRODUCT_NAME_SIZE);
                // print list of products. on failure to print, or filling a page, get user input
                if (ChangeItemDatabase::getNext(tempItem, filter) == 0)
                {
                    menuIndex++;
                    printIndex(menuIndex);
                    printf("%-*s  %*i\n", MAX_DESCRIPTION_SIZE-1, tempItem.description, 6, tempItem.id);
                }
                else // failed to get an element
                {
                    cout << "[0]  Back    [N]  Create New Change Item" << endl;
                    break;
                }
                if (menuIndex == 16) // page is full
                {
                    cout << "[0]  Back    [C]  Next Page    [N]  Create New Change Item" << endl;
                    nextPageLegal = 1; // next page is a legal menu option for user selection
                    break;
                }
            }

            // collect valid input
            userSelection = getIndexInput(menuIndex);
            if (userSelection >= 1) // valid input
            {
                // able to continue to next step
                ChangeItemDatabase::seekToBeginning();
                ChangeItemDatabase::select(pairedItem, userSelection, menuIndex);
                newRequest.changeItemId = pairedItem.id; // save foreign key of item to request
                ChangeItemDatabase::seekToBeginning(); // clean up for future calls

                // makes sure requester has not already requested this change
                requestFilter.changeItemId = pairedItem.id;
                requestFilter.requesterId = newRequest.requesterId;
                change_request tempRequest;
                ChangeRequestDatabase::seekToBeginning();
                if (ChangeRequestDatabase::getNext(tempRequest, requestFilter) == 0)
                {
                    // request is a duplicate, abort process
                    cout << "This requester has already requested this change." << endl;
                    cout << "Press enter to return to the main menu." << endl;
                    getYNInput();
                    step = Complete;
                    break;
                }
                ChangeRequestDatabase::seekToBeginning();


                if (ChangeRequestDatabase::writeElement(newRequest) == 1) // save element
                {
                    // if write fails
                    cout << SAVE_ERROR << endl; // encounter error
                    while (1) // handle error
                    {
                        returnFlag = getYNInput(); // prompt for y/n repsonse
                        if (returnFlag == 1) // if y repeat current step
                        {
                            ChangeItemDatabase::seekToBeginning();
                            break; 
                        }
                        else if (returnFlag == 0)// if n return to main menu
                        {
                            step = Complete;
                            break;
                        }
                    }
                    break;
                }
                else
                {
                    cout << "Change Request Added" << endl;
                    step = Complete;
                }
                break;
            }
            else if (userSelection < 1) // special cases
            {
                if (userSelection == -1)
                {
                    // user selected back return to previous menu
                    ChangeItemDatabase::seekToBeginning(); // clean up for future calls
                    step = InputRequestDescription;
                    break;
                }
                if ((userSelection == -3) && nextPageLegal) 
                {
                    // restart this step printing a new page of the menu
                    break;
                }
                if (userSelection == -4) // user wants to create new change item
                {
                    step = InputPriority;
                    break;
                }
            }

            // if invalid input, reset menu
            cout << OPTION_NOT_AVAILABLE << endl;
            ChangeItemDatabase::seekToBeginning();
            break;

        case (InputPriority):
            ChangeItemDatabase::seekToBeginning();
            ChangeRequestDatabase::seekToBeginning();
            cout << endl << endl;
            cout << "Handle Change Request:" << endl;
            cout << "Enter Priority of Change:" << endl;
            cout << "[1]  Lowest" << endl;
            cout << "[2]  Low" << endl;
            cout << "[3]  Middle" << endl;
            cout << "[4]  High" << endl;
            cout << "[5]  Highest" << endl;
            cout << "[0]  Back" << endl;
            
            userSelection = makeSelection();
            returnFlag = 0; // used for error checking

            if (userSelection == '0') // user selected back return to previous menu 
            {
                step = ChooseChangeItem;
                break;
            }

            switch (userSelection)
            {
            case '1':
                newItem.priority = lowest;
                break;
            case '2':
                newItem.priority = low;
                break;
            case '3':
                newItem.priority = middle;
                break;
            case '4':
                newItem.priority = high;
                break;
            case '5':
                newItem.priority = highest;
                break;
            default:
                returnFlag = 1; // invalid input
                cout << OPTION_NOT_AVAILABLE << endl;
                break;
            }

            if (returnFlag == 0) // try to save the new change item and request
            {
                if (!savedItem)
                {
                    if (ChangeItemDatabase::writeElement(newItem) == 1) // save element
                    {
                        // if write fails
                        cout << SAVE_ERROR << endl; // encounter error
                        while (1) // handle error
                        {
                            returnFlag = getYNInput(); // prompt for y/n repsonse
                            if (returnFlag == 1) // if y repeat current step
                            {
                                break; 
                            }
                            else if (returnFlag == 0)// if n return to main menu
                            {
                                step = Complete;
                                break;
                            }
                        }
                    }
                    else
                    {
                        cout << "Change Item Added" << endl;
                        savedItem = true;
                    }
                }

                if (savedItem && ! savedRequest)
                {
                    newRequest.changeItemId = ChangeItemDatabase::getChangeItemCount();
                    if (ChangeRequestDatabase::writeElement(newRequest) == 1) // save element
                    {
                        // if write fails
                        cout << SAVE_ERROR << endl; // encounter error
                        while (1) // handle error
                        {
                            returnFlag = getYNInput(); // prompt for y/n repsonse
                            if (returnFlag == 1) // if y repeat current step
                            {
                                break; 
                            }
                            else if (returnFlag == 0)// if n return to main menu
                            {
                                step = Complete;
                                break;
                            }
                        }
                    }
                    else
                    {
                        cout << "Change Request Added" << endl;
                        savedRequest = true;
                    }
                }

                if (savedItem && savedRequest)
                {
                    // completed successfully
                    step = Complete;
                }
            }
            break;

        case (Complete):
            cout << endl << endl;
            // cleanup and exit
            ChangeItemDatabase::seekToBeginning();
            ChangeRequestDatabase::seekToBeginning();
            RequesterDatabase::seekToBeginning();
            Product::seekToBeginning();
            Release::seekToBeginning();
            return 1;
        default:
            break;
        }
    }
}

//==================
// implementation of helper functions for product management and handling a request

// prints the index formatted as "[#]  " or "[##] " based on numbr of characters needed to print number
void printIndex(int index)
{
    cout << "[" << index << "]";
    if (index < 10)
    {
        cout << "  ";  
    }
    else
    {
        cout << " ";
    }
    return;
}

//========

// formatting rules for printing a phone number
void printPhone(char* phone)
{
    cout << "+1 "; 
    // iterate through the numbers in the phone number and print hyphens where needed
    for (int i = 0; i < PHONE_SIZE; i++)
    {
        cout << phone [i];
        if ((i == 2) || (i == 5))
        {
            cout << "-";
        }
    }
    return;
}

//========

// get an integer hashing to user text input
// implemented by collecting and judging a string
int makeSelection()
{
    string selection;
    int inputLength;
    getline(cin, selection);
    inputLength = selection.length();

    switch (inputLength)
    {
    case 1:
        return selection[0];
        break;
    case 2:
        return selection[0]*256 + selection[1];
        break;
    default:
        return -1;
        break;
    }
}

//========

// return value from [1, menu count]
// or a flag indicating some special selections
// implemented by getting string input and judging it
int getIndexInput(int menuCount)
{
    string input;
    getline(cin, input);
    int inLen = input.length();

    //first check for special inputs
    if (!strcmp(input.c_str(), "0"))
    {
        return -1;
    }
    if (!strcmp(input.c_str(), "00"))
    {
        return -2;
    }
    if (inLen == 1)
    {
        char firstChar = input[0]; 
        if ((firstChar == 'C') || (firstChar == 'c'))
        {
            return -3;
        }
        else if ((firstChar == 'N') || (firstChar == 'n'))
        {
            return -4;
        }
    }

    // check validity of input
    if (inLen == 0)
    {
        return 0; // input is garbage
    }
    // check each character input
    for (int i = 0; i < inLen; i++)
    {
        if (i > 2) // if user input more than two characters input is garbage
        {
            return 0; // input is garbage
        }
        if (!isdigit(input[i])) // if user submitted non numeric character input is garbage
        {
            return 0; // input is garbage
        }
    }
    // we are guaranteed input is an integer from 1 - 99
    int selection = atoi(input.c_str());
    // is input in the range of the menu options?
    if (selection > menuCount)
    {
        return 0; // input is garbage
    }
    // else input is good
    return selection;
}

// function to collect a yes or no response
// implemented by getting string input and judging it
int getYNInput()
{
    string answer;
    // get input
    getline(cin, answer);
    int inLen = answer.length();

    // check yes or no
    if ((inLen == 1) && ((answer[0] == 'y') || (answer[0] == 'Y')))
    {
        return 1;
    }

    if ((inLen == 1) && ((answer[0] == 'n') || (answer[0] == 'N')))
    {
        return 0;
    }

    // if neither return error flag
    return -1;
}

int getStringInput(const int MAX_SIZE, char* readInto)
{
    string line;
    // get input
    getline(cin, line);
    int inLen = line.length();
    // check for special cases
    if ((inLen == 1) && (line[0] == '0')) // if user selected back
    {
        return -1;
    }
    // check length is legal
    if (inLen > MAX_SIZE-1)
    {
        cout << EXCEEDS_BOUNDS << endl;
        return 0;
    }
    if (inLen < 1)
    {
        cout << BELOW_BOUNDS << endl;
        return 0;
    }
    strncpy(readInto, line.c_str(), MAX_SIZE);
    return 1;
}
