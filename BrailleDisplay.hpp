#pragma once
#include <vector>
#include <QObject>
#include <QTimer>
#include <brlapi.h>
#include <QSocketNotifier>

#include "Display.hpp"

class BrailleDisplay40 : public QObject {
  // TODO: 40 characters, 1 line, blinking
  Q_OBJECT
private:
  std::vector<std::vector<Color> > carrier;
  bool fill;
  int cline = 0;
  int cblink = 0;
  QTimer timer;
  QSocketNotifier* notifier;
  brlapi_fileDescriptor fd;
  brlapi_connectionSettings_t brl_settings;
  brlapi_handle_t* bh;

  brlapi_keyCode_t key_up, key_down, key_left, key_right,
    key_rotate, key_undo, key_quit, key_nextstone,
    key_domove;


  int width, height;
  void nextLine() {
    if (++cline > height) --cline;
  }
  void prevLine () {
    if (--cline < 0) ++cline;
  }
  const int brl_flags[4][2] = {
    { 1 << 0, 1 << 3 },
    { 1 << 1, 1 << 4 },
    { 1 << 2, 1 << 5 },
    { 1 << 6, 1 << 7 }
  };
  unsigned char getBrailleCodeFrom (int x, int y);
  void blinkreset () {
    this->cblink = 0;
  }
  void loadConfiguration();

public:
  BrailleDisplay40 (int width, int height, bool fill, QObject * parent = 0);
  ~BrailleDisplay40 ();

signals:
  void keyStroke(Key k);

public slots:
  void setPixel (int x, int y, Color color);
  void notice (const std::string& str) {
    (void) str;
    // TODO
  }
  void refresh ();
  void tick () {
    cblink = (cblink+1)%4;
    refresh();
  }
  void readyToRead ();
};
