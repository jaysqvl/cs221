/*
*  File: imageTileColorPicker.cpp
*  Implements the image tile color picker for CPSC 221 PA2.
*
*/

#include "imageTileColorPicker.h"

ImageTileColorPicker::ImageTileColorPicker(PNG& otherimage) {
  // complete your implementation below
  img_other = otherimage;
}

HSLAPixel ImageTileColorPicker::operator()(PixelPoint p) {
  // complete your implementation below
  // Didn't use variables to minimize memory usage
  HSLAPixel* temp = img_other.getPixel(p.x % img_other.width(), p.y % img_other.height());

  double hue,sat,lue,al;
  hue = temp->h;
  sat = temp->s;
  lue = temp->l;
  al = temp->a;

  return HSLAPixel(hue, sat, lue, al);
}