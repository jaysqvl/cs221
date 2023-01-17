// File:        imglist.cpp
// Date:        2022-01-27 10:21
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//

#include "imglist.h"

#include <math.h> // provides fmax, fmin, and fabs functions

/**************************
* MISCELLANEOUS FUNCTIONS *
**************************/

/*
* This function is NOT part of the ImgList class,
* but will be useful for one of the ImgList functions.
* Returns the "difference" between two hue values.
* PRE: hue1 is a double between [0,360).
* PRE: hue2 is a double between [0,360).
* 
* The hue difference is the absolute difference between two hues,
* but takes into account differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
double HueDiff(double hue1, double hue2) {
  return fmin(fabs(hue1 - hue2), fabs(360 + fmin(hue1, hue2) - fmax(hue1, hue2)));
}

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/*
* Default constructor. Makes an empty list
*/
ImgList::ImgList() {
  // set appropriate values for all member attributes here
  northwest = NULL;
  southeast = NULL;
}

/*
* Creates a list from image data
* PRE: img has dimensions of at least 1x1
*/
ImgList::ImgList(PNG& img) {
  // build the linked node structure and set the member attributes appropriately
  northwest = NULL;
  southeast = NULL;
  
  // will point to the new row that needs to be connected
  ImgNode * currHead = NULL;

  //northwest instantiated
  northwest = makeRow(img, 0);

  // will point to the last row instantiated
  ImgNode * lastHead = northwest;
  
  for (unsigned int y = 1; y < img.height(); y++) {
    // currently created row needs to connect to last connected row
    currHead = makeRow(img, y);
    connectRow(lastHead, currHead);
    lastHead = currHead;
  }
}

/*
* Copy constructor.
* Creates this this to become a separate copy of the data in otherlist
*/
ImgList::ImgList(const ImgList& otherlist) {
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
* Assignment operator. Enables statements such as list1 = list2;
*   where list1 and list2 are both variables of ImgList type.
* POST: the contents of this list will be a physically separate copy of rhs
*/
ImgList& ImgList::operator=(const ImgList& rhs) {
  // Re-build any existing structure using rhs as a template
  
  if (this != &rhs) { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();
    
    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }
  
  return *this;
}

/*
* Destructor.
* Releases any heap memory associated with this list.
*/
ImgList::~ImgList() {
  // Ensure that any existing heap memory is deallocated
  Clear();
}

/************
* ACCESSORS *
************/

/*
* Returns the horizontal dimension of this list (counted in nodes)
* Note that every row will contain the same number of nodes, whether or not
*   the list has been carved.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionX() const {
  // replace the following line with your implementation
  ImgNode * temp = northwest;
  if (temp == NULL) {
    return 0;
  }
  int count = 1;
  while(temp->east != NULL) {
    count++;
    temp = temp->east;
  }

  return count;
}

/*
* Returns the vertical dimension of the list (counted in nodes)
* It is useful to know/assume that the grid will never have nodes removed
*   from the first or last columns. The returned value will thus correspond
*   to the height of the PNG image from which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   y dimension.
*/
unsigned int ImgList::GetDimensionY() const {
  // replace the following line with your implementation
  ImgNode * temp = northwest;
  if (temp == NULL) {
    return 0;
  }
  unsigned count = 1;
  while(temp->south != NULL) {
    count++;
    temp = temp->south;
  }

  return count;
}

/*
* Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
* The returned value will thus correspond to the width of the PNG image from
*   which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionFullX() const {
  // replace the following line with your implementation
  
  // we just sum up all the skiprights into count
  ImgNode * temp = northwest;
  if (temp == NULL) {
    return 0;
  }
  unsigned count = 1;
  while(temp->east != NULL) {
    count++;
    count += temp->skipright;
    temp = temp->east;
  }

  return count;
}

/*
* Returns a pointer to the node which best satisfies the specified selection criteria.
* The first and last nodes in the row cannot be returned.
* PRE: rowstart points to a row with at least 3 physical nodes
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rowstart - pointer to the first node in a row
* PARAM: selectionmode - criterion used for choosing the node to return
*          0: minimum luminance across row, not including extreme left or right nodes
*          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
*        In the (likely) case of multiple candidates that best match the criterion,
*        the left-most node satisfying the criterion (excluding the row's starting node)
*        will be returned.
* A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
* That is, a hue value of exactly 360 should be converted to 0.
* The hue difference is the absolute difference between two hues,
* but be careful about differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
  // add your implementation below

  // begin at the second node (we assume that it exists)
  ImgNode * curr = rowstart->east;
  ImgNode * min = curr;

  // fill mode 0 (min luminance across row)
  if (selectionmode == 0) {
    while(curr->east != NULL) {
      if (curr->colour.l < min->colour.l) {
        min = curr;
      }
      curr = curr->east;
    }
  } else if (selectionmode == 1) {
    while(curr->east != NULL) {
      if (HueDiff(curr->west->colour.h, curr->colour.h) + HueDiff(curr->colour.h, curr->east->colour.h) < 
      HueDiff(min->west->colour.h, min->colour.h) + HueDiff(min->colour.h, min->east->colour.h)) {
        min = curr;
      }
      curr = curr->east;
    }
  }

  curr = nullptr;

  return min;
}

/*
* Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
* PRE: fillmode is an integer in the range of [0,2]
* PARAM: fillgaps - whether or not to fill gaps caused by carving
*          false: render one pixel per node, ignores fillmode
*          true: render the full width of the original image,
*                filling in missing nodes using fillmode
* PARAM: fillmode - specifies how to fill gaps
*          0: solid, uses the same colour as the node at the left of the gap
*          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
*             Note that "average" for hue will use the closer of the angular distances,
*             e.g. average of 10 and 350 will be 0, instead of 180.
*             Average of diametric hue values will use the smaller of the two averages
*             e.g. average of 30 and 210 will be 120, and not 300
*                  average of 170 and 350 will be 80, and not 260
*          2: *** OPTIONAL - FOR BONUS ***
*             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
*             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
*             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
*             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
*             Like fillmode 1, use the smaller difference interval for hue,
*             and the smaller-valued average for diametric hues
*/
PNG ImgList::Render(bool fillgaps, int fillmode) const {
  // Add/complete your implementation below
  PNG outpng;
  PNG * pngpointer = &outpng;

  
  // fillmode solid
  if (fillgaps) {
    if (fillmode == 0) {
      RenderSolid(pngpointer);
    } else if (fillmode == 1) {
      RenderSolidAverage(pngpointer);
    } else if (fillmode == 2) {
      RenderGradient(pngpointer);
    }
  } else {
    RenderIgnoreFill(pngpointer);
  }
  
  pngpointer = nullptr;
  return outpng;
}

/************
* MODIFIERS *
************/

/*
* Removes exactly one node from each row in this list, according to specified criteria.
* The first and last nodes in any row cannot be carved.
* PRE: this list has at least 3 nodes in each row
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: selectionmode - see the documentation for the SelectNode function.
* POST: this list has had one node removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(int selectionmode) {
  /*
  Basically we just want to find the special node in each row based on the selectionmode and delete it
  
  Things to update: Nodes directly next to the special node (prev and next)(skip values) and then delete the special node
  
  the plan:
  PER ROW
    use selectnode to return us the special node
    update its neighbors to point at each other as well as update their skip values
    delete the special node

  NOTE: selectionmode 0 == the least luminance node gets removed
        selectionmode 1 == the least HueDiff node gets removed
  */

  if (GetDimensionX() <= 2 || !(selectionmode == 0 || selectionmode == 1)) {
    return;
  }

  ImgNode * currRow = northwest;

  while(currRow != NULL) {
    ImgNode * toDelete = SelectNode(currRow, selectionmode);
    
    // Update it's left neighbor
    toDelete->west->east = toDelete->east;
    toDelete->west->skipright += 1;

    // Update it's right neighbor
    toDelete->east->west = toDelete->west;
    toDelete->east->skipleft += 1;

    // Get rid of the value inside that node
    delete toDelete;
    toDelete = nullptr;

    currRow = currRow->south;
  }
}

// note that a node on the boundary will never be selected for removal
/*
* Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
* based on specific selection criteria.
* Note that this should remove one node from every row, repeated "rounds" times,
* and NOT remove "rounds" nodes from one row before processing the next row.
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rounds - number of nodes to remove from each row
*        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
*        i.e. Ensure that the final list has at least two nodes in each row.
* POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(unsigned int rounds, int selectionmode) {
  // add your implementation here
  unsigned int startdim = GetDimensionX();
  unsigned int count = 0;
  if (rounds > startdim - 2) {
    rounds = startdim - 2;
  }

  if (GetDimensionX() <= 2 || !(selectionmode == 0 || selectionmode == 1)) {
    return;
  }

  // how to get carve to run if we input more than the allowed.
  // count = 0; rounds = 2; then we want to run from count = 0 to rounds
  // if rounds > startdim - 2 rounds = startdim - 2


  while (count < rounds && GetDimensionX() > 2) {
    Carve(selectionmode);
    count++;
  }
}


/*
* Helper function deallocates all heap memory associated with this list,
* puts this list into an "empty" state. Don't forget to set your member attributes!
* POST: this list has no currently allocated nor leaking heap memory,
*       member attributes have values consistent with an empty list.
*/
void ImgList::Clear() {
  // add your implementation here
  
  // base case (finished if northwest successfully = NULL)
  if (northwest == NULL) {
    return;
  }
  
  while (northwest != NULL) {
    ImgNode * temp = northwest;
    northwest = northwest->south;
    deleteTopRow(temp);
    temp = NULL;
  }

  northwest = NULL;
  southeast = NULL;
}

/* ************************
*  * OPTIONAL - FOR BONUS *
** ************************
* Helper function copies the contents of otherlist and sets this list's attributes appropriately
* PRE: this list is empty
* PARAM: otherlist - list whose contents will be copied
* POST: this list has contents copied from by physically separate from otherlist
*/
void ImgList::Copy(const ImgList& otherlist) {
  // add your implementation here
   
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/

// ****************
// CONSTRUCTOR HELPERS
// ****************

// Builds forwards
// EFFECTS: Makes a row of doubly linked nodes and returns a pointer to the head of the first node created
ImgNode* ImgList::makeRow(PNG& img, unsigned int y) {
  ImgNode * head = new ImgNode();
  head->colour = *(img.getPixel(0, y));

  ImgNode * curr = head;

  for (unsigned x = 1; x < img.width(); x++) {
    ImgNode * next = new ImgNode();
    next->colour = *(img.getPixel(x, y));
    curr->east = next;
    next->west = curr;
    curr = next;
  }

  // updates southeast with the last created node
  southeast = curr;

  return head;
}

// EFFECTS: connects head1 and head2, sets southeast equal to the last processed node
void ImgList::connectRow(ImgNode* head1, ImgNode* head2) {
  // if we reach the end of the row, stop
  if (head1 == NULL) {
    return;
  }
  
  // else we continue to connect the nodes
  head1->south = head2;
  head2->north = head1;
  connectRow(head1->east, head2->east);
}

// ****************
// CLEAR HELPERS
// ****************

// deletes the entire row
void ImgList::deleteTopRow(ImgNode* head) {
  // if head == NULL we completely deleted every single element in the row
  while (head != NULL) {
    ImgNode * curr = head;
    head = head->east;
    delete curr;
    curr = head;
  }
  head = NULL;
}

// ****************
// RENDER HELPERS
// ****************

void ImgList::RenderSolid(PNG *& pngpointer) const {
  // if skipright > 0 then for (int rounds = 0; rounds < skipright; rounds++)
  // run this and update x only if the current node has a skipright

  pngpointer->resize(GetDimensionFullX(), GetDimensionY());
  ImgNode * row = northwest;

  // change the pixels of all the rows
  for (unsigned int y = 0; y < GetDimensionY(); y++) {
    // change the pixels of a whole row
    ImgNode * toCopy = row;

    unsigned int x = 0;
    while(toCopy != NULL) {
      // Sets curr pixel to the current pixel address
      HSLAPixel * currPixel = pngpointer->getPixel(x, y); // PNG

      // update the pixel
      currPixel->h = toCopy->colour.h;
      currPixel->s = toCopy->colour.s;
      currPixel->l = toCopy->colour.l;
      currPixel->a = toCopy->colour.a;

      unsigned int rounds = toCopy->skipright;
      while (rounds > 0) {
        rounds -= 1;
        x++;
        HSLAPixel * nextPixel = pngpointer->getPixel(x, y);

        nextPixel->h = toCopy->colour.h;
        nextPixel->s = toCopy->colour.s;
        nextPixel->l = toCopy->colour.l;
        nextPixel->a = toCopy->colour.a;
      }
      
      // move along the row
      toCopy = toCopy->east;
      x++;
    }
    row = row->south;
  }
}

void ImgList::RenderSolidAverage(PNG *& pngpointer) const {
    // if skipright > 0 then for (int rounds = 0; rounds < skipright; rounds++)
  // run this and update x only if the current node has a skipright

  pngpointer->resize(GetDimensionFullX(), GetDimensionY());
  ImgNode * row = northwest;

  // change the pixels of all the rows
  for (unsigned int y = 0; y < GetDimensionY(); y++) {
    // change the pixels of a whole row
    ImgNode * toCopy = row;

    unsigned int x = 0;
    while(toCopy != NULL) {
      // Sets curr pixel to the current pixel address
      HSLAPixel * currPixel = pngpointer->getPixel(x, y); // PNG

      // update the pixel
      currPixel->h = toCopy->colour.h;
      currPixel->s = toCopy->colour.s;
      currPixel->l = toCopy->colour.l;
      currPixel->a = toCopy->colour.a;

      unsigned int rounds = toCopy->skipright;
      while (rounds > 0) {
        // Update the pixelpointer to point at the gap 
        rounds -= 1;
        x++;
        HSLAPixel * nextPixel = pngpointer->getPixel(x, y);
        
        // Store the average in an HSLAPixel
        HSLAPixel * avgPixel = AverageGap(toCopy);

        nextPixel->h = avgPixel->h;
        nextPixel->s = avgPixel->s;
        nextPixel->l = avgPixel->l;
        nextPixel->a = avgPixel->a;
        delete avgPixel;
        avgPixel = nullptr;
      }
      
      // move along the row
      toCopy = toCopy->east;
      x++;
    }
    row = row->south;
  }
}

// takes in a node and returns the average colour values between it and the node directly to its right
// PRE: Next node will never be null because there cannot be a gap at the end or beginning of each column
HSLAPixel * ImgList::AverageGap(ImgNode* gapNode) const {
  // Diametric case
  double newHue = 0;
  ImgNode * next = gapNode->east;

  double temp1 = fmin(gapNode->colour.h, next->colour.h);
  double temp2 = fmax(gapNode->colour.h, next->colour.h);

  if (HueDiff(gapNode->colour.h, next->colour.h) == 180) { // Diametric case always choose the smaller angle (aka the min value between adding the two up and dividing)
    newHue = fmin(fmod((((temp1 + 360) + temp2) / 2.0), 360.0), ((gapNode->colour.h + next->colour.h) / 2.0));
  } else if (fabs(gapNode->colour.h - next->colour.h) > 180) { // Angular case always choose the smaller angle (aka the min difference)
    newHue = fmod((((temp1 + 360) + temp2) / 2.0), 360.0);
  } else {
    newHue = (gapNode->colour.h + next->colour.h) / 2.0;
  }
  
  HSLAPixel * avgPixel = new HSLAPixel();
  avgPixel->h = newHue;
  avgPixel->s = (gapNode->colour.s + next->colour.s) / 2.0;
  avgPixel->l = (gapNode->colour.l + next->colour.l) / 2.0;
  avgPixel->a = (gapNode->colour.a + next->colour.a) / 2.0;
  return avgPixel;
}

void ImgList::RenderGradient(PNG *& pngpointer) const {
  // TODO
  pngpointer->resize(GetDimensionFullX(), GetDimensionY());
}

void ImgList::RenderIgnoreFill(PNG *& pngpointer) const {
  pngpointer->resize(GetDimensionX(), GetDimensionY());
  ImgNode * row = northwest;

  // change the pixels of all the rows
  for (unsigned y = 0; y < GetDimensionY(); y++) {
    // change the pixels of a whole row
    ImgNode * toCopy = row;
    int x = 0;
    while(toCopy != NULL) {
      // Sets curr pixel to the current pixel address
      HSLAPixel * currPixel = pngpointer->getPixel(x, y); // PNG

      // update the pixel
      currPixel->h = toCopy->colour.h;
      currPixel->s = toCopy->colour.s;
      currPixel->l = toCopy->colour.l;
      currPixel->a = toCopy->colour.a;

      // move along the row
      toCopy = toCopy->east;
      x++;
    }
    row = row->south;
  }
}