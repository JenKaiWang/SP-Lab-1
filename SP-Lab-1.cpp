#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_NAME_LEN 64
#define MAX_LINE_LEN 256
#include "readfile.c"
#include "readfile.h"


void printDatabase(const char *filename) {
// Open the specified file
    if (open_file(filename) != 0) {
        // Print an error message if the file cannot be opened
        perror("Error opening file");
        return;
    }

    int id, salary, employeeCount = 0;
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    char buffer[MAX_LINE_LEN];
    
    // Print the header for the database table
    printf("\n%-20s %-20s %-10s\n", "NAME", "SALARY", "ID");
    printf("----------------------------------------------\n");
    // Read data from the file until the end of file is reached
    while (read_string(buffer, MAX_LINE_LEN) == 0) {
        // Parse the comma-separated values from the buffer
        char* token = strtok(buffer, ",");
        if (token) {
            id = atoi(token);
            token = strtok(NULL, ",");
            strcpy(fname, token);
            token = strtok(NULL, ",");
            strcpy(lname, token);
            token = strtok(NULL, ",");
            salary = atoi(token);

            char fullname[MAX_NAME_LEN * 2];
            snprintf(fullname, sizeof(fullname), "%s %s", fname, lname);

            printf("%-20s %-20d %-10d\n", fullname, salary, id);
            employeeCount++; 
        }
    }
      printf("Number of Employees (%d)\n", employeeCount);
    // Close the file after reading
    close_file();
}


void lookupByID(const char *filename) {
    int searchID, id, salary;
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    char buffer[MAX_LINE_LEN];
    
    // let user to enter the employee ID to search for
    printf("Enter a 6 digit employee id: ");
    scanf("%d", &searchID);
    
    // Open the specified file
    if (open_file(filename) != 0) {
        // Print an error message if the file cannot be opened
        perror("Error opening file");
        return;
    }

    int found = 0;
    while (read_string(buffer, MAX_LINE_LEN) == 0) {
        char* token = strtok(buffer, ",");
        if (token) {
            id = atoi(token);
            // Check if the current employee ID matches the search ID
            if (id == searchID) {
                token = strtok(NULL, ",");
                strcpy(fname, token);
                token = strtok(NULL, ",");
                strcpy(lname, token);
                token = strtok(NULL, ",");
                salary = atoi(token);

                // Create a full name with space between first name and last name
                char fullname[MAX_NAME_LEN * 2];
                snprintf(fullname, sizeof(fullname), "%s %s", fname, lname);

                printf("\n%-20s %-20d %-10d\n", fullname, salary, id);
                found = 1;
                break; // Break the loop once the record is found
            }
        }
    }
    // If the employee with the given ID is not found, print a message
    if (!found) {
        printf("Employee with ID %d not found.\n", searchID);
    }

    close_file();
}


void lookupByLastName(const char *filename) {
    char searchLastName[MAX_NAME_LEN];
    printf("Enter Employee's last name (no extra spaces): ");
    scanf("%63s", searchLastName);  

    // Open the file for reading
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LEN];
    int found = 0;
    printf("\n%-20s %-20s %-10s\n", "NAME", "SALARY", "ID");
   // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
        int id, salary;

        if (sscanf(line, "%d,%63[^,],%63[^,],%d", &id, fname, lname, &salary) == 4) {
            if (strcmp(lname, searchLastName) == 0) {
                // Print employee information if the last name matches
                printf("%-20s %-20d %-10d\n", strcat(strcat(fname, " "), lname), salary, id);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("Employee with Last Name '%s' not found.\n", searchLastName);
    }

    fclose(file);
}

int isAlphaString(const char *str) {
    while (*str) {
        // Check if the current character is not alphabetic
        if (!isalpha((unsigned char)*str)) {
            // Return 0 if the character is not alphabetic
            return 0;
        }
        str++;
    }
    // Return 1 if all characters are alphabetic
    return 1;
}

void getStringInput(const char *prompt, char *str, int maxLen) {
    int isValid;
    do {
        printf("%s", prompt);
        scanf("%63s", str);
        // Check if the input string contains only alphabetic characters
        isValid = isAlphaString(str);
        if (!isValid) {
            printf("Input must only contain letters. Please try again.\n");
        }
    } while (!isValid);
}

void addEmployee(const char *filename) {
    char fname[MAX_NAME_LEN], lname[MAX_NAME_LEN];
    int salary, confirm;
    int maxId = 0, id;

    // Open the file in append mode to add new records
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Find the maximum ID in the existing database
    fseek(file, 0, SEEK_SET);
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d", &id) == 1) {
            if (id > maxId) {
                maxId = id;
            }
        }
    }


    getStringInput("Enter the first name of the employee: ", fname, MAX_NAME_LEN);
    getStringInput("Enter the last name of the employee: ", lname, MAX_NAME_LEN);

	int inputResult;
    do {
        printf("Enter employee's salary (30000 to 150000): ");
        inputResult = scanf("%d", &salary);
        
        if (inputResult != 1 || salary < 30000 || salary > 150000) {
            printf("Salary must be an integer between 30000 and 150000. Please enter again.\n");
            // Clear the input buffer
            while (getchar() != '\n');
            inputResult = 0; 
        }
    } while (inputResult == 0);

    printf("Do you want to add the following employee to the DB?\n");
    printf("\t%s %s, salary: %d\n", fname, lname, salary);
    printf("Enter 1 for yes, 0 for no: ");
    scanf("%d", &confirm);

    if (confirm == 1) {
        fprintf(file, "\n%d,%s,%s,%d", maxId + 1, fname, lname, salary);
        printf("Employee added successfully.\n");
    }

    fclose(file); 
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    int choice;

    do {
        printf("\nEmployee DB Menu:\n");
        printf("----------------------------\n");
        printf("(1) Print the Database\n");
        printf("(2) Lookup by ID\n");
        printf("(3) Lookup by Last Name\n");
        printf("(4) Add an Employee\n");
        printf("(5) Quit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Please enter a valid number.\n");
            while (getchar() != '\n');
            continue;
        }

        if (choice < 1 || choice > 5) {
            printf("Hey, %d is not between 1 and 5, try again...\n", choice);
            continue;
        }

        switch(choice) {
            case 1:
                printDatabase(filename);
                break;
            case 2:
                lookupByID(filename);
                break;
            case 3:
                lookupByLastName(filename);
                break;
            case 4:
                addEmployee(filename);
                break;
            case 5:
                printf("good bye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while(choice != 5);

    return 0;
}

