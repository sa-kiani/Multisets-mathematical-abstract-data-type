// COMP2521 25T3 - Assignment 1
// Implementation of the Multiset ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Mset.h"
#include "MsetStructs.h"

////////////////////////////////////////////////////////////////////////
// Basic Operations

// Stage 1 helper declarations:
struct node *newNode(int elem, int count);

static struct node *InsertHelper(struct node *t, Mset s, int elem, int count);

static struct node *nodeDelete(struct node *t, Mset s, int elem, int count);

static struct node *destroyNode(struct node *t, Mset s);

static int findAndCount(struct node *t, int search);

static int findBiggest(struct node *t);

void inOrderForFile(struct node *t, FILE *output, int biggest);

// Stage 2 helper declarations:
void copyTreeToUnion(struct node *t, Mset new);

void mergeToUnion(struct node *t, Mset new);

void addIfIntersect(struct node *t1, Mset set2, Mset new);

static bool checkIfIncluded(struct node *t1, Mset s2);

static bool checkIfEqual(struct node *t1, Mset s2);


// Stage 3 helper declerations:

static struct node *checkBalance(struct node *t);

static struct node *rotateLeft(struct node *t);

static struct node *rotateRight(struct node *t);

static void updateHeight(struct node *t);

static int getBalance(struct node *t);

static int getHeight(struct node *t);

// stage 4 helper declerations
static struct node *getIndex(struct node *t, int index);

static int subtreeSize(struct node *t);

static int travserseForIndex(struct node *t, int elem);

/**
 * Creates a new empty multiset.
 */
Mset MsetNew(void) {
    // Allocate memory for new Mset
    struct mset *newMset = malloc(sizeof (struct mset));

    if (newMset == NULL) {
        fprintf(stderr, "Memory allocation for new Mset failed!\n");
        exit(EXIT_FAILURE);
    }
    
    // Set variables to NULL/0
    newMset->tree = NULL;
    newMset->size = 0;
    newMset->totalCount = 0;
    return newMset;
}

/**
 * helper function for MsetFree to free individual nodes
 */
void freeNodes(struct node *t) {
    // Base case: return when no nodes left
    if (t == NULL) {
        return;
    }

    // Free all nodes on current node's left and right
    freeNodes(t->left);
    freeNodes(t->right);

    // Free the node itself
    free(t);
}

/**
 * Frees all memory allocated to the multiset.
 */
void MsetFree(Mset s) {
    // Return when set doesnt exist anymore
    if (s == NULL) {
        return;
    }

    // Free the nodes then s itself
    freeNodes(s->tree);
    free(s);
}

/**
 * Helper function for insertHelper
 * Creates a new node and assigns its height, element, and count.
 */
struct node *newNode(int elem, int count) {
    // Allocate memory for node
    struct node *new = malloc (sizeof(struct node));

    if (new == NULL) {
        fprintf(stderr, "Memory allocation for new node failed!\n");
        exit(EXIT_FAILURE);
    }
    
    // Set all variables to NULL/0
    new->left = NULL;
    new->right = NULL;
    new->elem = elem;
    new->count = count;
    new->height = 0;

    return new;
}


/**
 * Inserts one of an element into the multiset. Does nothing if the
 * element is equal to UNDEFINED.
 */
void MsetInsert(Mset s, int elem) {
    // Do nothing if element is undefined
    if (elem == UNDEFINED) {                            
        return;
    }

    // Check if Mset exists
    if (s == NULL) {
        fprintf(stderr, "Warning: no Mset to be inserted into!\n");
        exit(EXIT_FAILURE);
    }

    // Update tree after calling insertion helper
    s->tree = InsertHelper(s->tree, s, elem, 1);
}


/**
 * Inserts the given amount of an element into the multiset. Does
 * nothing if the element is equal to UNDEFINED or the given amount is 0
 * or less.
 */
void MsetInsertMany(Mset s, int elem, int amount) {
    // Check if added node is valid
    if (elem == UNDEFINED || amount == 0) {                         
        return;
    }

    // Check if Mset exists
    if (s == NULL) {
        fprintf(stderr, "Warning: no Mset to be inserted into!\n");
        exit(EXIT_FAILURE);                     
    }

    // Update tree after calling insertion helper
    s->tree = InsertHelper(s->tree, s, elem, amount);
}

/**
 * Helper Function for MsetInsert and MsetInsertMany
 * Inserts the node into the Mset and adjusts Mset data
 */
static struct node *InsertHelper(struct node *t, Mset s, int elem, int count) {
    // If node is at correct vacant destination for insertion
    if (t == NULL) {
        // Adjust Mset data
        s->totalCount = s->totalCount + count;
        s->size = s->size + 1;

        // Add node in the vacant space
        return newNode(elem, count);
    }
    
    // Save the current pointer's node value
    int svalue = t->elem;

    // If it exists, increase its count by required count, traverse otherwise
    if (svalue == elem) {
        t->count = t->count + count;
        // Adjust Mset data
        s->totalCount = s->totalCount + count;
        return t;
    } else if (elem < svalue) {
        t->left = InsertHelper(t->left, s, elem, count);
    } else if (elem > svalue) {
        t->right = InsertHelper(t->right, s, elem, count);
    } 

    // Update Mset data
    updateHeight(t);
    return checkBalance(t);
}


/**
 * Helper Function for MsetInsert and MsetInsertMany
 * Checks that the tree is balanced and adjusts via rotations if not
 */
static struct node *checkBalance(struct node *t) {
    // Rotate if left or right heavy

    // If right heavy:
    if (getBalance(t) < -1) {
        // Check if its just a right turn or a left right
        if (getBalance(t->right) <= 0) {
            t = rotateLeft(t);
        } else {
            t->right = rotateRight(t->right);
            t = rotateLeft(t);                  
        }
    }
    
    // If left heavy:
    if (getBalance(t) > 1) {            
        // Check if its just a right turn or a left right
        if (getBalance(t->left) >= 0) {
            t = rotateRight(t);
        } else {            
            t->left = rotateLeft(t->left);
            t = rotateRight(t);
        }
    }

    return t;
}

/**
 * Helper function for checkBalance
 * Returns the balance for the given node
 */
static int getBalance(struct node *t) {
    return getHeight(t->left) - getHeight(t->right);
}

/**
 * Helper function for getBalance
 * Returns -1 if the node doesnt exist
 */
static int getHeight(struct node *t) {
    if (t != NULL) {
        return t->height;
    }

    return -1;
}

/**
 * Helper Function for insertion, deletion, and rotations
 * updates the new height for the given node
 */
static void updateHeight(struct node *t) {
    // If the node exists
    if (t != NULL) {
        // Find the height of left and right child nodes
        int left = getHeight(t->left);
        int right = getHeight(t->right);

        // Update t's height based on the child node with bigger height
        if (left > right) {
            t->height = left + 1;
        } else {
            t->height = right + 1;
        }   
    }
}


/**
 * Helper Function for checkBalance
 * Rotates the given subtree to the left
 */
static struct node *rotateLeft(struct node *t) {
    // Return if t or its right doesnt exist
    if (t == NULL || t->right == NULL) {
        return t;
    }
    
    // Reallocate nodes to rotate, with successor taking over as the root node
    struct node *successor = t->right;
    t->right = successor->left;
    successor->left = t;

    // Update data
    updateHeight(t);
    updateHeight(successor);

    // Return new root
    return successor;
}


/**
 * Helper Function for checkBalance
 * Rotates the given subtree to the right
 */
static struct node *rotateRight(struct node *t) {
    // Return if t or its left doesnt exist
    if (t == NULL || t->left == NULL) {
        return t;
    }

    // Reallocate nodes to rotate, with successor taking over as the root node
    struct node *successor = t->left;
    t->left = successor->right;
    successor->right = t;

    // Update data
    updateHeight(t);
    updateHeight(successor);

    // Return new root
    return successor;
}

/**
 * Deletes one of an element from the multiset.
 */
void MsetDelete(Mset s, int elem) {
    // Check if Mset exists
    if (s == NULL) {
        fprintf(stderr, "Warning: no Mset to be deleted from!\n");
        exit(EXIT_FAILURE);
    }

    s->tree = nodeDelete(s->tree, s, elem, 1);
}

/**
 * Deletes the given amount of an element from the multiset.
 */
void MsetDeleteMany(Mset s, int elem, int amount) {
    // Check if Mset exists
    if (s == NULL) {
        fprintf(stderr, "Warning: no Mset to be deleted from!\n");
        exit(EXIT_FAILURE);
    }

    s->tree = nodeDelete(s->tree, s, elem, amount);
}

/**
 * Helper Function for MsetDelete and MsetDeleteMany
 * Finds the note to be deleted and deducts the given value from the chosen
 * node's count. Deletes if count is <= 0.
 */
static struct node *nodeDelete(struct node *t, Mset s, int elem, int count) {
    // Return if no tree exists                         
    if (t == NULL) {
        return NULL;
    }   

    // Traverse to desired node:        
    if (elem > t->elem) {
        t->right = nodeDelete(t->right, s, elem, count);
    } else if (elem < t->elem) {
        t->left = nodeDelete(t->left, s, elem, count);
    } else if (elem == t->elem) {
        // Check how much to delete by -> destroy the node if need be
        int newCount = t->count - count;
        if (newCount <= 0) {
            return destroyNode(t, s);
        } else {
            t->count = newCount;
            s->totalCount = s->totalCount - count;
        }
    }

    // Update height and check if tree needs rebalancing
    updateHeight(t);
    return checkBalance(t);
}

/**
 * Helper Function for nodeDelete
 * Destroys given node and adjusts tree afterwards
 */
static struct node *destroyNode(struct node *t, Mset s) {
    s->size = s->size - 1;
    s->totalCount = s->totalCount - t->count;
    
    // Case 1: leaf node
    if (t->left == NULL && t->right == NULL) {
        free(t);
        return NULL;
    }
    
    // Case 2: one child
    // If no right child, return the left one, skipping over the current
    // Likewise for no left child
    if (t->right == NULL) {
        struct node *left = t->left;
        free(t);
        return left;

    } else if (t->left == NULL) {
        struct node *right = t->right;
        free(t);
        return right;
    }

    // Case 3: two children
    // Find the successor to replace deleted node
    // Must be the smallest node on the right subtree to ensure balance
    struct node *successor = t->right;

    while (successor->left != NULL) {
        successor = successor->left;
    }

    t->elem = successor->elem;
    t->count = successor->count;

    t->right = nodeDelete(successor, s, successor->elem, successor->count);

    // Update height and check if tree needs rebalancing
    updateHeight(t);
    return checkBalance(t);
}

/**
 * Returns the number of distinct elements in the multiset.
 */
int MsetSize(Mset s) {
    if (s == NULL) {
        return 0;
    }           

    return s->size;
}


/**
 * Returns the sum of counts of all elements in the multiset.
 */
int MsetTotalCount(Mset s) {
    if (s == NULL) {
        return 0;
    }
    return s->totalCount;
}


/**
 * Returns the count of an element in the multiset, or 0 if it doesn't
 * occur in the multiset.
 */
int MsetGetCount(Mset s, int elem) {
    // Check if Mset exists
    if (s == NULL) {
        fprintf(stderr, "Warning: no Mset to get a node's count from!\n");
        exit(EXIT_FAILURE);
    }

    return findAndCount(s->tree, elem);
}

/**
 * Helper function for MsetGetCount
 * Locates the desired node and returns its count.
 */
static int findAndCount(struct node *t, int search) {
    // Return 0 if the node or the entire tree doesn't exist
    if (t == NULL) {
        return 0;
    }
    
    // Return the count if node is found
    if (t->elem == search) {
        return t->count;
    }

    // Traverse to find 
    if (search < t->elem) {
        return findAndCount(t->left, search);
    } else if (search > t->elem) {
        return findAndCount(t->right, search);
    }

    // Return 0 if node doesnt exist
    return 0;       
}

/**
 * Prints the multiset to a file.
 * The elements of the multiset should be printed in ascending order
 * inside a pair of curly braces, with elements separated by a comma
 * and space. Each element should be printed inside a pair of
 * parentheses with its count, separated by a comma and space.
 */
void MsetPrint(Mset s, FILE *file) {
    // Check if Mset exists
    if (s == NULL) {
        fprintf(stderr, "Warning: no Mset exists for printing!\n");
        exit(EXIT_FAILURE);
    }

    // Check if file exists:
    if (file == NULL) {
        fprintf(stderr, "Warning: no file exists for printing!\n");
        exit(EXIT_FAILURE);
    }


    // find the biggest number to avoid having a comma after it
    int biggest = findBiggest(s->tree);

    // using in-order traversal:
    fprintf(file, "{");                             
    inOrderForFile(s->tree, file, biggest);
    fprintf(file, "}");
}

/**
 * Helper function for MsetPrint
 * Finds the biggest value to avoid having a comma after inserting it 
 * into the file
 */
static int findBiggest(struct node *t) {
    // Traverse right until reaching the biggest node
    if (t->right == NULL) {
        return t->elem;
    }

    return findBiggest(t->right);
}

/**
 * Helper function for MsetPrint
 * Uses in-order traversal to insert the values into the file
 */
void inOrderForFile(struct node *t, FILE *output, int biggest) {
    if (t != NULL) {
        // Insert without a comma if it is the last node
        if (t->elem == biggest) {
            fprintf(output, "(%d, %d)", t->elem, t->count);
        } else {
            inOrderForFile(t->left, output, biggest);
            fprintf(output, "(%d, %d), ", t->elem, t->count);
            inOrderForFile(t->right, output, biggest);
        }
    } 

}

////////////////////////////////////////////////////////////////////////
// Advanced Operations

/**
 * Returns a new multiset representing the union of the two given
 * multisets.
 */
Mset MsetUnion(Mset s1, Mset s2) {
    // Check if Msets exist
    if (s1 == NULL) {
        fprintf(stderr, "Warning: no s1 to be inserted into union!\n");
        exit(EXIT_FAILURE);
    }

    if (s2 == NULL) {
        fprintf(stderr, "Warning: no s2 to be merged into union!\n");
        exit(EXIT_FAILURE);
    }
    
    // create new set to insert union into
    Mset new = MsetNew();

    // insert set s1
    copyTreeToUnion(s1->tree, new);

    // Insert s2 and change count if they overlap with s1
    mergeToUnion(s2->tree, new);

    return new;
}

/**
 * Helper function for MsetUnion
 * Copies the first tree into the union Mset using in-order traversal
 */
void copyTreeToUnion(struct node *t, Mset new) {
    if (t != NULL) {
        copyTreeToUnion(t->left, new);
        
        MsetInsertMany(new, t->elem, t->count);

        copyTreeToUnion(t->right, new);
    }
} 

/**
 * Helper function for MsetUnion
 * Adds the second tree's values to the new Mset
 */
void mergeToUnion(struct node *t, Mset new) {
    if (t != NULL) {
        mergeToUnion(t->left, new);
        
        // Find the count for current node in s2 
        int currentVal = MsetGetCount(new, t->elem);

        // Update to s2's count if its bigger than whats already in union tree
        if (currentVal < t->count) {
            MsetInsertMany(new, t->elem, t->count - currentVal); 
        }

        mergeToUnion(t->right, new);
    }
}


/**
 * Returns a new multiset representing the intersection of the two
 * given multisets.
 */
Mset MsetIntersection(Mset s1, Mset s2) {
    // Check if Msets exist
    if (s1 == NULL) {
        fprintf(stderr, "Warning: no s1 to be intersected!\n");
        exit(EXIT_FAILURE);
    }

    if (s2 == NULL) {
        fprintf(stderr, "Warning: no s2 to be intersected!\n");
        exit(EXIT_FAILURE);
    }
    
    // create new set to insert union into
    Mset new = MsetNew();
    
    // Add a value if its an intersection of s1 and s2
    addIfIntersect(s1->tree, s2, new);

    return new;
}

/**
 * Helper function for MsetIntersection
 * Adds to the new tree if both the first and second tree have it
 */
void addIfIntersect(struct node *t1, Mset set2, Mset new) {
    if (t1 != NULL) {
        // Progress with in order implementation
        addIfIntersect(t1->left, set2, new);

        // Find the element's count in set2
        int set2Count = MsetGetCount(set2, t1->elem);

        // If it also exists in set2, add the minimum amount to new Mset
        if (set2Count > 0) {
            if (t1->count < set2Count) {
                MsetInsertMany(new, t1->elem, t1->count);
            } else if (set2Count < t1->count) {
                MsetInsertMany(new, t1->elem, set2Count);
            }
        }

        addIfIntersect(t1->right, set2, new);
    }
}

/**
 * Returns true if the multiset s1 is included in the multiset s2, and
 * false otherwise.
 */
bool MsetIncluded(Mset s1, Mset s2) {
    // Check if Msets exist
    if (s1->tree == NULL) {
        fprintf(stderr, "Warning: no s1 to check for inclusion!\n");
        exit(EXIT_FAILURE);
    }

    if (s2->tree == NULL) {
        fprintf(stderr, "Warning: no s2 to check for inclusion!\n");
        exit(EXIT_FAILURE);
    }

    return checkIfIncluded(s1->tree, s2);
}

/**
 * Helper for MsetIncluded
 * Checks if the first tree is included in the second, and returns true if so
 */
static bool checkIfIncluded(struct node *t1, Mset s2) {
    if (t1 == NULL) {
        return true;
    }

    // in order recursive implementation
    if (!checkIfIncluded(t1->left, s2)) {
        return false;
    }

    // Return false if node doesnt exist, or is smaller than whats in the tree
    int set2Count = MsetGetCount(s2, t1->elem);
    if (set2Count <= 0 || set2Count < t1->count) {
        return false;
    } 

    return checkIfIncluded(t1->right, s2);
}


/**
 * Returns true if the two given multisets are equal, and false
 * otherwise.
 */
bool MsetEquals(Mset s1, Mset s2) {
    // Return true if both trees are NULL
    if (s1->tree == NULL && s2->tree == NULL) {
        return true;
    }

    // Return false if just one of the trees are NULL
    if (s1->tree == NULL || s2->tree == NULL) {
        return false;
    }

    // Check individual nodes 
    return checkIfEqual(s1->tree, s2);
}

/**
 * Helper for MsetEquals
 * Checks if the node from first tree is in second tree
 */
static bool checkIfEqual(struct node *t1, Mset s2) {
    // Return true if no nodes are left
    if (t1 == NULL) {
        return true;
    }

    // if the left node isnt equal, return false
    if (!checkIfEqual(t1->left, s2)){
        return false;
    }

    // Check to see if the node exists in s2, and theyre the same count
    if (MsetGetCount(s2, t1->elem) != t1->count) {
        return false;
    }

    // if the right node isnt equal, return false
    return checkIfEqual(t1->right, s2);
}


////////////////////////////////////////////////////////////////////////
// Index Operations

/**
 * Returns the element at the given index and its count, or
 * {UNDEFINED, 0} if the given index is outside the range [0, n - 1]
 * where n is the size of the multiset.
 */
struct item MsetAtIndex(Mset s, int index) {
    if (s == NULL || index < 0 || index >= s->size) {
        return (struct item){UNDEFINED, 0};
    }

    struct node *indexNode = getIndex(s->tree, index);
    
    if (indexNode == NULL) {
        return (struct item){UNDEFINED, 0};
    }

    return (struct item){indexNode->elem, indexNode->count};
}   

/**
 * Helper for MsetAtIndex
 * checks the number of nodes on the left subtree. it would equal the index.
 */
static struct node *getIndex(struct node *t, int index) {

    // Find how many nodes to the left of current node
    int left_of_node = subtreeSize(t->left);
    
    // Move until index matches node's position 
    if (left_of_node == index) {

        return t;
    } else if (left_of_node > index) {
        return getIndex(t->left, index);
    } else {
        // index - left_of_node - 1 will remove the root node and left nodes
        // from ind
        return getIndex(t->right, index - (left_of_node + 1));
    }
}

/**
 * Helper for getIndex
 * Counts the size of the given subtree
 */
static int subtreeSize(struct node *t) {
    if (t == NULL) {
        return 0;
    }

    return 1 + subtreeSize(t->right) + subtreeSize(t->left);
}


/**
 * Returns the index of the given element if it exists, or -1 otherwise.
 */
int MsetIndexOf(Mset s, int elem) {
    // Return -1 if no Mset exists
    if (s == NULL) {
        return -1;
    }

    // Traverse tree until desired node reached:
    return travserseForIndex(s->tree, elem);

}

static int travserseForIndex(struct node *t, int elem) {
    // Return NULL if the desired node doesnt exist
    if (t == NULL) {
        return -1;
    }

    // Traverse 
    if (elem < t->elem) {
        return travserseForIndex(t->left, elem);
    } else if (elem > t->elem) {
        int rightIndex = travserseForIndex(t->right, elem);

        // Return -1 if the desired element is not in the right subtree
        if (rightIndex == -1) {
            return -1;
        }
        // Else return adjusted index
        return 1 + subtreeSize(t->left) + rightIndex;
    } else {
        return subtreeSize(t->left);
    }
}
