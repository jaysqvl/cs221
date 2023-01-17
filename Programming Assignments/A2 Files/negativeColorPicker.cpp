/*
*  File: negativeColorPicker.cpp
*  Implements the negative color picker for CPSC 221 PA2.
*
*/

#include "negativeColorPicker.h"

NegativeColorPicker::NegativeColorPicker(PNG& inputimg)
{
  // complete your implementation below
  img = inputimg;
}

HSLAPixel NegativeColorPicker::operator()(PixelPoint p)
{
  // complete your implementation below
  HSLAPixel* pixel = img.getPixel(p.x, p.y);

  double ph, ps, pl, pa;
  ph = pixel->h;
  ps = pixel->s;
  pl = 1.0 - pixel->l;
  pa = pixel->a;

  if (ph < 180.0) {
    ph += 180.0;
  } else {
    ph -= 180.0;
  }

  HSLAPixel ans = HSLAPixel(ph, ps, pl, pa);

  return ans;
}
