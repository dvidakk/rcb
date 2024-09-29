#ifndef RB_TREE_H
#define RB_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    char* user_id;
    char* displayname;
    Color color;
    struct RBNode *left, *right, *parent;
} RBNode;

typedef struct {
    RBNode *root;
    RBNode *nil;  // Sentinel node
} RBTree;

RBTree* createRBTree() {
    RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
    tree->nil = (RBNode*)malloc(sizeof(RBNode));
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil->right = tree->nil->parent = NULL;
    tree->root = tree->nil;
    return tree;
}

void leftRotate(RBTree* tree, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != tree->nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rightRotate(RBTree* tree, RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != tree->nil) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == tree->nil) {
        tree->root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

void insertFixup(RBTree* tree, RBNode* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode* y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(tree, z->parent->parent);
            }
        } else {
            RBNode* y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

void insertUser(RBTree* tree, char* user_id, char* displayname) {
    RBNode* z = (RBNode*)malloc(sizeof(RBNode));
    z->user_id = strdup(user_id);
    z->displayname = strdup(displayname);
    z->color = RED;
    
    RBNode* y = tree->nil;
    RBNode* x = tree->root;
    while (x != tree->nil) {
        y = x;
        if (strcmp(z->user_id, x->user_id) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == tree->nil) {
        tree->root = z;
    } else if (strcmp(z->user_id, y->user_id) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }
    z->left = tree->nil;
    z->right = tree->nil;
    
    insertFixup(tree, z);
}

char* getUserDisplayname(RBTree* tree, char* user_id) {
    RBNode* x = tree->root;
    while (x != tree->nil) {
        int cmp = strcmp(user_id, x->user_id);
        if (cmp == 0) {
            return x->displayname;
        } else if (cmp < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    return NULL;
}

void freeRBNode(RBNode* node) {
    if (node->user_id) free(node->user_id);
    if (node->displayname) free(node->displayname);
    free(node);
}

void freeRBTreeHelper(RBTree* tree, RBNode* node) {
    if (node != tree->nil) {
        freeRBTreeHelper(tree, node->left);
        freeRBTreeHelper(tree, node->right);
        freeRBNode(node);
    }
}

void freeRBTree(RBTree* tree) {
    if (tree) {
        freeRBTreeHelper(tree, tree->root);
        free(tree->nil);
        free(tree);
    }
}

#endif // RB_TREE_H