#include <string>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <QSettings>
#include <QChar>
#include <QtGlobal>
#include <QCoreApplication>
#include <QSharedPointer>
#include "BoolField.hpp"
#include "GameState.hpp"
#include "CursesDisplay.hpp"
#include "BrailleDisplay.hpp"

using namespace std;
int main (int argc, char** argv) {

  QCoreApplication a(argc, argv);

  QCoreApplication::setOrganizationName("uxul.de");
  QCoreApplication::setOrganizationDomain("uxul.de");
  QCoreApplication::setApplicationName("SirTET");

  GameState field(10, 20);
  
  BrailleDisplay40* bdisp = NULL;
  try {
    bdisp = new BrailleDisplay40(10, 20, true, &a);
    QObject::connect(&field, SIGNAL(setPixel(int,int,Color)), bdisp, SLOT(setPixel(int,int,Color)));
    QObject::connect(&field, SIGNAL(notice(const std::string&)), bdisp, SLOT(notice(const std::string&)));
    QObject::connect(&field, SIGNAL(refresh()), bdisp, SLOT(refresh()));
  } catch (...) {
    cerr << "Could not open braille device" << endl;
  }

  CursesDisplay cdisp(0, 0, 10, 20);

  QObject::connect(&field, SIGNAL(setPixel(int,int,Color)), &cdisp, SLOT(setPixel(int,int,Color)));
  QObject::connect(&field, SIGNAL(notice(const std::string&)), &cdisp, SLOT(notice(const std::string&)));
  QObject::connect(&field, SIGNAL(refresh()), &cdisp, SLOT(refresh()));
  QObject::connect(&cdisp, SIGNAL(keyStroke(Key)), &field, SLOT(keyStroke(Key)));

  for (int i = 0; i < 10; ++i) {
    if (i != 1) field.setBrick(i, 0, field.BLACK);
    if (i != 1) field.setBrick(i, 1, field.BLACK);
  }

  field.setBrick(0, 2, field.BLACK);
  field.setBrick(1, 2, field.BLACK);
  field.setBrick(2, 2, field.BLACK);
  field.setBrick(9, 2, field.BLACK);

  field.nextStone ();
  field.nextRotation ();

  field.draw();

  a.exec();

  /*  while(1) {
    int c = cdisp->getKey();
    if (c == cdisp->key_up) {
      field.prevRow();
    } else if (c == cdisp->key_down) {
      field.nextRow();
    } else if (c == cdisp->key_left) {
      field.prevCol();
    } else if (c == cdisp->key_right) {
      field.nextCol();
    } else if (c == cdisp->key_undo) {
      field.undoMove();
    } else if (c == cdisp->key_rotate) {
      field.nextRotation();
    } else if (c == cdisp->key_nextstone) {
      field.nextStone();
    } else if (c == cdisp->key_domove) {
      field.doMove();
    } else if (c == cdisp->key_quit) {
      return 0;
    } else if (c == 338) {
      if(bdisp) bdisp->nextLine();
    } else if (c == 339) {
      if(bdisp) bdisp->prevLine();
    }
    if(bdisp) bdisp->tick();
    field.draw();
    cdisp->refresh();
    if(bdisp) bdisp->refresh();
    }*/
}
