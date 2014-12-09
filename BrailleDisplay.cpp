#include "BrailleDisplay.hpp"

#include <iostream>
using namespace std;
BrailleDisplay40::BrailleDisplay40 (int width, int height, bool fill, QObject * parent) : QObject(parent), width(width), height(height), fill(fill) {
  bh = (brlapi_handle_t*) malloc (brlapi_getHandleSize());
  fd = brlapi__openConnection(bh, NULL, &settings);
  if (fd < 0) {
    throw fd;
  }
  brlapi__enterTtyMode(bh, BRLAPI_TTY_DEFAULT, NULL);
  carrier.resize(width);
  for (auto &x : carrier) {
    x.resize(height);
    for (auto &y : x) {
      y = Color::EMPTY;
    }
  }
  connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
  timer.start(250);
}
BrailleDisplay40::~BrailleDisplay40 () {
  brlapi__leaveTtyMode(bh);
  brlapi__closeConnection(bh);
  free(bh);
}

void BrailleDisplay40::setPixel (int x, int y, Color color) {
  Q_ASSERT(0 <= y);
  Q_ASSERT(0 <= x);
  Q_ASSERT(y < height);
  Q_ASSERT(x < width);
  carrier[x][y] = color;
}
unsigned char BrailleDisplay40::getBrailleCodeFrom (int x, int y) {
  unsigned char ret = 0x00; //fill ? 0xFF : 0x00;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      int k = x + i;
      int l = y + j;
      if ((0 <= k) && (k < width) && (0 <= l) && (l < height)) {
	//	  translateXY(k, l, carrier.width, carrier.height);
	switch (carrier[k][l]) {
	case EMPTY:
	  break;
	case FILLED:
	  ret |= brl_flags[j][i];
	  break;
	case FITS_EMPTY:
	case FITS_FULL:
	  if (cblink % 4 < 2) ret |= brl_flags[j][i];
	  break;
	case DOESNT_FIT_FULL:
	case DOESNT_FIT_EMPTY:
	  if (cblink % 2 == 0) ret |= brl_flags[j][i];
	  break;
	}
      } else if (fill) {
	ret |= brl_flags[j][i];
      }
    }
  }
  return ret;
}

void BrailleDisplay40::refresh () {
  unsigned char dots[40];
  for (int i = 0; i < 40; ++i) {
    dots[i] = getBrailleCodeFrom(2*i, cline);
  }
  brlapi__writeDots(bh, dots);
}
