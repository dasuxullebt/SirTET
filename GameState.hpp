#pragma once
#include "BoolField.hpp"
#include "Display.hpp"
#include <list>
#include <memory>
#include <QObject>

/* the stones and their rotations */
const BoolField L[] =
  { BoolField (3, 2, { 
	true, false,
	true, false,
	true, true }),
    BoolField (2, 3, {
	true, true, true,
	true, false, false}),
    BoolField (3, 2, {
	true, true,
	  false, true,
	  false, true}),
    BoolField (2, 3, {
	false, false, true,
	true, true, true }) };

const BoolField J[] =
  { BoolField (3, 2, {
	       false, true,
	       false, true,
	       true, true}),
    BoolField (2, 3, {
	       true, false, false,
	       true, true, true}),
    BoolField (3, 2, {
	       true, true,
	       true, false,
	       true, false}),
    BoolField (2, 3, {
	       true, true, true,
	       false, false, true}) };

const BoolField S[] =
  { BoolField (3, 2, {
	       true, false,
	       true, true,
	       false, true}),
    BoolField (2, 3, {
	       false, true, true,
	       true, true, false}) };

const BoolField Z[] =
  { BoolField (3, 2, {
	       false, true,
	       true, true,
	       true, false }),
    BoolField (2, 3, {
	       true, true, false,
	       false, true, true }) };

const BoolField O[] =
  { BoolField (2, 2, {
	       true, true,
	       true, true }) };

const BoolField T[] =
  { BoolField (2, 3, {
	       true, true, true,
	       false, true, false }),
    BoolField (3, 2, {
	       false, true,
	       true, true,
	       false, true }),
    BoolField (2, 3, {
	       false, true, false,
	       true, true, true }),
    BoolField (3, 2, {
	       true, false,
	       true, true,
	       true, false }) };

const BoolField I[] =
  { BoolField (4, 1, {
	       true,
	       true,
	       true,
	       true }),
    BoolField (1, 4, {
	true, true, true, true }) };

const BoolField* const stones[] = {L, J, O, T, I, S, Z};

const int stonelens[] = {4, 4, 1, 4, 2, 2, 2};

class GameState : public QObject {
  Q_OBJECT
private:
  class HistoryItem {
  public:
    // TODO: Use enum and Qoption here
    static const int STONE = 0;
    static const int LINE = 1;

    const BoolField* stone;
    int type;
    int crow;
    int ccol;

    HistoryItem (int type, int crow, int ccol, const BoolField* stone) :
      type(type), crow(crow), ccol(ccol), stone(stone) {};
  };

public:

  static const bool WHITE = false;
  static const bool BLACK = true;

  std::list<HistoryItem> history;

  int width, height, braille_len;
  BoolField cbox;

  int cstone = 0, crot = 3, crow = 0, ccol = 0, cblink = 0;

  const BoolField* stone;

  bool fits = false;
  GameState (int width, int height);
  ~GameState ();

  /* set play field */
  void setBrick (int x, int y, bool color);

  bool undoMove ();
  bool doMove ();

  bool stoneFits ();
  bool lineFits (int y);
  void recalculate ();

  void draw ();
  void nextRow ();
  void prevRow ();
  void nextStone ();
  void nextCol ();
  void prevCol ();
  void nextRotation ();

  void doRefresh() {
    emit refresh(); // TODO
  }
signals:
  void setPixel (int x, int y, Color color);
  void notice (const std::string& c);
  void refresh();
public slots:
  void keyStroke(Key k);
};
