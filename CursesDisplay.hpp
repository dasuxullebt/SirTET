#pragma once

#include "GameState.hpp"
#include "Display.hpp"
#include <string>
#include <QChar>
#include <QSocketNotifier>

class CursesDisplay : public QObject {
  Q_OBJECT
private:
  int transx, transy, width, height;
  enum gravity {
    GR_RIGHT_LEFT = 0,
    GR_DOWN_UP = 1,
    GR_LEFT_RIGHT = 2,
    GR_UP_DOWN = 3
  };

  gravity gr;

  void translateWH (int& w, int& h) {
    if (gr % 2 == 1) std::swap(w, h);
  }
  void translateXY
  (int &x, int &y, int w, int h) {
    int xys[] = {x, y, w-x, h-y};
    x = xys[gr % 4];
    y = xys[(1 + gr) % 4];
  }

  QChar stone_fits_full = '@';
  QChar stone_fits_empty = '=';
  QChar stone_doesnt_fit_full = 'X';
  QChar stone_doesnt_fit_empty = 'O';
  QChar stone_empty = '.';
  QChar stone_filled = '#';

  void loadConfig();
  QSocketNotifier* notifier;

public:
  int key_up, key_down, key_left, key_right,
    key_rotate, key_undo, key_quit, key_nextstone,
    key_domove;
  CursesDisplay (int transx, int transy, int width, int height);
  ~CursesDisplay ();
signals:
  void keyStroke(Key k);
public slots:
  void readyToRead();
  void setPixel (int x, int y, Color color);
  void notice (const std::string& x);
  void refresh ();
};
