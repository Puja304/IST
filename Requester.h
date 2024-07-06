/* Requester.h
description:
This is the module for maintenance of requester objects.
version history:
ver1 -24/07/02, original by Wah Paw Hser and Puja Shah
*/

//==================

#include <iostream>
#include <fstream>

//==================

const int MAX_REQUESTER_NAME_SIZE = 30; // max length for requester name
const int PHONE_NUMBER_SIZE = 10;       // length of phone number
const int MAX_EMAIL_SIZE = 24;          // max length for email address
const int MAX_DEPARTMENT_SIZE = 12;     // max length for department name

//==================

class Requester {
public:
    Requester(
        /* description: name of the requester
        used for: input */
        const char* name,
        /* description: phone number of the requester
        used for: input */
        const char* phone,
        /* description: email address of the requester
        used for: input */
        const char* email,
        /* description: department of the requester
        used for: input */
        const char* department
    );
    /* constructs a Requester object using the given name, phone, email and department.
    preconditions: 'name' must be 1-30 chars.
                   'phone' must be exactly 10 numerical chars.
                   'email' must be 1-24 chars.
                   'department' must be 1-12 chars.
    postcondition: a Requester object is successfully created.
    exceptions raised: throws an exception if 'name', 'phone', 'email, or 'department'
                       do not meet the correct format.
    */

    ~Requester();
    /* destructor for Requester objects.
    precondition: none.
    postcondition: the Requester resources are cleaned up.
    exceptions raised: none.
    */
    
    void initRequester();
    /* initializes the Requester module
    precondition: none.
    postcondition: the Requester module is initialized.
    exceptions raised: none.
    */
    
    //==================

    void uninitRequester();
    /* cleans up the Requester module to uninitialize.
    precondition: the Requester module must already be initialized.
    postcondition: the Requester module is uninitialized.
    exceptions raised: none.
    */

    int getRequesterId() const;
    /* this function returns the requester ID of the requester.
    precondition: none.
    postcondition: the function returns the requester ID of the requester.
    exceptions raised: none.
    */
    
    //==================
    
    const char* getName() const;
    /* returns the name of the requester
    preconditions: none
    postconditions: the name remains unchanged
    */ 
    
    const char* getPhone() const;
    /* returns the requester's phone number
    preconditions: none
    postconditions: the phone number remains unchanged
    */ 
        
    const char* getEmail() const;
    /* returns the requester's email
    preconditions: none
    postconditions: the email remains unchanged
    */     
    
    const char* getDepartment() const;
    /* returns the requester's department
    preconditions: none
    postconditions: the department remains unchanged
    */ 
    
    void showRequester() const;
    /* shows the details of the requester to the user.
    precondition: none.
    postcondition: the requester details remain unchanged.
    exceptions raised: none.
    */

    //==================

    static void openRequesterFile(
        /* description: the mode that the file is opened in
        (std::ios::in for writing) and (std::ios::out for reading)
        used for: input */
        std::ios_base::openmode mode
    );
    /* this function opens the requester file in the specified 'mode'.
    precondition: none.
    postcondition: the requester file is opened in the specified 'mode'.
    exceptions raised: throws an exception if the file does not open.
    */
    
    static void writeRequester(
        /* description: the requester object that is to be written by this function
        used for: input */
        const Requester& requester
    );
    /* writes a requester object to the requester file.
    precondition: the requester file must be open in 'std::ios::out' mode for writability
    postcondition: the requester object is written.
    exceptions raised: throws an exception if file is not open in 'std::ios::out' mode for writability
    */

    static bool getNext(
        /* description: a reference to the requester object to be read
        used for: output */
        Requester& requester
    );
    /* reads the next requester object from the file. returns true, if read successfully.
    precondition: the requester file must be open in 'std::ios::in' mode for readability
    postcondition: the next requester object is read.
    exceptions raised: throws an exception if file is not open in 'std::ios::in' mode for readability
    */

    static void closeRequesterFile();
    /* this function closes the requester file.
    precondition: the requester file must already be open.
    postcondition: the requester file is closed,
    exceptions raised: throws an exception if the requester file is not already be open.
    */

    static void seekToBeginningOfFile();
    /* postion in the file goes back to the beginning of the requester file.
    precondition: the requester file must already be open.
    postcondition: postion in the file goes back to the beginning of the requester file.
    exceptions raised: throws an exception if the requester file is not already be open.
    */

private:
    char name[MAX_REQUESTER_NAME_SIZE];     // name of requester
    char phone[PHONE_NUMBER_SIZE];          // phone number of requester
    char email[MAX_EMAIL_SIZE];             // email of requester
    char department[MAX_DEPARTMENT_SIZE];   // department of requester
    int requesterId;                        // requester ID
    static int nextRequesterId;             // the next requester ID to be assigned

    static std::fstream requesterFile;      // the file stream for the Requester file
};

//==================

Requester addRequester(
    /* description: name of the requester
    used for: input */
    const char* name,
    /* description: phone number of the requester
    used for: input */
    const char* phone,
    /* description: email address of the requester
    used for: input */
    const char* email,
    /* description: department of the requester
    used for: input */
    const char* department
);
/* creates a Requester object using the given name, phone, email and department
and returns the Requester.
preconditions: 'name' must be 1-30 chars.
                'phone' must be exactly 10 numerical chars.
                'email' must be 1-24 chars.
                'department' must be 1-12 chars.
postcondition: a Requester object is successfully created.
exceptions raised: throws an exception if 'name', 'phone', 'email, or 'department'
                   do not meet the correct format.
*/
