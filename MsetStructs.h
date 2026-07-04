// COMP2521 25T3 - Assignment 1

#ifndef MSET_STRUCTS_H
#define MSET_STRUCTS_H

struct mset {
    struct node *tree;  
    int size;
    int totalCount;
};

// DO NOT MODIFY THE NAME OF THIS STRUCT
struct node {
    int elem;          
    int count;          
    struct node *left;  
    struct node *right; 
    int height;
};

////////////////////////////////////////////////////////////////////////
// Cursors

struct cursor {
};

////////////////////////////////////////////////////////////////////////

#endif
