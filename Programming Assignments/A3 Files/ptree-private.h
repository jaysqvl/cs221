/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////

// frees the object referenced by the given pointer using post order traversal
void DestroyNode(Node * r);

// Recursively counts every node beginning from the given node
int SizeHelper(const Node * n) const;

// 
void RenderHelper(PNG *& pngpointer, const Node * n) const;

// Recursively counts every node in the tree which has two NULLs indicating it's a leaf
int LeafHelper(const Node * n) const;

// Copy recursive helper which rebuilds/copys the given node into our current PTree structure
void CopyNode(const Node * copy, Node *& paste);

// Flips a given png image horizontally
void HFlipHelper(PNG *& pngpointer);

// Flips a given png image vertically
void VFlipHelper(PNG *& pngpointer);

void PruneHelper(Node * n, double tolerance);

bool Prunable(Node * n, Node * x, double tolerance);

#endif