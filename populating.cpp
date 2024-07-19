#include "ScenarioControl.h"
#include <cstring>
#include <iostream>
#include <iomanip>


// g++ populating.cpp ScenarioControl.cpp Requester.cpp Change.cpp Product.cpp -o tried


const char** populateProducts(const char** productNames) {
    for (int i = 0; i < 5; ++i) {
        product prod;
        strncpy(prod.name, productNames[i], MAX_PRODUCT_NAME_SIZE);
        Product::writeProduct(prod);
    }
    return productNames;
}

void populateReleases(const char** productNames) {
    const char* releaseDates[] = {"2024-07-01", "2024-07-02", "2024-07-03", "2024-07-04", "2024-07-05",
                                  "2024-07-06", "2024-07-07", "2024-07-08", "2024-07-09", "2024-07-10",
                                  "2024-07-11", "2024-07-12", "2024-07-13", "2024-07-14", "2024-07-15", "2024-07-16"};
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            release rel;
            strncpy(rel.name, productNames[i], MAX_PRODUCT_NAME_SIZE);
            strncpy(rel.date, releaseDates[(i * 3) + j], RELEASE_DATE_SIZE);
            snprintf(rel.releaseId, MAX_RELEASE_ID_SIZE, "R%05d", (i * 3) + j + 1);
            Release::writeRelease(rel);
        }
    }
}


void populateRequesters() {
    const char* requesterNames[] = {"Alice", "Bob", "Charlie", "David", "Eve",
                                    "Frank", "Grace", "Hank", "Ivy", "Jack"};
    const char* phoneNumbers[] = {"123-456-7890", "234-567-8901", "345-678-9012", "456-789-0123", "567-890-1234",
                                  "678-901-2345", "789-012-3456", "890-123-4567", "901-234-5678", "012-345-6789"};
    const char* emails[] = {"alice@example.com", "bob@example.com", "charlie@example.com", "david@example.com", "eve@example.com",
                            "frank@example.com", "grace@example.com", "hank@example.com", "ivy@example.com", "jack@example.com"};
    const char* departments[] = {"DeptA", "DeptB", "DeptC", "DeptD", "DeptE",
                                 "DeptF", "DeptG", "DeptH", "DeptI", "DeptJ"};
    for (int i = 0; i < 10; ++i) {
        requester req;
        strncpy(req.name, requesterNames[i], MAX_REQUESTER_NAME_SIZE);
        strncpy(req.phone, phoneNumbers[i], PHONE_NUMBER_SIZE);
        strncpy(req.email, emails[i], MAX_EMAIL_SIZE);
        strncpy(req.department, departments[i], MAX_DEPARTMENT_SIZE);
        req.requesterId = i + 1;
        RequesterDatabase::writeElement(req);
    }
}

void populateChangeRequests() {
    for (int i = 0; i < 30; ++i) {
        change_request req;
        req.changeItemId = i + 1;
        req.requesterId = (i % 10) + 1; // Cycle through requesters
        snprintf(req.requestDate, DATE_SIZE+1, "2024-07-%02d", (i % 16) + 1);
        snprintf(req.release, MAX_RELEASE_ID_SIZE, "R%05d", (i % 16) + 1);
        ChangeRequestDatabase::writeElement(req);
    }
}


void populateChangeItems(const char** productNames) {
    for (int i = 0; i < 30; ++i) {
        change_item item;
        item.id = i + 1;
        item.status = (ChangeItemStates) 1 << i;
        item.priority = i%5;
        strncpy(item.product, productNames[i % 5], MAX_PRODUCT_NAME_SIZE);
        snprintf(item.release, MAX_RELEASE_ID_SIZE, "R%05d", (i % 16) + 1);
        snprintf(item.description, MAX_DESCRIPTION_SIZE, "Change request %d", i + 1);
        ChangeItemDatabase::writeElement(item);
    }
}


int main() {
    initControl();
    const char* productNames[] = {"ProductA", "ProductB", "ProductC", "ProductD", "ProductE"};
    populateProducts(productNames);
    populateReleases(productNames);
    populateRequesters();
    populateChangeRequests();
    populateChangeItems(productNames);
    uninitControl();

    return 0;
}



