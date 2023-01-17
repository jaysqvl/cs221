/*
*  File: censorColorPicker.cpp
*  Implements the censor color picker for CPSC 221 PA2.
*
*/

#include "censorColorPicker.h"

#include <math.h> // gives access to sqrt function

/*
*  Useful function for computing the Euclidean distance between two PixelPoints
*/
double PointDistance(PixelPoint a, PixelPoint b) {
  unsigned int diff_x, diff_y;
  if (a.x > b.x)
    diff_x = a.x - b.x;
  else
    diff_x = b.x - a.x;
  if (a.y > b.y)
    diff_y = a.y - b.y;
  else
    diff_y = b.y - a.y;
  return sqrt(diff_x * diff_x + diff_y * diff_y);
}

CensorColorPicker::CensorColorPicker(unsigned int b_width, PixelPoint ctr, unsigned int rad, PNG& inputimage) {
  blockwidth = b_width;
  center = ctr;
  radius = rad;
  img = inputimage;

  int new_width = img.width();
  int new_height = img.height();

  // make new placeholder blockyimg
  PNG blocky;
  blocky.resize(new_width, new_height);

  // initialize new vars
  double hue,sat,lue,alp;

  for (int y = 0; y < new_height; y += blockwidth) { // iterates over mosaic border
    for (int x = 0; x < new_width; x += blockwidth) {
      HSLAPixel holder = avgBlockVals(blockwidth, x, y);
      hue = holder.h;
      sat = holder.s;
      lue = holder.l;
      alp = holder.a;

      for(int ysub = y; ysub < y + blockwidth && ysub < new_height; ysub++) { // fills within mosaic border
        for (int xsub = x; xsub < x + blockwidth && xsub < new_width; xsub++) {
          HSLAPixel* temp = blocky.getPixel(xsub,ysub);

          temp->h = hue;
          temp->s = sat;
          temp->l = lue;
          temp->a = alp;
        }
      }
    }

  }

  blockyimg = blocky;
}

HSLAPixel CensorColorPicker::operator()(PixelPoint p)
{
  // complete your implementation below
  int x = p.x;
  int y = p.y;

  double hue,sat,lue,alp, distance;
  distance = PointDistance(p,center);
  
  HSLAPixel* temp;

  if (distance <= radius) {
    temp = blockyimg.getPixel(x,y);
  } else {
    temp = img.getPixel(x,y);
  }

  hue = temp->h;
  sat = temp->s;
  lue = temp->l;
  alp = temp->a;

  return HSLAPixel(hue,sat,lue,alp);
}

// EFFECTS: given a block and its position, finds the average pixel values within the whole block
HSLAPixel CensorColorPicker::avgBlockVals(unsigned int block, int x, int y) {
  // initialize vars
  double hue, sat, lue, alp, total;
  hue = 0.0;
  sat = 0.0;
  lue = 0.0;
  alp = 0.0;
  total = 0.0;

  int height = img.height();
  int width = img.width();

  for (int ysub = y; ysub < y + block && ysub < height; ysub++) {
    for (int xsub = x; xsub < x + block && xsub < width; xsub++) {
      HSLAPixel* temp = img.getPixel(xsub,ysub);

      hue += temp->h;
      sat += temp->s;
      lue += temp->l;
      alp += temp->a;
      total += 1.0;
    }
  }

  hue /= total;
  sat /= total;
  lue /= total;
  alp /= total;

  return HSLAPixel(hue,sat,lue,alp);
}
