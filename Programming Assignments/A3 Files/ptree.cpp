/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  // add your implementation below
  DestroyNode(root);
}

/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  // add your implementation below
  CopyNode(other.root, root);
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  // replace the line below with your implementation
  Node * newNode = new Node();
  
  int ulX = ul.first;
  int ulY = ul.second;
  newNode->upperleft = ul;
  newNode->width = w;
  newNode->height = h;

  // leaf node case no avg
  if (w == 1 && h == 1) {
    HSLAPixel * leafPix = im.getPixel(ulX, ulY);
    newNode->avg.h = leafPix->h;
    newNode->avg.s = leafPix->s;
    newNode->avg.l = leafPix->l;
    newNode->avg.a = leafPix->a;

    newNode->A = NULL;
    newNode->B = NULL;
    return newNode;
  }

  // avg pixel over region
  double newAvgH = 0.0;
  double newHueX = 0.0;
  double newHueY = 0.0;
  double newS = 0.0;
  double newL = 0.0;
  double newA = 0.0;
  double count = 0.0;


  for (unsigned int x = ulX; x < ulX + w; x++) {
    for (unsigned int y = ulY; y < ulY + h; y++) {
      HSLAPixel * currPixel = im.getPixel(x, y);
      newHueX += Deg2X(currPixel->h);
      newHueY += Deg2Y(currPixel->h);
      newS += currPixel->s;
      newL += currPixel->l;
      newA += currPixel->a;
      count += 1.0;
    }
  }

  newHueX /= count;
  newHueY /= count;
  newAvgH = XY2Deg(newHueX, newHueY);

  newNode->avg.h = newAvgH;
  newNode->avg.s = newS / count;
  newNode->avg.l = newL / count;
  newNode->avg.a = newA / count;
  // avg pixel end

  // recursive width and height cases
  if (h > w) { // vertical divide
    int aHeight = h/2; // will always be less due to integer division
    int bHeight = 0.0;
    pair<unsigned int, unsigned int> aPair = ul;
    pair<unsigned int, unsigned int> bPair;

    bPair.first = ulX; // X of upperleft inputted
    bPair.second = ulY + aHeight; // Y of upperleft + h/2 (aHeight will always be h/2)

    if (h % 2 == 0) {
      bHeight = aHeight;
    } else {
      bHeight = h - aHeight;
    }

    newNode->A = BuildNode(im, aPair, w, aHeight);
    newNode->B = BuildNode(im, bPair, w, bHeight);
  } else { // horizontal divide when equal or wider than tall
    int aWidth = w/2;
    int bWidth = 0.0;
    pair<unsigned int, unsigned int> aPair = ul;
    pair<unsigned int, unsigned int> bPair;

    bPair.first = ulX + aWidth; // X of upperleft of left + w/2 (aWidth will always be w/2)
    bPair.second = ulY; // Y of upperleft

    if (w % 2 == 0) {
      bWidth = aWidth;
    } else {
      bWidth = w - aWidth;
    }

    newNode->A = BuildNode(im, aPair, aWidth, h);
    newNode->B = BuildNode(im, bPair, bWidth, h);
  }

  return newNode;
}

////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  // add your implementation below
  pair<unsigned int, unsigned int> startPos = {0, 0}; // upper left of any image
  root = BuildNode(im, startPos, im.width(), im.height());
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  // add your implementation below
  Copy(other);
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  // add your implementation below
  if (other.root != root) {
    Clear();
    Copy(other);
  }
  return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  // add your implementation below
  Clear();
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  
  PNG outpng;
  PNG * pngpointer = &outpng;

  pngpointer->resize(root->width, root->height);
  RenderHelper(pngpointer, root);

  return outpng;
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  // add your implementation below
  PruneHelper(root, tolerance);
}

// Prunehelper iterates through the whole list of subtrees
void PTree::PruneHelper(Node * n, double tolerance) {
  if (n == NULL) {
    return;
  }
  
  // needs to be preorder because it needs to process highest subtree first
  if (Prunable(n, n, tolerance)) {
		DestroyNode(n);
	} else {
		PruneHelper(n->A, tolerance);
		PruneHelper(n->B, tolerance);
	}
}

// Given a node, will produce true if subtree's below it are prunable, false otherwise
bool PTree::Prunable(Node * n, Node * x, double tolerance) {
  if (x == NULL || n == NULL) {
    return false;
  }

  if(x->A == NULL && x->B == NULL) {
		return x->avg.dist(n->avg) <= tolerance;
	} else {
		return Prunable(x->A, x, tolerance) && Prunable(x->B, x, tolerance);
	}
}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  // replace the line below with your implementation

  return SizeHelper(root);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  // replace the line below with your implementation
  return LeafHelper(root);
}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  // add your implementation below
  // Essentially needs to flip A and B nodes in every node
  // HFlipHelper(root);

  PNG outpng = Render();
  PNG * pngpointer = &outpng;

  HFlipHelper(pngpointer);

  *this = PTree(outpng);
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  // add your implementation below
  PNG outpng = Render();
  PNG * pngpointer = &outpng;

  VFlipHelper(pngpointer);

  *this = PTree(outpng);
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

int PTree::SizeHelper(const Node * n) const {
   if (n == nullptr) {
     return 0;
   } else {
     return SizeHelper(n->A) + SizeHelper(n->B) + 1;
   }
}

int PTree::LeafHelper(const Node * n) const {
  if (n->A == nullptr && n->B == nullptr) {
    return 1;
  } else {
    return LeafHelper(n->A) + LeafHelper(n->B);
  }
}

void PTree::DestroyNode(Node * r) {
  if (r == NULL) {
    return;
  }

  Node * t = r;

  if (r->A != NULL || r->B != NULL) { // post order traversal
    DestroyNode(t->A);
    DestroyNode(t->B);
    delete r;
    t = nullptr;
  } else {
    delete r;
    t = nullptr;
  }
}

void PTree::RenderHelper(PNG *& pngpointer, const Node * n) const {
  
  // pre-order traversal
  if (n == NULL) {
    return;
  }

  pair<unsigned int, unsigned int> ul = n->upperleft;

  if (n->A == NULL && n->B == NULL) { // if it's a leaf, we want its data in our image
    HSLAPixel * currPixel = pngpointer->getPixel(ul.first, ul.second);
    currPixel->h = n->avg.h;
    currPixel->s = n->avg.s;
    currPixel->l = n->avg.l;
    currPixel->a = n->avg.a;
  }

  RenderHelper(pngpointer, n->A);
  RenderHelper(pngpointer, n->B);
}

void PTree::CopyNode(const Node * copy, Node *& paste) {
  
  /* Cases:
    copy = NULL, paste = smt || paste = nullptr
    copy = smt, paste = smt || paste = newNode(copy)
    copy = smt, paste = null || paste = newNode(copy)
    copy = null, paste = null || paste = nullptr
  */

  if (copy == nullptr) {
    paste = nullptr;
    return;
  }
  
  Node * newNode = new Node();

  pair<unsigned int, unsigned int> newUL;
  newUL.first = copy->upperleft.first;
  newUL.second = copy->upperleft.second;
  newNode->upperleft = newUL;

  newNode->width = copy->width;
  newNode->height = copy->height;

  newNode->avg = copy->avg;

  paste = newNode;

  newNode = nullptr;

  CopyNode(copy->A, paste->A);
  CopyNode(copy->B, paste->B);
}

void PTree::VFlipHelper(PNG *& pngpointer) {
  unsigned int imWidth = root->width;
  unsigned int imHeight = root->height;

  for (unsigned int x = 0; x < imWidth; x++) {
    for (unsigned int y = 0; y < imHeight/2; y++) {
      HSLAPixel * temp = new HSLAPixel();
      HSLAPixel * leftP = pngpointer->getPixel(x, y);
      temp->h = leftP->h;
      temp->s = leftP->s;
      temp->l = leftP->l;
      temp->a = leftP->a;

      HSLAPixel * rightP = pngpointer->getPixel(x, imHeight - y - 1);

      *leftP = *rightP;
      *rightP = *temp;
      delete temp;
    }
  }
}

void PTree::HFlipHelper(PNG *& pngpointer) {

  unsigned int imWidth = root->width;
  unsigned int imHeight = root->height;

  for (unsigned int x = 0; x < imWidth/2; x++) {
    for (unsigned int y = 0; y < imHeight; y++) {
      HSLAPixel * temp = new HSLAPixel();
      HSLAPixel * leftP = pngpointer->getPixel(x, y);
      temp->h = leftP->h;
      temp->s = leftP->s;
      temp->l = leftP->l;
      temp->a = leftP->a;

      HSLAPixel * rightP = pngpointer->getPixel(imWidth - x - 1, y);

      *leftP = *rightP;
      *rightP = *temp;
      delete temp;
    }
  }
}