/*
 * ============================================================
 *  Community Help Desk and Resource Allocation System
 *  Course  : Programming in C  (DBT7288)
 *  Author  : <Your Name>
 *  Date    : 2026
 * ============================================================
 *
 *  Modules
 *  -------
 *  1. Beneficiary Management
 *  2. Volunteer Management
 *  3. Resource / Inventory Management
 *  4. Resource Allocation
 *  5. Search Records
 *  6. Reports
 *  7. Save Data
 *  8. Exit
 *
 *  Memory: malloc / calloc used; realloc NOT used.
 *  Files : beneficiaries.txt, volunteers.txt,
 *          resources.txt, allocations.txt
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ──────────────────────────── CONSTANTS ───────────────────────────── */
#define MAX_NAME       50
#define MAX_CATEGORY   30
#define MAX_SKILL      30
#define MAX_DATE       15
#define MAX_CONTACT    15
#define MAX_ADDRESS    80
#define MAX_PHONE      15

#define FILE_BENEFICIARY  "beneficiaries.txt"
#define FILE_VOLUNTEER    "volunteers.txt"
#define FILE_RESOURCE     "resources.txt"
#define FILE_ALLOCATION   "allocations.txt"

/* ─────────────────────────── STRUCTURES ───────────────────────────── */

typedef struct {
    int  beneficiaryId;
    char name[MAX_NAME];
    char contact[MAX_CONTACT];
    char address[MAX_ADDRESS];
    char needCategory[MAX_CATEGORY];
    int  familySize;
    int  priorityLevel;          /* 1=urgent  2=normal  3=low */
} Beneficiary;

typedef struct {
    int  volunteerId;
    char name[MAX_NAME];
    char phone[MAX_PHONE];
    char skill[MAX_SKILL];
    int  isAvailable;            /* 1=available  0=not available */
} Volunteer;

typedef struct {
    int  itemId;
    char itemName[MAX_NAME];
    char category[MAX_CATEGORY];
    int  quantityAvailable;
    int  reorderLevel;
} Resource;

typedef struct {
    int  allocationId;
    int  beneficiaryId;
    int  itemId;
    int  requestedQty;
    int  allocatedQty;
    char allocationDate[MAX_DATE];
} Allocation;

/* ─────────────────────── GLOBAL DATA ARRAYS ───────────────────────── */

Beneficiary *beneficiaries = NULL;
int beneficiaryCount = 0;

Volunteer *volunteers = NULL;
int volunteerCount = 0;

Resource *resources = NULL;
int resourceCount = 0;

Allocation *allocations = NULL;
int allocationCount = 0;

/* ═══════════════════════════════════════════════════════════════════
   UTILITY HELPERS
   ═══════════════════════════════════════════════════════════════════ */

/* Flush leftover newline characters from stdin */
void flushInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Read a non-empty string; re-prompt if blank */
void readNonEmptyString(const char *prompt, char *buf, int maxLen) {
    do {
        printf("%s", prompt);
        fgets(buf, maxLen, stdin);
        buf[strcspn(buf, "\n")] = '\0';   /* strip newline */
        if (strlen(buf) == 0)
            printf("  [!] Input cannot be empty. Try again.\n");
    } while (strlen(buf) == 0);
}

/* Read a positive integer with validation */
int readPositiveInt(const char *prompt) {
    int v;
    char line[64];
    while (1) {
        printf("%s", prompt);
        fgets(line, sizeof(line), stdin);
        if (sscanf(line, "%d", &v) == 1 && v > 0)
            return v;
        printf("  [!] Please enter a positive integer.\n");
    }
}

/* Read a non-negative integer */
int readNonNegInt(const char *prompt) {
    int v;
    char line[64];
    while (1) {
        printf("%s", prompt);
        fgets(line, sizeof(line), stdin);
        if (sscanf(line, "%d", &v) == 1 && v >= 0)
            return v;
        printf("  [!] Please enter a non-negative integer.\n");
    }
}

/* Read an integer in a closed range [lo, hi] */
int readIntInRange(const char *prompt, int lo, int hi) {
    int v;
    char line[64];
    while (1) {
        printf("%s", prompt);
        fgets(line, sizeof(line), stdin);
        if (sscanf(line, "%d", &v) == 1 && v >= lo && v <= hi)
            return v;
        printf("  [!] Enter a number between %d and %d.\n", lo, hi);
    }
}

/* Check whether a beneficiary ID already exists */
int beneficiaryIdExists(int id) {
    int i;
    for (i = 0; i < beneficiaryCount; i++)
        if (beneficiaries[i].beneficiaryId == id) return 1;
    return 0;
}

/* Check whether a volunteer ID already exists */
int volunteerIdExists(int id) {
    int i;
    for (i = 0; i < volunteerCount; i++)
        if (volunteers[i].volunteerId == id) return 1;
    return 0;
}

/* Check whether a resource ID already exists */
int resourceIdExists(int id) {
    int i;
    for (i = 0; i < resourceCount; i++)
        if (resources[i].itemId == id) return 1;
    return 0;
}

/* Check whether an allocation ID already exists */
int allocationIdExists(int id) {
    int i;
    for (i = 0; i < allocationCount; i++)
        if (allocations[i].allocationId == id) return 1;
    return 0;
}

/* Find index of beneficiary by ID (-1 if not found) */
int findBeneficiary(int id) {
    int i;
    for (i = 0; i < beneficiaryCount; i++)
        if (beneficiaries[i].beneficiaryId == id) return i;
    return -1;
}

/* Find index of volunteer by ID (-1 if not found) */
int findVolunteer(int id) {
    int i;
    for (i = 0; i < volunteerCount; i++)
        if (volunteers[i].volunteerId == id) return i;
    return -1;
}

/* Find index of resource by ID (-1 if not found) */
int findResource(int id) {
    int i;
    for (i = 0; i < resourceCount; i++)
        if (resources[i].itemId == id) return i;
    return -1;
}

/* Print a separator line */
void separator(void) {
    printf("-------------------------------------------------------------\n");
}

/* Print a bold section header */
void header(const char *title) {
    separator();
    printf("  %s\n", title);
    separator();
}

/* ═══════════════════════════════════════════════════════════════════
   DISPLAY HELPERS
   ═══════════════════════════════════════════════════════════════════ */

void printBeneficiary(Beneficiary *b) {
    const char *pstr = (b->priorityLevel == 1) ? "Urgent" :   // ternary operator / conditon operator 
                       (b->priorityLevel == 2) ? "Normal" : "Low";
    printf("  ID:%-5d  Name: %-25s  Category: %-15s\n",
           b->beneficiaryId, b->name, b->needCategory);
    printf("          Contact: %-15s  Family Size: %d   Priority: %s\n",
           b->contact, b->familySize, pstr);
    printf("          Address: %s\n", b->address);
}

void printVolunteer(Volunteer *v) {
    printf("  ID:%-5d  Name: %-25s  Skill: %-15s  Phone: %-12s  Available: %s\n",
           v->volunteerId, v->name, v->skill, v->phone,
           v->isAvailable ? "Yes" : "No");
}

void printResource(Resource *r) {
    printf("  ID:%-5d  %-25s  Category: %-15s  Qty: %-6d  Reorder: %d%s\n",
           r->itemId, r->itemName, r->category,
           r->quantityAvailable, r->reorderLevel,
           (r->quantityAvailable <= r->reorderLevel) ? "  [LOW STOCK]" : "");
}

void printAllocation(Allocation *a) {
    printf("  AllocID:%-5d  BenefID:%-5d  ItemID:%-5d  "
           "Requested:%-4d  Allocated:%-4d  Date:%s\n",
           a->allocationId, a->beneficiaryId, a->itemId,
           a->requestedQty, a->allocatedQty, a->allocationDate);
}

/* ═══════════════════════════════════════════════════════════════════
   MODULE 1 – BENEFICIARY MANAGEMENT
   ═══════════════════════════════════════════════════════════════════ */

void addBeneficiary(void) {
    header("ADD BENEFICIARY");
    Beneficiary b;

    /* unique ID */
    do {
        b.beneficiaryId = readPositiveInt("  Enter Beneficiary ID  : ");
        if (beneficiaryIdExists(b.beneficiaryId))
            printf("  [!] ID %d already exists. Choose a different ID.\n",
                   b.beneficiaryId);
    } while (beneficiaryIdExists(b.beneficiaryId));

    readNonEmptyString("  Enter Name           : ", b.name, MAX_NAME);
    readNonEmptyString("  Enter Contact        : ", b.contact, MAX_CONTACT);
    readNonEmptyString("  Enter Address        : ", b.address, MAX_ADDRESS);
    readNonEmptyString("  Enter Need Category  : ", b.needCategory, MAX_CATEGORY);
    b.familySize    = readPositiveInt("  Enter Family Size    : ");
    b.priorityLevel = readIntInRange("  Priority (1=Urgent 2=Normal 3=Low): ", 1, 3);

    /* grow array using realloc */
    Beneficiary *temp = (Beneficiary *)realloc(beneficiaries, (beneficiaryCount + 1) * sizeof(Beneficiary));
    if (!temp) { printf("  [!] Memory allocation failed.\n"); return; }
    beneficiaries = temp;
    beneficiaries[beneficiaryCount++] = b;

    printf("  [OK] Beneficiary added successfully.\n");
}

void displayAllBeneficiaries(void) {
    header("ALL BENEFICIARIES");
    if (beneficiaryCount == 0) { printf("  No records found.\n"); return; }
    int i;
    for (i = 0; i < beneficiaryCount; i++) {
        printBeneficiary(&beneficiaries[i]);
        separator();
    }
}

void searchBeneficiary(void) {
    header("SEARCH BENEFICIARY");
    int id = readPositiveInt("  Enter Beneficiary ID to search: ");
    int idx = findBeneficiary(id);
    if (idx == -1) { printf("  [!] Beneficiary ID %d not found.\n", id); return; }
    printBeneficiary(&beneficiaries[idx]);
}

void editBeneficiary(void) {
    header("EDIT BENEFICIARY");
    int id = readPositiveInt("  Enter Beneficiary ID to edit: ");
    int idx = findBeneficiary(id);
    if (idx == -1) { printf("  [!] Record not found.\n"); return; }

    printf("  Current record:\n");
    printBeneficiary(&beneficiaries[idx]);
    printf("  Enter new values (leave unchanged where prompted):\n");

    // beneficiaries is a pointer but beneficiaries[idx] is a struct, so we can modify it directly without needing to return anything
    readNonEmptyString("  New Name           : ", beneficiaries[idx].name, MAX_NAME);
    readNonEmptyString("  New Contact        : ", beneficiaries[idx].contact, MAX_CONTACT);
    readNonEmptyString("  New Address        : ", beneficiaries[idx].address, MAX_ADDRESS);
    readNonEmptyString("  New Need Category  : ", beneficiaries[idx].needCategory, MAX_CATEGORY);
    beneficiaries[idx].familySize    = readPositiveInt("  New Family Size    : ");
    beneficiaries[idx].priorityLevel = readIntInRange("  Priority (1/2/3)   : ", 1, 3);

    printf("  [OK] Beneficiary updated.\n");
}

void deleteBeneficiary(void) {
    header("DELETE BENEFICIARY");
    int id = readPositiveInt("  Enter Beneficiary ID to delete: ");
    int idx = findBeneficiary(id);
    if (idx == -1) { printf("  [!] Record not found.\n"); return; }

    /* Shift left */
    int i;
    for (i = idx; i < beneficiaryCount - 1; i++)
        beneficiaries[i] = beneficiaries[i + 1];
    beneficiaryCount--;

    printf("  [OK] Beneficiary ID %d deleted.\n", id);
}
// last one will be overwritten by the next one, so no need to free memory here
// after one extra we can use realloc to shrink the array if we want, but not required for this project


void beneficiaryMenu(void) {
    int ch;
    do {
        header("BENEFICIARY MANAGEMENT");
        printf("  1. Add Beneficiary\n"
               "  2. Display All\n"
               "  3. Search by ID\n"
               "  4. Edit Beneficiary\n"
               "  5. Delete Beneficiary\n"
               "  0. Back\n");
        ch = readIntInRange("  Choice: ", 0, 5);
        switch (ch) {
            case 1: addBeneficiary();         break;
            case 2: displayAllBeneficiaries(); break;
            case 3: searchBeneficiary();       break;
            case 4: editBeneficiary();         break;
            case 5: deleteBeneficiary();       break;
        }
    } while (ch != 0);
}

/* ═══════════════════════════════════════════════════════════════════
   MODULE 2 – VOLUNTEER MANAGEMENT
   ═══════════════════════════════════════════════════════════════════ */

void addVolunteer(void) {
    header("ADD VOLUNTEER");
    Volunteer v;

    do {
        v.volunteerId = readPositiveInt("  Enter Volunteer ID  : ");
        if (volunteerIdExists(v.volunteerId))
            printf("  [!] ID %d already exists.\n", v.volunteerId);
    } while (volunteerIdExists(v.volunteerId));

    readNonEmptyString("  Enter Name          : ", v.name, MAX_NAME);
    readNonEmptyString("  Enter Phone         : ", v.phone, MAX_PHONE);
    readNonEmptyString("  Enter Skill         : ", v.skill, MAX_SKILL);
    v.isAvailable = readIntInRange("  Available? (1=Yes 0=No): ", 0, 1);

    /* grow array using realloc */
    Volunteer *temp = (Volunteer *)realloc(volunteers, (volunteerCount + 1) * sizeof(Volunteer));
    if (!temp) { printf("  [!] Memory allocation failed.\n"); return; }
    volunteers = temp;
    volunteers[volunteerCount++] = v;

    printf("  [OK] Volunteer added successfully.\n");
}

void displayAllVolunteers(void) {
    header("ALL VOLUNTEERS");
    if (volunteerCount == 0) { printf("  No records found.\n"); return; }
    int i;
    for (i = 0; i < volunteerCount; i++) printVolunteer(&volunteers[i]);
}

void searchVolunteer(void) {
    header("SEARCH VOLUNTEER");
    int id = readPositiveInt("  Enter Volunteer ID: ");
    int idx = findVolunteer(id);
    if (idx == -1) { printf("  [!] Volunteer ID %d not found.\n", id); return; }
    printVolunteer(&volunteers[idx]);
}

void editVolunteer(void) {
    header("EDIT VOLUNTEER");
    int id = readPositiveInt("  Enter Volunteer ID to edit: ");
    int idx = findVolunteer(id);
    if (idx == -1) { printf("  [!] Record not found.\n"); return; }

    readNonEmptyString("  New Name    : ", volunteers[idx].name, MAX_NAME);
    readNonEmptyString("  New Phone   : ", volunteers[idx].phone, MAX_PHONE);
    readNonEmptyString("  New Skill   : ", volunteers[idx].skill, MAX_SKILL);
    volunteers[idx].isAvailable = readIntInRange("  Available? (1/0): ", 0, 1);
    printf("  [OK] Volunteer updated.\n");
}

void deleteVolunteer(void) {
    header("DELETE VOLUNTEER");
    int id = readPositiveInt("  Enter Volunteer ID to delete: ");
    int idx = findVolunteer(id);
    if (idx == -1) { printf("  [!] Record not found.\n"); return; }
    int i;
    for (i = idx; i < volunteerCount - 1; i++) volunteers[i] = volunteers[i + 1];
    volunteerCount--;
    printf("  [OK] Volunteer ID %d deleted.\n", id);
}

void volunteerMenu(void) {
    int ch;
    do {
        header("VOLUNTEER MANAGEMENT");
        printf("  1. Add Volunteer\n"
               "  2. Display All\n"
               "  3. Search by ID\n"
               "  4. Edit Volunteer\n"
               "  5. Delete Volunteer\n"
               "  0. Back\n");
        ch = readIntInRange("  Choice: ", 0, 5);
        switch (ch) {
            case 1: addVolunteer();         break;
            case 2: displayAllVolunteers(); break;
            case 3: searchVolunteer();      break;
            case 4: editVolunteer();        break;
            case 5: deleteVolunteer();      break;
        }
    } while (ch != 0);
}

/* ═══════════════════════════════════════════════════════════════════
   MODULE 3 – RESOURCE / INVENTORY MANAGEMENT
   ═══════════════════════════════════════════════════════════════════ */

void addResource(void) {
    header("ADD RESOURCE ITEM");
    Resource r;

    do {
        r.itemId = readPositiveInt("  Enter Item ID       : ");
        if (resourceIdExists(r.itemId))
            printf("  [!] ID %d already exists.\n", r.itemId);
    } while (resourceIdExists(r.itemId));

    readNonEmptyString("  Enter Item Name     : ", r.itemName, MAX_NAME);
    readNonEmptyString("  Enter Category      : ", r.category, MAX_CATEGORY);
    r.quantityAvailable = readNonNegInt("  Enter Quantity      : ");
    r.reorderLevel      = readNonNegInt("  Enter Reorder Level : ");

    /* grow array using realloc */
    Resource *temp = (Resource *)realloc(resources, (resourceCount + 1) * sizeof(Resource));
    if (!temp) { printf("  [!] Memory allocation failed.\n"); return; }
    resources = temp;
    resources[resourceCount++] = r;

    printf("  [OK] Resource item added.\n");
}

void displayAllResources(void) {
    header("RESOURCE INVENTORY");
    if (resourceCount == 0) { printf("  No items in inventory.\n"); return; }
    int i;
    for (i = 0; i < resourceCount; i++) printResource(&resources[i]);
}

void searchResource(void) {
    header("SEARCH RESOURCE");
    char keyword[MAX_NAME];
    readNonEmptyString("  Enter item name or category to search: ", keyword, MAX_NAME);

    /* convert keyword to lowercase for case-insensitive compare */
    int i, j, found = 0;
    char kLow[MAX_NAME], buf[MAX_NAME];
    strncpy(kLow, keyword, MAX_NAME);
    for (i = 0; kLow[i]; i++) kLow[i] = (char)tolower((unsigned char)kLow[i]);

    for (i = 0; i < resourceCount; i++) {
        strncpy(buf, resources[i].itemName, MAX_NAME);
        for (j = 0; buf[j]; j++) buf[j] = (char)tolower((unsigned char)buf[j]);
        if (strstr(buf, kLow)) { printResource(&resources[i]); found = 1; continue; }

        strncpy(buf, resources[i].category, MAX_CATEGORY);
        for (j = 0; buf[j]; j++) buf[j] = (char)tolower((unsigned char)buf[j]);
        if (strstr(buf, kLow)) { printResource(&resources[i]); found = 1; }
    }
    if (!found) printf("  [!] No matching resource found.\n");
}

void editResource(void) {
    header("EDIT RESOURCE ITEM");
    int id = readPositiveInt("  Enter Item ID to edit: ");
    int idx = findResource(id);
    if (idx == -1) { printf("  [!] Item not found.\n"); return; }

    readNonEmptyString("  New Item Name     : ", resources[idx].itemName, MAX_NAME);
    readNonEmptyString("  New Category      : ", resources[idx].category, MAX_CATEGORY);
    resources[idx].quantityAvailable = readNonNegInt("  New Quantity      : ");
    resources[idx].reorderLevel      = readNonNegInt("  New Reorder Level : ");
    printf("  [OK] Resource item updated.\n");
}

void deleteResource(void) {
    header("DELETE RESOURCE ITEM");
    int id = readPositiveInt("  Enter Item ID to delete: ");
    int idx = findResource(id);
    if (idx == -1) { printf("  [!] Item not found.\n"); return; }
    int i;
    for (i = idx; i < resourceCount - 1; i++) resources[i] = resources[i + 1];
    resourceCount--;
    printf("  [OK] Item ID %d deleted.\n", id);
}

void resourceMenu(void) {
    int ch;
    do {
        header("RESOURCE INVENTORY MANAGEMENT");
        printf("  1. Add Resource Item\n"
               "  2. Display Inventory\n"
               "  3. Search Resource\n"
               "  4. Edit Resource\n"
               "  5. Delete Resource\n"
               "  0. Back\n");
        ch = readIntInRange("  Choice: ", 0, 5);
        switch (ch) {
            case 1: addResource();         break;
            case 2: displayAllResources(); break;
            case 3: searchResource();      break;
            case 4: editResource();        break;
            case 5: deleteResource();      break;
        }
    } while (ch != 0);
}

/* ═══════════════════════════════════════════════════════════════════
   MODULE 4 – RESOURCE ALLOCATION
   ═══════════════════════════════════════════════════════════════════ */

void allocateResource(void) {
    header("ALLOCATE RESOURCE");
    Allocation a;

    /* Unique allocation ID */
    do {
        a.allocationId = readPositiveInt("  Enter Allocation ID    : ");
        if (allocationIdExists(a.allocationId))
            printf("  [!] Allocation ID %d already exists.\n", a.allocationId);
    } while (allocationIdExists(a.allocationId));

    /* Validate beneficiary */
    a.beneficiaryId = readPositiveInt("  Enter Beneficiary ID   : ");
    if (findBeneficiary(a.beneficiaryId) == -1) {
        printf("  [!] Beneficiary ID %d does not exist. Allocation cancelled.\n",
               a.beneficiaryId);
        return;
    }

    /* Validate resource */
    a.itemId = readPositiveInt("  Enter Item ID           : ");
    int rIdx = findResource(a.itemId);
    if (rIdx == -1) {
        printf("  [!] Item ID %d does not exist. Allocation cancelled.\n", a.itemId);
        return;
    }

    a.requestedQty = readPositiveInt("  Enter Requested Qty     : ");
    readNonEmptyString("  Enter Date (DD-MM-YYYY) : ", a.allocationDate, MAX_DATE);

    /* Stock check */
    if (resources[rIdx].quantityAvailable >= a.requestedQty) {
        a.allocatedQty = a.requestedQty;
        resources[rIdx].quantityAvailable -= a.allocatedQty;
        printf("  [OK] Allocation successful. %d unit(s) of '%s' issued.\n",
               a.allocatedQty, resources[rIdx].itemName);
        if (resources[rIdx].quantityAvailable <= resources[rIdx].reorderLevel)
            printf("  [WARN] Stock of '%s' is at or below reorder level (%d units left).\n",
                   resources[rIdx].itemName, resources[rIdx].quantityAvailable);
    } else {
        a.allocatedQty = 0;
        printf("  [!] Insufficient stock. Available: %d, Requested: %d. "
               "Allocation recorded as failed.\n",
               resources[rIdx].quantityAvailable, a.requestedQty);
    }

    /* Append allocation */
    Allocation *newArr = (Allocation *)malloc((allocationCount + 1) * sizeof(Allocation));
    if (!newArr) { printf("  [!] Memory allocation failed.\n"); return; }
    if (allocations) {
        memcpy(newArr, allocations, allocationCount * sizeof(Allocation));
        free(allocations);
    }
    allocations = newArr;
    allocations[allocationCount++] = a;
}

void displayAllAllocations(void) {
    header("ALL ALLOCATION RECORDS");
    if (allocationCount == 0) { printf("  No allocation records found.\n"); return; }
    int i;
    for (i = 0; i < allocationCount; i++) printAllocation(&allocations[i]);
}

void allocationMenu(void) {
    int ch;
    do {
        header("RESOURCE ALLOCATION");
        printf("  1. New Allocation\n"
               "  2. View All Allocations\n"
               "  0. Back\n");
        ch = readIntInRange("  Choice: ", 0, 2);
        switch (ch) {
            case 1: allocateResource();      break;
            case 2: displayAllAllocations(); break;
        }
    } while (ch != 0);
}

/* ═══════════════════════════════════════════════════════════════════
   MODULE 5 – SEARCH RECORDS (global cross-module search)
   ═══════════════════════════════════════════════════════════════════ */

void searchMenu(void) {
    int ch;
    do {
        header("SEARCH RECORDS");
        printf("  1. Search Beneficiary by ID\n"
               "  2. Search Volunteer by ID\n"
               "  3. Search Resource by Name / Category\n"
               "  0. Back\n");
        ch = readIntInRange("  Choice: ", 0, 3);
        switch (ch) {
            case 1: searchBeneficiary(); break;
            case 2: searchVolunteer();   break;
            case 3: searchResource();    break;
        }
    } while (ch != 0);
}

/* ═══════════════════════════════════════════════════════════════════
   MODULE 6 – REPORTS
   ═══════════════════════════════════════════════════════════════════ */

void reportStockSummary(void) {
    header("STOCK SUMMARY REPORT");
    if (resourceCount == 0) { printf("  No inventory data.\n"); return; }
    printf("  %-5s  %-25s  %-15s  %8s  %8s\n",
           "ID", "Item Name", "Category", "Qty", "Reorder");
    separator();
    int i;
    for (i = 0; i < resourceCount; i++) {
        printf("  %-5d  %-25s  %-15s  %8d  %8d%s\n",
               resources[i].itemId, resources[i].itemName,
               resources[i].category, resources[i].quantityAvailable,
               resources[i].reorderLevel,
               (resources[i].quantityAvailable <= resources[i].reorderLevel)
               ? "  << LOW" : "");
    }
}

void reportLowStock(void) {
    header("LOW STOCK ALERT");
    int i, found = 0;
    for (i = 0; i < resourceCount; i++) {
        if (resources[i].quantityAvailable <= resources[i].reorderLevel) {
            printResource(&resources[i]);
            found = 1;
        }
    }
    if (!found) printf("  All items are above reorder level.\n");
}

void reportBeneficiariesServed(void) {
    header("BENEFICIARIES SERVED");
    /* Find unique beneficiary IDs that have at least one successful allocation */
    int i, j, served = 0;
    int *servedIds = (int *)calloc(beneficiaryCount, sizeof(int));
    if (!servedIds) { printf("  [!] Memory error.\n"); return; }

    for (i = 0; i < allocationCount; i++) {
        if (allocations[i].allocatedQty > 0) {
            int found = 0;
            for (j = 0; j < served; j++)
                if (servedIds[j] == allocations[i].beneficiaryId) { found = 1; break; }
            if (!found) servedIds[served++] = allocations[i].beneficiaryId;
        }
    }

    printf("  Total unique beneficiaries served: %d\n\n", served);
    for (i = 0; i < served; i++) {
        int idx = findBeneficiary(servedIds[i]);
        if (idx != -1) printBeneficiary(&beneficiaries[idx]);
    }
    free(servedIds);
}

void reportPendingAllocations(void) {
    header("PENDING / FAILED ALLOCATIONS");
    int i, found = 0;
    for (i = 0; i < allocationCount; i++) {
        if (allocations[i].allocatedQty == 0) {
            printAllocation(&allocations[i]);
            found = 1;
        }
    }
    if (!found) printf("  No pending or failed allocations.\n");
}

void reportVolunteerSummary(void) {
    header("VOLUNTEER SUMMARY");
    int i, avail = 0, total = volunteerCount;
    for (i = 0; i < volunteerCount; i++)
        if (volunteers[i].isAvailable) avail++;
    printf("  Total Volunteers : %d\n", total);
    printf("  Available        : %d\n", avail);
    printf("  Unavailable      : %d\n", total - avail);
    separator();
    for (i = 0; i < volunteerCount; i++) printVolunteer(&volunteers[i]);
}

void reportsMenu(void) {
    int ch;
    do {
        header("REPORTS");
        printf("  1. Stock Summary\n"
               "  2. Low Stock Alert\n"
               "  3. Beneficiaries Served\n"
               "  4. Pending / Failed Allocations\n"
               "  5. Volunteer Summary\n"
               "  0. Back\n");
        ch = readIntInRange("  Choice: ", 0, 5);
        switch (ch) {
            case 1: reportStockSummary();        break;
            case 2: reportLowStock();            break;
            case 3: reportBeneficiariesServed(); break;
            case 4: reportPendingAllocations();  break;
            case 5: reportVolunteerSummary();    break;
        }
    } while (ch != 0);
}

/* ═══════════════════════════════════════════════════════════════════
   MODULE 7 – FILE HANDLING (SAVE & LOAD)
   ═══════════════════════════════════════════════════════════════════ */

void saveData(void) {
    FILE *fp;
    int i;

    /* ---- beneficiaries ---- */
    fp = fopen(FILE_BENEFICIARY, "w");
    if (!fp) { printf("  [!] Cannot open %s for writing.\n", FILE_BENEFICIARY); return; }
    fprintf(fp, "%d\n", beneficiaryCount);
    for (i = 0; i < beneficiaryCount; i++) {
        Beneficiary *b = &beneficiaries[i];
        fprintf(fp, "%d|%s|%s|%s|%s|%d|%d\n",
                b->beneficiaryId, b->name, b->contact,
                b->address, b->needCategory, b->familySize, b->priorityLevel);
    }
    fclose(fp);

    /* ---- volunteers ---- */
    fp = fopen(FILE_VOLUNTEER, "w");
    if (!fp) { printf("  [!] Cannot open %s for writing.\n", FILE_VOLUNTEER); return; }
    fprintf(fp, "%d\n", volunteerCount);
    for (i = 0; i < volunteerCount; i++) {
        Volunteer *v = &volunteers[i];
        fprintf(fp, "%d|%s|%s|%s|%d\n",
                v->volunteerId, v->name, v->phone, v->skill, v->isAvailable);
    }
    fclose(fp);

    /* ---- resources ---- */
    fp = fopen(FILE_RESOURCE, "w");
    if (!fp) { printf("  [!] Cannot open %s for writing.\n", FILE_RESOURCE); return; }
    fprintf(fp, "%d\n", resourceCount);
    for (i = 0; i < resourceCount; i++) {
        Resource *r = &resources[i];
        fprintf(fp, "%d|%s|%s|%d|%d\n",
                r->itemId, r->itemName, r->category,
                r->quantityAvailable, r->reorderLevel);
    }
    fclose(fp);

    /* ---- allocations ---- */
    fp = fopen(FILE_ALLOCATION, "w");
    if (!fp) { printf("  [!] Cannot open %s for writing.\n", FILE_ALLOCATION); return; }
    fprintf(fp, "%d\n", allocationCount);
    for (i = 0; i < allocationCount; i++) {
        Allocation *a = &allocations[i];
        fprintf(fp, "%d|%d|%d|%d|%d|%s\n",
                a->allocationId, a->beneficiaryId, a->itemId,
                a->requestedQty, a->allocatedQty, a->allocationDate);
    }
    fclose(fp);

    printf("  [OK] Data saved to files successfully.\n");
}

/*static:
Limits the function’s visibility to the current .c file (internal linkage). 
like private function 

*char  return a pointer pointing to char

Without ** the caller's pointer never moves:


*(ptr + n) works for accessing known positions, but **ptr lets you freely 
reposition the pointer to any address without calculating the distance — which is exactly what token parsing needs.
*/

/* Helper: parse a pipe-delimited token from a line buffer */

static char *nextToken(char **ptr) {
    char *start = *ptr;
    char *end   = strchr(start, '|');
    if (end) { *end = '\0'; *ptr = end + 1; }
    else       { *ptr = start + strlen(start); }
    return start;
}

void loadData(void) {
    FILE *fp;
    int count, i;
    char line[512];

    /* ---- beneficiaries ---- */
    fp = fopen(FILE_BENEFICIARY, "r");
    if (fp) {
        fscanf(fp, "%d\n", &count);
        if (beneficiaries) free(beneficiaries);
        beneficiaries = (Beneficiary *)malloc(count * sizeof(Beneficiary));
        beneficiaryCount = 0;
        for (i = 0; i < count && fgets(line, sizeof(line), fp); i++) {
            line[strcspn(line, "\n")] = '\0';
            char *p = line;
            Beneficiary *b = &beneficiaries[beneficiaryCount++];
            b->beneficiaryId = atoi(nextToken(&p));
            strncpy(b->name,         nextToken(&p), MAX_NAME);
            strncpy(b->contact,      nextToken(&p), MAX_CONTACT);
            strncpy(b->address,      nextToken(&p), MAX_ADDRESS);
            strncpy(b->needCategory, nextToken(&p), MAX_CATEGORY);
            b->familySize    = atoi(nextToken(&p));
            b->priorityLevel = atoi(nextToken(&p));
        }
        fclose(fp);
        printf("  Loaded %d beneficiary record(s).\n", beneficiaryCount);
    }

    /* ---- volunteers ---- */
    fp = fopen(FILE_VOLUNTEER, "r");
    if (fp) {
        fscanf(fp, "%d\n", &count);
        if (volunteers) free(volunteers);
        volunteers = (Volunteer *)malloc(count * sizeof(Volunteer));
        volunteerCount = 0;
        for (i = 0; i < count && fgets(line, sizeof(line), fp); i++) {
            line[strcspn(line, "\n")] = '\0';
            char *p = line;
            Volunteer *v = &volunteers[volunteerCount++];
            v->volunteerId = atoi(nextToken(&p));
            strncpy(v->name,  nextToken(&p), MAX_NAME);
            strncpy(v->phone, nextToken(&p), MAX_PHONE);
            strncpy(v->skill, nextToken(&p), MAX_SKILL);
            v->isAvailable = atoi(nextToken(&p));
        }
        fclose(fp);
        printf("  Loaded %d volunteer record(s).\n", volunteerCount);
    }

    /* ---- resources ---- */
    fp = fopen(FILE_RESOURCE, "r");
    if (fp) {
        fscanf(fp, "%d\n", &count);
        if (resources) free(resources);
        resources = (Resource *)malloc(count * sizeof(Resource));
        resourceCount = 0;
        for (i = 0; i < count && fgets(line, sizeof(line), fp); i++) {
            line[strcspn(line, "\n")] = '\0';
            char *p = line;
            Resource *r = &resources[resourceCount++];
            r->itemId = atoi(nextToken(&p));
            strncpy(r->itemName, nextToken(&p), MAX_NAME);
            strncpy(r->category, nextToken(&p), MAX_CATEGORY);
            r->quantityAvailable = atoi(nextToken(&p));
            r->reorderLevel      = atoi(nextToken(&p));
        }
        fclose(fp);
        printf("  Loaded %d resource record(s).\n", resourceCount);
    }

    /* ---- allocations ---- */
    fp = fopen(FILE_ALLOCATION, "r");
    if (fp) {
        fscanf(fp, "%d\n", &count);
        if (allocations) free(allocations);
        allocations = (Allocation *)malloc(count * sizeof(Allocation));
        allocationCount = 0;
        for (i = 0; i < count && fgets(line, sizeof(line), fp); i++) {
            line[strcspn(line, "\n")] = '\0';
            char *p = line;
            Allocation *a = &allocations[allocationCount++];
            a->allocationId  = atoi(nextToken(&p));
            a->beneficiaryId = atoi(nextToken(&p));
            a->itemId        = atoi(nextToken(&p));
            a->requestedQty  = atoi(nextToken(&p));
            a->allocatedQty  = atoi(nextToken(&p));
            strncpy(a->allocationDate, nextToken(&p), MAX_DATE);
        }
        fclose(fp);
        printf("  Loaded %d allocation record(s).\n", allocationCount);
    }
}

/* ═══════════════════════════════════════════════════════════════════
   FREE ALL DYNAMIC MEMORY
   ═══════════════════════════════════════════════════════════════════ */

void freeAll(void) {
    if (beneficiaries) { free(beneficiaries); beneficiaries = NULL; }
    if (volunteers)    { free(volunteers);    volunteers    = NULL; }
    if (resources)     { free(resources);     resources     = NULL; }
    if (allocations)   { free(allocations);   allocations   = NULL; }
}

/* ═══════════════════════════════════════════════════════════════════
   SEED SAMPLE DATA (15+ records)
   ═══════════════════════════════════════════════════════════════════ */
/*
Iteration 1: newArr → holds Beneficiary block address → given to beneficiaries → newArr dies
Iteration 2: newArr → holds Volunteer block address   → given to volunteers   → newArr dies
Iteration 3: newArr → holds Resource block address    → given to resources    → newArr dies
Iteration 4: newArr → holds Allocation block address  → given to allocations  → newArr dies
*/
void seedSampleData(void) {
    /* ── Beneficiaries (6 records) ── */
    const char *bNames[]   = {"Aisha Al Mansoori","Ravi Kumar","Sara Qasim",
                               "John Doe","Fatima Nasser","Li Wei"};
    const char *bContact[] = {"0501234567","0559876543","0523456789",
                               "0561234000","0589001234","0507654321"};
    const char *bAddr[]    = {"Al Ain, Block 3","Mussafah, Villa 7",
                               "Sharjah, Apt 12","Dubai, Flat 5",
                               "Abu Dhabi, Tower 2","Ajman, Room 9"};
    const char *bCat[]     = {"Food","Medicine","Clothing",
                               "Books","Food","Medicine"};
    int bFamily[]  = {4,2,6,3,5,1};
    int bPriority[]= {1,2,3,1,2,3};

    int i;
for (i = 0; i < 6; i++) {
    Beneficiary b;
    b.beneficiaryId  = 1001 + i;
    strncpy(b.name,         bNames[i],   MAX_NAME);
    strncpy(b.contact,      bContact[i], MAX_CONTACT);
    strncpy(b.address,      bAddr[i],    MAX_ADDRESS);
    strncpy(b.needCategory, bCat[i],     MAX_CATEGORY);
    b.familySize    = bFamily[i];
    b.priorityLevel = bPriority[i];

    Beneficiary *newArr = (Beneficiary *)realloc(beneficiaries, (beneficiaryCount + 1) * sizeof(Beneficiary));
    if (!newArr) { fprintf(stderr, "Memory allocation failed\n"); free(beneficiaries); exit(EXIT_FAILURE); }
    beneficiaries = newArr;
    beneficiaries[beneficiaryCount++] = b;
    }

    /* ── Volunteers (4 records) ── */
    const char *vNames[] = {"Ahmed Hassan","Priya Sharma","Carlos Reyes","Mona Ali"};
    const char *vPhones[]= {"0551112222","0552223333","0553334444","0554445555"};
    const char *vSkills[]= {"Driving","Data Entry","Field Visit","Packing"};
    int vAvail[]         = {1,1,0,1};

    for (i = 0; i < 4; i++) {
        Volunteer v;
        v.volunteerId = 2001 + i;
        strncpy(v.name,  vNames[i],  MAX_NAME);
        strncpy(v.phone, vPhones[i], MAX_PHONE);
        strncpy(v.skill, vSkills[i], MAX_SKILL);
        v.isAvailable = vAvail[i];

        Volunteer *newArr = (Volunteer *)realloc(volunteers, (volunteerCount + 1) * sizeof(Volunteer));
        if (!newArr) { fprintf(stderr, "Memory allocation failed\n"); free(volunteers); exit(EXIT_FAILURE); }
        volunteers = newArr;
        volunteers[volunteerCount++] = v;
    }

    /* ── Resources (5 records) ── */
    const char *rNames[]= {"Rice Bag 5kg","Paracetamol","School Bag",
                            "Winter Jacket","Baby Formula"};
    const char *rCats[] = {"Food","Medicine","Education","Clothing","Food"};
    int rQty[]          = {50,200,30,15,5};
    int rReorder[]      = {10,50,5,3,10};

    for (i = 0; i < 5; i++) {
        Resource r;
        r.itemId = 3001 + i;
        strncpy(r.itemName, rNames[i], MAX_NAME);
        strncpy(r.category, rCats[i],  MAX_CATEGORY);
        r.quantityAvailable = rQty[i];
        r.reorderLevel      = rReorder[i];

        Resource *newArr = (Resource *)realloc(resources, (resourceCount + 1) * sizeof(Resource));
        if (!newArr) { fprintf(stderr, "Memory allocation failed\n"); free(resources); exit(EXIT_FAILURE); }
        resources = newArr;
        resources[resourceCount++] = r;
    }

    /* ── Allocations (3 seed records) ── */
    int aIds[]   = {4001,4002,4003};
    int aBenef[] = {1001,1002,1003};
    int aItem[]  = {3001,3002,3001};
    int aReqQty[]= {5,10,20};
    int aAlcQty[]= {5,10,20};
    const char *aDates[] = {"01-05-2026","02-05-2026","03-05-2026"};

    for (i = 0; i < 3; i++) {
        Allocation a;
        a.allocationId  = aIds[i];
        a.beneficiaryId = aBenef[i];
        a.itemId        = aItem[i];
        a.requestedQty  = aReqQty[i];
        a.allocatedQty  = aAlcQty[i];
        strncpy(a.allocationDate, aDates[i], MAX_DATE);

        /* deduct from stock */
        int rIdx = findResource(a.itemId);
        if (rIdx != -1) resources[rIdx].quantityAvailable -= a.allocatedQty;

        Allocation *newArr = (Allocation *)realloc(allocations, (allocationCount + 1) * sizeof(Allocation));
        if (!newArr) { fprintf(stderr, "Memory allocation failed\n"); free(allocations); exit(EXIT_FAILURE); }
        allocations = newArr;
        allocations[allocationCount++] = a;
    }

    printf("  [OK] Sample data loaded: %d beneficiaries, %d volunteers, "
           "%d resources, %d allocations.\n",
           beneficiaryCount, volunteerCount, resourceCount, allocationCount);
}

/* ═══════════════════════════════════════════════════════════════════
   MAIN
   ═══════════════════════════════════════════════════════════════════ */

int main(void) {
    printf("\n");
    header("COMMUNITY HELP DESK AND RESOURCE ALLOCATION SYSTEM");
    printf("  Loading existing data from files...\n");
    loadData();

    /* If files were empty / first run, offer to load sample data */
    if (beneficiaryCount == 0 && volunteerCount == 0
        && resourceCount == 0 && allocationCount == 0) {
        printf("  No saved data found.\n");
        int choice = readIntInRange("  Load 15 sample records for demo? (1=Yes 0=No): ", 0, 1);
        if (choice == 1) seedSampleData();
    }

    int ch;
    do {
        header("MAIN MENU");
        printf("  1. Beneficiary Management\n"
               "  2. Volunteer Management\n"
               "  3. Resource Inventory Management\n"
               "  4. Resource Allocation\n"
               "  5. Search Records\n"
               "  6. Reports\n"
               "  7. Save Data\n"
               "  8. Exit\n");
        ch = readIntInRange("  Choice: ", 1, 8);
        switch (ch) {
            case 1: beneficiaryMenu(); break;
            case 2: volunteerMenu();   break;
            case 3: resourceMenu();    break;
            case 4: allocationMenu();  break;
            case 5: searchMenu();      break;
            case 6: reportsMenu();     break;
            case 7: saveData();        break;
            case 8:
                saveData();
                printf("  Goodbye!\n");
                break;
        }
    } while (ch != 8);

    freeAll();
    return 0;
}