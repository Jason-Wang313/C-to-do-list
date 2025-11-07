/*
 * =====================================================================================
 *
 * Filename:  todo.c
 *
 * Description:  A simple command-line to-do list manager in C.
 * This project demonstrates core C concepts including:
 * - Structs
 * - Pointers (including pointers to pointers)
 * - Singly Linked Lists
 * - Dynamic Memory Allocation (malloc, free)
 * - File I/O (fopen, fprintf, fgets, fclose)
 * - String Manipulation (strcpy, fgets, sscanf)
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constants ---
#define MAX_TASK_LEN 256
#define FILENAME "tasks.txt"

// --- Data Structure ---

// Define a Task structure
// This is the blueprint for each to-do item
typedef struct Task {
    char description[MAX_TASK_LEN]; // The text of the task
    int completed;                  // 0 = incomplete, 1 = complete
    struct Task *next;              // Pointer to the next task in the list
} Task;

// --- Function Prototypes ---

// Core Linked List Functions
Task* createTask(const char* description);
void addTask(Task** head, const char* description);
void deleteTask(Task** head, int index);
void freeList(Task* head);

// Application-Specific Functions
void displayTasks(Task* head);
void markComplete(Task* head, int index);
void saveTasks(Task* head);
void loadTasks(Task** head);
void printMenu(void);
void clearInputBuffer(void);

// --- Main Function (The Program's Entry Point) ---

int main() {
    Task* head = NULL; // 'head' is a pointer to the first task in our list.
                      // We start with an empty list (NULL).
    int choice = 0;
    char inputBuffer[MAX_TASK_LEN];
    char taskDescription[MAX_TASK_LEN];
    int taskIndex;

    printf("Welcome to your C To-Do List Manager!\n");
    
    // Load existing tasks from the file, if any.
    // We pass '&head' (a pointer to our 'head' pointer) so the function
    // can modify 'head' and build our linked list in memory.
    loadTasks(&head);

    while (1) {
        printMenu();
        
        // Get user choice
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
            continue; // Handle input error
        }

        // sscanf parses the string from inputBuffer.
        // This is safer than scanf() as it avoids many input buffer issues.
        if (sscanf(inputBuffer, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1: // Add Task
                printf("Enter task description: ");
                if (fgets(taskDescription, sizeof(taskDescription), stdin) == NULL) {
                    break;
                }
                // Remove the newline character that fgets() stores
                taskDescription[strcspn(taskDescription, "\n")] = 0;
                addTask(&head, taskDescription);
                printf("Task added.\n");
                break;

            case 2: // List Tasks
                displayTasks(head);
                break;

            case 3: // Mark Complete
                printf("Enter task number to mark complete: ");
                if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
                    break;
                }
                if (sscanf(inputBuffer, "%d", &taskIndex) != 1) {
                    printf("Invalid number.\n");
                    break;
                }
                markComplete(head, taskIndex);
                break;

            case 4: // Delete Task
                printf("Enter task number to delete: ");
                if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
                    break;
                }
                if (sscanf(inputBuffer, "%d", &taskIndex) != 1) {
                    printf("Invalid number.\n");
                    break;
                }
                deleteTask(&head, taskIndex);
                break;

            case 5: // Save and Quit
                printf("Saving tasks and quitting...\n");
                saveTasks(head); // Save all tasks to file
                freeList(head);  // Free all allocated memory
                return 0;        // Exit the program

            default:
                printf("Invalid choice. Please select from 1-5.\n");
        }
    }

    return 0; // Should never be reached
}

// --- Function Definitions ---

/**
 * @brief Clears the standard input buffer.
 * Call this after using scanf to prevent issues.
 * Note: We use fgets/sscanf in main, which is safer, but this
 * is a good utility function to know.
 */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Prints the main menu options.
 */
void printMenu(void) {
    printf("\n--- C To-Do List ---\n");
    printf("1. Add a new task\n");
    printf("2. List all tasks\n");
    printf("3. Mark a task as complete\n");
    printf("4. Delete a task\n");
    printf("5. Save and Quit\n");
    printf("Enter your choice: ");
}

/**
 * @brief Allocates memory for a new Task and initializes it.
 * @param description The text for the new task.
 * @return A pointer to the newly created Task on the heap.
 */
Task* createTask(const char* description) {
    // 1. Allocate memory on the heap for one Task struct.
    // sizeof(Task) calculates the number of bytes needed.
    Task* newTask = (Task*)malloc(sizeof(Task));

    // 2. Check if malloc failed (e.g., out of memory).
    if (newTask == NULL) {
        printf("Error: Could not allocate memory for new task.\n");
        exit(1); // Exit program on critical error
    }

    // 3. Initialize the new task's data.
    // Use strncpy for safety to avoid buffer overflows.
    strncpy(newTask->description, description, MAX_TASK_LEN - 1);
    newTask->description[MAX_TASK_LEN - 1] = '\0'; // Ensure null-termination
    newTask->completed = 0; // New tasks are incomplete by default
    newTask->next = NULL;   // This task is not pointing to anything... yet.

    // 4. Return the pointer to the new task.
    return newTask;
}

/**
 * @brief Adds a new task to the end of the linked list.
 * @param head A pointer-to-a-pointer to the head of the list.
 * We use this so we can modify the 'head' pointer itself
 * if the list is currently empty.
 * @param description The text for the new task.
 */
void addTask(Task** head, const char* description) {
    Task* newTask = createTask(description);

    // Case 1: The list is empty.
    if (*head == NULL) {
        *head = newTask; // The new task is now the head of the list.
    } 
    // Case 2: The list is not empty.
    else {
        Task* current = *head;
        // Traverse to the end of the list
        while (current->next != NULL) {
            current = current->next;
        }
        // 'current' is now the last node. Link the new task.
        current->next = newTask;
    }
}

/**
 * @brief Displays all tasks in the list, with their index and status.
 * @param head A pointer to the first task in the list.
 */
void displayTasks(Task* head) {
    if (head == NULL) {
        printf("\nYour to-do list is empty.\n");
        return;
    }

    printf("\n--- Your Tasks ---\n");
    Task* current = head;
    int index = 1;
    
    // Traverse the list from head to tail
    while (current != NULL) {
        // Print status ([X] or [ ]) and description
        printf("%d. [%c] %s\n", 
               index, 
               (current->completed ? 'X' : ' '), 
               current->description);
        
        current = current->next; // Move to the next task
        index++;
    }
}

/**
 * @brief Marks a task at a given index as complete.
 * @param head A pointer to the first task in the list.
 * @param index The 1-based index of the task to mark.
 */
void markComplete(Task* head, int index) {
    Task* current = head;
    int count = 1;

    // Traverse the list to find the Nth task
    while (current != NULL && count < index) {
        current = current->next;
        count++;
    }

    // Check if we found the task
    if (current == NULL) {
        printf("Error: Task %d not found.\n", index);
    } else {
        current->completed = 1;
        printf("Task %d marked as complete.\n", index);
    }
}

/**
 * @brief Deletes a task at a given index from the list.
 * @param head A pointer-to-a-pointer to the head of the list,
 * in case we need to delete the first item.
 * @param index The 1-based index of the task to delete.
 */
void deleteTask(Task** head, int index) {
    if (*head == NULL) {
        printf("Error: List is empty, nothing to delete.\n");
        return;
    }

    Task* temp = NULL;
    
    // Case 1: Deleting the head node (index == 1)
    if (index == 1) {
        temp = *head;            // 1. Store the node to be deleted
        *head = (*head)->next;   // 2. Point head to the *next* node
        free(temp);              // 3. Free the original head node
        printf("Task 1 deleted.\n");
        return;
    }

    // Case 2: Deleting a node other than the head
    Task* current = *head;
    int count = 1;

    // Traverse to find the node *before* the one to be deleted
    while (current != NULL && count < index - 1) {
        current = current->next;
        count++;
    }

    // Check if index is valid (i.e., we found the node before,
    // and the node to be deleted actually exists)
    if (current == NULL || current->next == NULL) {
        printf("Error: Task %d not found.\n", index);
        return;
    }

    // 'current' is now the (index-1)th node
    temp = current->next;            // 1. Store the node to be deleted (the index-th node)
    current->next = temp->next;    // 2. Link the (index-1)th node to the (index+1)th node
    free(temp);                      // 3. Free the deleted node
    printf("Task %d deleted.\n", index);
}

/**
 * @brief Frees all memory allocated for the linked list.
 * @param head A pointer to the first task.
 */
void freeList(Task* head) {
    Task* current = head;
    Task* temp = NULL;

    // Traverse the list, freeing each node one by one
    while (current != NULL) {
        temp = current;          // Store the current node
        current = current->next; // Move to the next node
        free(temp);              // Free the stored node
    }
    // 'head' will be a dangling pointer, but since the program is
    // exiting (or the list is being reset), this is okay.
}

/**
 * @brief Saves the entire linked list to the file "tasks.txt".
 * @param head A pointer to the first task.
 */
void saveTasks(Task* head) {
    // Open the file in "write" mode ("w").
    // This will create the file or overwrite it if it exists.
    FILE *file = fopen(FILENAME, "w"); 
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", FILENAME);
        return;
    }

    Task* current = head;
    // Traverse the list
    while (current != NULL) {
        // Write in a "CSV" (Comma Separated Value) format
        // e.g., "1,Buy milk" or "0,Study for exam"
        fprintf(file, "%d,%s\n", current->completed, current->description);
        current = current->next;
    }

    // Close the file handle
    fclose(file);
}

/**
 * @brief Loads tasks from "tasks.txt" into the linked list.
 * @param head A pointer-to-a-pointer to the head of the list.
 */
void loadTasks(Task** head) {
    // Open the file in "read" mode ("r").
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        // This is not an error. It just means we have no save file yet.
        printf("No existing task file found. Starting fresh.\n");
        return;
    }

    char lineBuffer[MAX_TASK_LEN + 10]; // Buffer to read each line
    char description[MAX_TASK_LEN];
    int completed;

    // Read one line at a time from the file until we reach the end
    while (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
        // Parse the line from the buffer.
        // The format %[^\n] means "read everything until a newline".
        if (sscanf(lineBuffer, "%d,%[^\n]", &completed, description) == 2) {
            // We have good data. Add it to our list.
            // We re-use addTask, which is efficient.
            addTask(head, description);
            
            // If the loaded task was complete, we must find it
            // in the list and mark it.
            if (completed) {
                Task* current = *head;
                // Go to the end of the list (where addTask just put it)
                while (current->next != NULL) {
                    current = current->next;
                }
                // Mark that new node as complete.
                current->completed = 1;
            }
        }
    }

    fclose(file);
    printf("Tasks loaded from %s.\n", FILENAME);
}
