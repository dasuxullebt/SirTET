#include <ncurses.h>
#include <iostream>
#include <QSettings>
#include <unistd.h>
#include "CursesDisplay.hpp"

using namespace std;

void CursesDisplay::loadConfig() {
  QSettings settings;
  key_up = settings.value("curses/key/up", KEY_UP).toInt();
  key_down = settings.value("curses/key/down", KEY_DOWN).toInt();
  key_left = settings.value("curses/key/left", KEY_LEFT).toInt();
  key_right = settings.value("curses/key/right", KEY_RIGHT).toInt();
  key_rotate = settings.value("curses/key/rotate", 32).toInt();
  key_undo = settings.value("curses/key/undo", KEY_BACKSPACE).toInt();
  key_quit = settings.value("curses/key/quit", 27).toInt();
  key_nextstone = settings.value("curses/key/nextstone", 9).toInt();
  key_domove = settings.value("curses/key/domove", 13).toInt();
  //TODO  tick = settings.value("braille/tick", 2).toInt();

  stone_fits_full = settings.value("curses/ui/stone_fits_full", stone_fits_full).toChar();
  stone_fits_empty = settings.value("curses/ui/stone_fits_empty", stone_fits_empty).toChar();
  stone_doesnt_fit_full = settings.value("curses/ui/stone_doesnt_fit_full", stone_doesnt_fit_full).toChar();
  stone_doesnt_fit_empty = settings.value("curses/ui/stone_doesnt_fit_empty", stone_doesnt_fit_empty).toChar();
  stone_empty = settings.value("curses/ui/stone_empty", stone_empty).toChar();
  stone_filled = settings.value("curses/ui/stone_filled", stone_filled).toChar();

  QString xgr(settings.value("curses/gravity", "up-down").toString());
  if (xgr == "up-down") {
    gr = GR_UP_DOWN;
  } else if (xgr == "left-right") {
    gr = GR_LEFT_RIGHT;
  } else if (xgr == "down-up") {
    gr = GR_DOWN_UP;
  } else if (xgr == "right-left") {
    gr = GR_RIGHT_LEFT;
  } else {
    Q_ASSERT(0); // TODO
  }

  // translate keys, TODO: hack
  int keys[] = {key_left, key_up, key_right, key_down};
  int* skeys[] = {&key_left, &key_up, &key_right, &key_down};

  for (int i = 0; i < 4; ++i) {
    *(skeys[i]) = keys[(i + gr) % 4];
  } 
}

CursesDisplay::CursesDisplay (int transx, int transy, int width, int height) :
  transx(transx), transy(transy), width(width), height(height)
{
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  nonl();
  cbreak();
  timeout(0);
  //halfdelay(1); // TODO
  curs_set(0);

  loadConfig();

  // fill initially with "empty" character
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      setPixel(i, j, EMPTY);
    }
  }

  notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
  connect(notifier, SIGNAL(activated(int)), this, SLOT(readyToRead()));
}

CursesDisplay::~CursesDisplay () {
  endwin();
}

void CursesDisplay::setPixel (int x, int y, Color color) {
  translateXY(x, y, width, height);
  wmove (stdscr, x + transx, y + transy);
  // todo: unicode statt ascii
  switch (color) {
  case FITS_FULL:
    waddch (stdscr, stone_fits_full.toLatin1());
    break;
  case FITS_EMPTY:
    waddch (stdscr, stone_fits_empty.toLatin1());
    break;
  case DOESNT_FIT_FULL:
    waddch (stdscr, stone_doesnt_fit_full.toLatin1());
    break;
  case DOESNT_FIT_EMPTY:
    waddch (stdscr, stone_doesnt_fit_empty.toLatin1());
    break;
  case EMPTY:
    waddch (stdscr, stone_empty.toLatin1());
    break;
  case FILLED:
    waddch (stdscr, stone_filled.toLatin1());
    break;
  default:
    abort();
  }
}

void CursesDisplay::refresh () {
  wrefresh(stdscr);
}

void CursesDisplay::notice (const string& x) {
  mvwaddstr (stdscr, width, height, x.c_str());
}

void CursesDisplay::readyToRead() {
  int c =  getch();
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
