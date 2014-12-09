#include <list>
#include <QtGlobal>
#include <QCoreApplication>
#include "GameState.hpp"

using namespace std;

GameState::GameState (int width, int height) :
  width (width), height (height), cbox (BoolField(width, height)), stone (NULL)
{
  recalculate ();
}

GameState::~GameState () {}

void GameState::setBrick (int x, int y, bool color) {
  emit setPixel(x, y, (color == GameState::BLACK) ? 
		FILLED :
		EMPTY);
  this->cbox.set(x, y, color);
}

bool GameState::undoMove () {
  if (history.empty()) {
    return false;
  } else {
    HistoryItem hf = history.front();
    if (hf.type == HistoryItem::STONE) {
      BoolField stone = *hf.stone;
      int crow = hf.crow;
      int ccol = hf.ccol;
      for (int i = 0; i < stone.width; ++i) {
	for (int j = 0; j < stone.height; ++j) {
	  if (stone.contents[i][j]) {
	    this->setBrick(crow+i,ccol+j,this->BLACK);
	  }
	}
      }
      history.pop_front();
      //this->draw(true, true);
      this->recalculate();
    } else if (hf.type == HistoryItem::LINE) {
      int ccol = hf.ccol;
      //this->draw(true, true);
      for (int i = 0; i < width; ++i) {
	for (int j = ccol; j < height-1; ++j) {
	  // TODO: also lazy here
	  if (this->cbox.contents[i][j] != this->cbox.contents[i][j+1]) {
	    this->setBrick(i, j, this->cbox.contents[i][j+1] ? this->BLACK : this->WHITE);
	  }
	}
	this->setBrick(i, height-1, this->WHITE);
      }
      history.pop_front();
      //this->draw(true, true);
      this->recalculate();
    } else {
      Q_ASSERT(false);
    }
    return true;
  }
}

bool GameState::doMove () {
  /* do the current move if possible and return true. return
   * false otherwise. */
  if (this->fits) {
    if (this->cstone < 7) {
      for (int i = 0; i < this->stone->width; ++i) {
	for (int j = 0; j < this->stone->height; ++j) {
	  int x = this->crow + i;
	  int y = this->ccol + j;
	  if (this->stone->contents[i][j] &&
	      (0 <= x) && (x < width) &&
	      (0 <= y) && (y < height)) {
	    this->setBrick(x, y, this->WHITE);
	  }
	}
      }

      HistoryItem newhist(HistoryItem::STONE, this->crow, this->ccol, this->stone);
      history.push_front(newhist);

      this->recalculate();
      return true;
    } else {
      // *assumes* that lineFits was checked

      for (int i = 0; i < width; ++i) {
	for (int j = height - 1; j > this->ccol; --j) {
	  this->setBrick(i, j, this->cbox.contents[i][j-1] ? this->BLACK : this->WHITE);
	}
	this->setBrick(i, this->ccol, this->BLACK);
      }

      HistoryItem newhist(HistoryItem::LINE, 0, this->ccol, NULL);
      history.push_front(newhist);

      this->recalculate();
    }
    return true;
  } else {
    return false;
  }
}

bool GameState::stoneFits () {
  string note;
  if (this->cbox.stoneFits(*this->stone, this->crow, note)) {
    /* the stone fits. up to four former history items can be
     * lines. the current stone must meet them all. */
    for (auto i = history.begin(); i != history.end(); ++i) {
      int lnum = i->ccol;
      int cy = this->cbox.maxinrow[this->crow] - this->stone->maxinrow[0];
      if (!((cy <= lnum) && (lnum < cy + this->stone->height))) {
	emit notice ("all introduced line MUST be split");
	return false;
      }
    }
    return true;
  } else {
    emit notice(note);
    return false;
  }
}

bool GameState::lineFits (int y) {
  // TODO: remove notices when not necessary.
  int num = 0;
  int min = y;
  int max = y;

  int ccols[] = {y, 0, 0, 0};

  for (auto i = history.begin(); i != history.end(); ++i) {
    if (++num > 3) {
      emit notice("At most four lines can be introduced before they must be split.");
      return false;
    } else {
      ccols[num] = i->ccol;
    }
  }

  // do necessary corrections
  for (int i = num; i >= 0; --i) {
    for (int j = i+1; j <= num; ++j) {
      if (ccols[i] <= ccols[j]) ccols[j]++;
    }
  }

  for (int i = 0; i <= num; ++i) {
    min = (min < ccols[i]) ? min : ccols[i];
    max = (max > ccols[i]) ? max : ccols[i];
  }
  if (max >= 20) {
    emit notice("Lines would go out of the borders.");
    return false;
  }
  if (max - min >= 4) {
    emit notice("The lines are too far apart to be split at once.");
    return false;
  }
  return this->cbox.lineFits(this->ccol);
}

void GameState::recalculate () {
  /* do ... stuff */
  if (this->cstone > 7) {
    this->cstone = 0; this->recalculate(); return;
  } else if (this->cstone == 7) {
    // Line
    this->ccol = this->ccol % height;
    this->fits = this->lineFits(this->ccol);
    return;
  } else {
    const BoolField* const cstones = stones[this->cstone];
    this->stone = & cstones[this->crot%stonelens[this->cstone]];
    if (this->stone->width + this->crow > width) {
      this->crow = width - this->stone->width;
      this->recalculate();
      return;
    } else if (this->crow < 0) {
      this->crow = 0;
      this->recalculate();
      return;
    }

    if ((this->fits = this->stoneFits())) {
      this->ccol = this->cbox.maxinrow[this->crow] - this->stone->maxinrow[0];
    } else {
      this->ccol = this->cbox.maxinrow[this->crow] - this->stone->maxinrow[0];
      if (this->ccol < this->stone->height) this->ccol = 0;

      //this->ccol = 13; /* TODO */
    }
  }
}

void GameState::draw () {
  int st_xmax, st_ymax, st_xmin, st_ymin;
  if (this->cstone < 7) {
    st_xmin = crow;
    st_ymin = ccol;
    st_xmax = crow + stone->width;
    st_ymax = ccol + stone->height;
  } else {
    st_ymin = ccol;
    st_ymax = ccol + 1;
    st_xmin = 0;
    st_xmax = width;
  }

  for (int i = 0; i < width; ++i) {
    bool b = (st_xmin <= i) && (i < st_xmax);
    for (int j = 0; j < height; ++j) {
      if (b && (st_ymin <= j) && (j < st_ymax)) {
	bool isstone = (this->cstone < 7) ?
	  stone->contents[i - st_xmin][j - st_ymin] :
	  true; // lines are always full
	if (isstone) {
	  if (cbox.contents[i][j]) {
	    if (fits) {
	      emit setPixel(i, j, FITS_FULL);
	    } else {
	      emit setPixel(i, j, DOESNT_FIT_FULL);
	    }
	  } else if (fits) {
	    emit setPixel(i, j, FITS_EMPTY);
	  } else {
	    emit setPixel(i, j, DOESNT_FIT_EMPTY);
	  }
	} else if (cbox.contents[i][j]) {
	  emit setPixel(i, j, FILLED);
	} else {
	  emit setPixel(i, j, EMPTY);
	}
      } else if (cbox.contents[i][j]) {
        emit setPixel(i, j, FILLED);
      } else {
	emit setPixel(i, j, EMPTY);
      }
    }
  }
  emit refresh();
}

void GameState::nextRow () {
  //this->draw(true, true);
  this->crow++;
  this->recalculate();
}

void GameState::prevRow () {
  //this->draw(true, true);
  this->crow--;
  this->recalculate();
}

void GameState::nextStone () {
  //this->draw(true, false);
  this->cstone++;
  this->recalculate();
}

void GameState::nextCol () {
  if (this->cstone == 7) {
    //this->draw(true, true);
    this->ccol++;
    if (this->ccol >= height) this->ccol = height - 1;
    this->recalculate();
  }
}

void GameState::prevCol () {
  if (this->cstone == 7) {
    //this->draw(true, true);
    this->ccol--;
    if (this->ccol < 0) this->ccol = 0;
    this->recalculate();
  }
}

void GameState::nextRotation () {
  //this->draw(true, true);
  this->crot++;
  // todo: wat?
  this->crot %= 20;
  this->recalculate();
}

void GameState::keyStroke(Key k) {
  switch (k) {
  case K_UP:
    prevRow(); break;
  case K_DOWN:
    nextRow(); break;
  case K_LEFT:
    prevCol(); break;
  case K_RIGHT:
    nextCol(); break;
  case K_UNDO:
    undoMove(); break;
  case K_ROTATE:
    nextRotation(); break;
  case K_NEXTSTONE:
    nextStone(); break;
  case K_DOMOVE:
    doMove(); break;
  case K_QUIT:
    // TODO
    QCoreApplication::instance()->exit(0);
    break;
  }
  draw();
}
