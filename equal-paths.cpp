#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here
bool equalH(Node* root, int currentLength, int& depth)
{
    if (root == NULL)
    {
        return true;
    }

    if (root->left == NULL && root->right == NULL) // if it's a leaf node, compare its depth to the first leaf
    {
        if (depth == -1)
        {
            depth = currentLength; // store depth of the first leaf you find
        }
        return (depth == currentLength); // compare this leaf's depth to the first
    }

    // check left and right children recursively
    bool left = equalH(root->left, currentLength + 1, depth);
    bool right = equalH(root->right, currentLength + 1, depth);

    
    return left && right; // return true only if both sides matched the expected depth
}

bool equalPaths(Node * root)
{
    int depth = -1;      // use this to store the depth of the first leaf
    int length = 0;      // track the depth as you recurse down
    bool result = true;  // will hold the final result

    result = equalH(root, length, depth);
    return result;
}


