#include "BrailleDisplay.hpp"
#include <brlapi.h>
#include <brltty/brldefs.h>
#include <QSettings>
#include <iostream>

using namespace std;
BrailleDisplay40::BrailleDisplay40 (int width, int height, bool fill, QObject * parent) : QObject(parent), width(width), height(height), fill(fill) {
  bh = (brlapi_handle_t*) malloc (brlapi_getHandleSize());
  fd = brlapi__openConnection(bh, NULL, &brl_settings);
  if (fd < 0) {
    throw fd;
  }
  if (brlapi__enterTtyMode(bh, BRLAPI_TTY_DEFAULT, NULL) == -1) throw fd;
  loadConfiguration();
  carrier.resize(width);
  for (auto &x : carrier) {
    x.resize(height);
    for (auto &y : x) {
      y = Color::EMPTY;
    }
  }
  notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
  connect(notifier, SIGNAL(activated(int)), this, SLOT(readyToRead()));
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

void BrailleDisplay40::readyToRead () {
  brlapi_keyCode_t c;
  while (brlapi__readKey(bh, 0, &c)) {
    if (c == key_down) {
      emit keyStroke(K_DOWN);
    } else if (c == key_up) {
      emit keyStroke(K_UP);
    }  else if (c == key_left) {
      emit keyStroke(K_LEFT);
    }  else if (c == key_right) {
      emit keyStroke(K_RIGHT);
    }  else if (c == key_rotate) {
      emit keyStroke(K_ROTATE);
    }  else if (c == key_undo) {
      emit keyStroke(K_UNDO);
    }  else if (c == key_quit) {
      emit keyStroke(K_QUIT);
    }  else if (c == key_nextstone) {
      emit keyStroke(K_NEXTSTONE);
    }  else if (c == key_domove) {
      emit keyStroke(K_DOMOVE);
    }
  }
}

void BrailleDisplay40::loadConfiguration() {
  QSettings settings;
  key_up = settings.value("brltty/key/up", (qulonglong)BRLAPI_KEY_SYM_UP).toULongLong();
  key_down = settings.value("brltty/key/down", (qulonglong)BRLAPI_KEY_SYM_DOWN).toULongLong();
  key_left = settings.value("brltty/key/left", (qulonglong)BRLAPI_KEY_SYM_LEFT).toULongLong();
  key_right = settings.value("brltty/key/right", (qulonglong)BRLAPI_KEY_SYM_RIGHT).toULongLong();
  key_rotate = settings.value("brltty/key/rotate", (qulonglong)BRLAPI_KEY_SYM_INSERT).toULongLong();
  key_undo = settings.value("brltty/key/undo", (qulonglong)BRLAPI_KEY_SYM_BACKSPACE).toULongLong();
  key_quit = settings.value("brltty/key/quit", (qulonglong)BRLAPI_KEY_SYM_ESCAPE).toULongLong();
  key_nextstone = settings.value("brltty/key/nextstone", (qulonglong)BRLAPI_KEY_SYM_TAB).toULongLong();
  key_domove = settings.value("brltty/key/domove", (qulonglong)BRLAPI_KEY_SYM_LINEFEED).toULongLong();
  //TODO  tick = settings.value("braille/tick", 2).toInt();

}
