/* STAVAR Laurentiu-Crisitan - 312CC */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OP_LENGTH 21

typedef struct Node {
    struct Node *prev, *next;
    char data;
} Node, *List;

typedef struct Lane {
    List list;
    Node *finger;
} Lane;

typedef struct Node_stack {
    struct Node_stack *next;
    Node *new_finger;
} Node_stack, *Stack;

typedef struct Node_queue {
    struct Node_queue *next;
    char op[MAX_OP_LENGTH];
} Node_queue;

typedef struct iQueue {
    struct Node_queue *head, *tail;
    int dimension;
} iQueue, *Queue;

List init_list(List list);
void print_list(List list, List finger, FILE *fw);
void add_nth_node(List list, char new_data, int position, 
                    char *str, FILE *fw);
List move_finger(List finger, char *str);
List move_chr_finger(List finger, char c, char *str, FILE *fw);
Stack init_stack(Stack stack);
void push(Stack stack, Node *new_finger_node);
List pop(Stack stack);
void print_stack(Stack stack);
Queue init_queue(Queue queue);
void enqueue(Queue queue, char *str);
void dequeue(Queue queue, char *str);
void start(Lane *lane, Queue queue, Stack undo, Stack redo);
void free_lane(Lane *lane);
void free_queue(Queue queue);
void free_stack(Stack stack);

// initiate a list
List init_list(List list) 
{
    list->prev = NULL;
    list->next = (List)malloc(1 * sizeof(*list->next));
    list->next->prev = list;
    list->next->next = NULL;
    list->next->data = '#';

    return list;
}

// prints a list
void print_list(List list, List finger, FILE *fw) 
{
    // skip the sentinell
    List current_node = list->next;

    while (current_node != NULL) {
        if (current_node != finger) {
            fprintf(fw, "%c", current_node->data);
        } else {
            fprintf(fw, "|%c|", current_node->data);
        }
        current_node = current_node->next;
    }

    fprintf(fw, "\n");
}

// add an element at an certain index left/right
void add_nth_node(List list, char new_data,
                    int position, char *str, FILE *fw) 
{
    // skip the sentinell
    List current_node = list->next;
    int i;

    for (i = 0; i < position; i++) {
        current_node = current_node->next;
        // if the position is too big
        if (current_node == NULL) {
            fprintf(fw, "ERROR\n");
            return;
        }
    }

    List new_node = (List)malloc(1 * sizeof(*new_node));
    new_node->data = new_data;

    // add the new element at left/right
    if (strcmp(str, "left") == 0) {
        // if we try to INSERT_LEFT in the first position
        if (current_node->prev->prev == NULL) {
            fprintf(fw, "ERROR\n");
            free(new_node);
            return;
        }

        new_node->prev = current_node->prev;
        new_node->next = current_node;
        current_node->prev->next = new_node;
        current_node->prev = new_node;
    } else if (strcmp(str, "right") == 0) {
        new_node->prev = current_node;
        new_node->next = current_node->next;

        // if we try to add an element in the tail
        if (current_node->next != NULL) {
            current_node->next->prev = new_node;
        }

        current_node->next = new_node;
    }
}

// moves finger to the left/right
List move_finger(List finger, char *str) 
{
    // if we want to add at left
    if (strcmp(str, "left") == 0 && finger->prev->prev != NULL) {
        finger = finger->prev;
    } else if (strcmp(str, "right") == 0) {
        // if we want to add at tail
        if (finger->next == NULL) {
            finger->next = (List)malloc(1 * sizeof(*finger->next));
            finger->next->prev = finger;
            finger->next->next = NULL;
            finger->next->data = '#';
        }

        finger = finger->next;
    }

    return finger;
}

// moves finger to the first character c to the left/right
List move_chr_finger(List finger, char c, char *str, FILE *fw) 
{
    List current_node = finger;

    if (strcmp(str, "left") == 0) {
        // search for character in the list
        while (current_node->prev != NULL) {
            if (current_node->data == c) break;
            current_node = current_node->prev;
        }

        // if we didn't find the character
        if (current_node->prev == NULL) {
            fprintf(fw, "ERROR\n");
            return finger;
        }
    } else if (strcmp(str, "right") == 0) {
        // search for character in the list
        while (current_node->next != NULL) {
            if (current_node->data == c) break;
            current_node = current_node->next;
        }

        // if we didn't find the character
        if (current_node->next == NULL && current_node->data != c) {
            List new_node = (List)malloc(1 * sizeof(*new_node));
            new_node->data = '#';
            current_node->next = new_node;
            new_node->prev = current_node;
            new_node->next = NULL;

            return new_node;
        }
    }

    return current_node;
}

// initiate a stack
Stack init_stack(Stack stack) 
{
    stack->next = (Stack)malloc(1 * sizeof(*(stack->next)));
    stack->next->new_finger = NULL;
    stack->next->next = NULL;

    return stack;
}

// add an element in the stack
void push(Stack stack, Node *new_finger_node) 
{
    // if the stack is empty
    if (stack->next->new_finger == NULL) {
        stack->next->new_finger = new_finger_node;
    } else {
        Stack new_node = (Stack)malloc(1 * sizeof(*new_node));
        new_node->new_finger = new_finger_node;
        new_node->next = stack->next;
        stack->next = new_node;
    }
}

// pop an element from a stack
List pop(Stack stack) 
{
    List to_return = stack->next->new_finger;
    Stack to_free = stack->next;

    // if the stack has more than one element
    if (stack->next->next != NULL) {
        stack->next = stack->next->next;
        free(to_free);
    } else {
        stack->next->new_finger = NULL;
    }

    return to_return;
}

// initiate a queue
Queue init_queue(Queue queue) 
{
    queue->dimension = 0;
    queue->head = (Node_queue *)malloc(1 * sizeof(Node_queue));
    queue->tail = queue->head;
    queue->head->next = NULL;

    return queue;
}

// adds an element to the queue
void enqueue(Queue queue, char *str) 
{
    // if the queue is empty
    if (queue->dimension == 0) {
        strcpy(queue->head->op, str);
    } else {
        Node_queue *new_node = (Node_queue *)malloc(1 * sizeof(Node_queue));
        new_node->next = NULL;
        strcpy(new_node->op, str);
        queue->tail->next = new_node;
        queue->tail = new_node;
    }

    (queue->dimension)++;
}

// pop an element from the queue and copy it in str
void dequeue(Queue queue, char *str) 
{
    // if the queue is empty
    if (queue->dimension == 0) return;

    // copy in str
    strcpy(str, queue->head->op);

    if (queue->dimension > 1) {
        Node_queue *to_free = queue->head;
        queue->head = queue->head->next;
        free(to_free);
    }

    (queue->dimension)--;
}

// contains the implementation
void start(Lane *lane, Queue queue, Stack undo, Stack redo) 
{
    char operation[21];
    int iter, i;

    // file read and file write
    FILE *fr, *fw;
    fr = fopen("tema1.in", "r");
    fw = fopen("tema1.out", "w");
    fscanf(fr, "%d", &iter);
    fgetc(fr);

    for (i = 0; i < iter; i++) {
        // read the operation
        fgets(operation, 21, fr);
        operation[strlen(operation) - 1] = '\0';

        // QUERY OPERATIONS and UNDO / REDO
        if (strcmp(operation, "SHOW_CURRENT") == 0) {
            fprintf(fw, "%c\n", lane->finger->data);
            continue;
        } else if (strcmp(operation, "SHOW") == 0) {
            print_list(lane->list, lane->finger, fw);
            continue;
        } else if (strcmp(operation, "UNDO") == 0) {
            push(redo, lane->finger);
            lane->finger = pop(undo);
            continue;
        } else if (strcmp(operation, "REDO") == 0) {
            push(undo, lane->finger);
            lane->finger = pop(redo);
            continue;
        }

        // EXECUTE OPERATION
        if (strcmp(operation, "EXECUTE") == 0) {
            // flag for error
            dequeue(queue, operation);

            // UPDATE OPERATIONS
            if (strcmp(operation, "MOVE_LEFT") == 0) {
                Node *finger_copy = lane->finger;
                lane->finger = move_finger(lane->finger, "left");
                if (lane->finger != finger_copy) push(undo, finger_copy);
            } else if (strcmp(operation, "MOVE_RIGHT") == 0) {
                push(undo, lane->finger);
                lane->finger = move_finger(lane->finger, "right");
            } else if (strncmp(operation, "MOVE_LEFT_CHAR", 14) == 0 &&
                       strlen(operation) == 16) {
                char c = operation[15];
                lane->finger = move_chr_finger(lane->finger, c, "left", fw);
            } else if (strncmp(operation, "MOVE_RIGHT_CHAR", 15) == 0 &&
                       strlen(operation) == 17) {
                char c = operation[16];
                lane->finger = move_chr_finger(lane->finger, c, "right", fw);
            } else if (strncmp(operation, "WRITE", 5) == 0 &&
                       strlen(operation) == 7) {
                char c = operation[strlen(operation) - 1];
                lane->finger->data = c;
            } else if (strncmp(operation, "INSERT_LEFT", 11) == 0 &&
                       strlen(operation) == 13) {
                char c = operation[12];

                // find the index of the finger
                int index = 0;
                List current_node = lane->list->next;
                while (current_node != lane->finger) {
                    current_node = current_node->next;
                    index++;
                }

                add_nth_node(lane->list, c, index, "left", fw);

                // move the finger if it is the case
                if (lane->finger->prev->prev != NULL) {
                    lane->finger = lane->finger->prev;
                }
            } else if (strncmp(operation, "INSERT_RIGHT", 12) == 0 &&
                       strlen(operation) == 14) {
                char c = operation[13];

                // find the index of the finger
                int index = 0;
                List current_node = lane->list->next;
                while (current_node != lane->finger) {
                    current_node = current_node->next;
                    index++;
                }

                add_nth_node(lane->list, c, index, "right", fw);

                // move the finger
                lane->finger = lane->finger->next;
            }

            continue;
        }

        // else add the command to the queue
        enqueue(queue, operation);
    }

    fclose(fr);
    fclose(fw);
}

void free_lane(Lane *lane) 
{
    List current_node = lane->list->next;
    while (current_node != NULL) {
        List to_free = current_node;
        current_node = current_node->next;
        free(to_free);
    }

    free(lane->list);
    free(lane);
}

void free_queue(Queue queue) 
{
    while (queue->head != NULL) {
        Node_queue *to_free = queue->head;
        queue->head = queue->head->next;
        free(to_free);
    }

    free(queue);
}

void free_stack(Stack stack) 
{
    Stack current_node = stack->next;
    while (current_node != NULL) {
        Stack to_free = current_node;
        current_node = current_node->next;
        free(to_free);
    }

    free(stack);
}

int main() 
{
    Lane *lane = (Lane *)malloc(1 * sizeof(Lane));
    lane->list = (List)malloc(1 * sizeof(*(lane->list)));
    lane->list = init_list(lane->list);
    lane->finger = lane->list->next;
    Stack undo = (Stack)malloc(1 * sizeof(*undo));
    undo = init_stack(undo);
    Stack redo = (Stack)malloc(1 * sizeof(*redo));
    redo = init_stack(redo);
    Queue queue = (Queue)malloc(1 * sizeof(*queue));
    queue = init_queue(queue);

    start(lane, queue, undo, redo);

    free_lane(lane);
    free_queue(queue);
    free_stack(undo);
    free_stack(redo);
}