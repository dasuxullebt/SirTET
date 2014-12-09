#include <QtGlobal>
#include "BoolField.hpp"

using namespace std;

bool BoolField::inBounds (int x, int y) {
  return (x < this->width) && (y < this->height);
}

bool BoolField::get (int x, int y) {
  Q_ASSERT (x < this->width);
  Q_ASSERT (y < this->height);
  return this->contents[x][y];
}

void BoolField::set (int x, int y, bool val) {
  Q_ASSERT (x < this->width);
  Q_ASSERT (y < this->height);
  if (contents[x][y] != val) {
    if (val) {
      if (y > this->maxinrow[x]) this->maxinrow[x] = y;
    } else {
      if (y == this->maxinrow[x]) {
	// we are the largest - find the next one below us
	int z = y-1;
	while ((z >= 0) && (!this->contents[x][z])) { z--; }
	this->maxinrow[x] = z;
      }
    }
    this->contents[x][y] = val;
  }
}

BoolField::BoolField (int width, int height,
		      initializer_list<bool> inits) :
  width (width), height (height) {
  this->contents.resize(width);
  this->maxinrow.resize(width);
  for (int i = 0; i < width; ++i) {
    this->maxinrow[i] = -1;
    this->contents[i].resize(height);
    for (int j = 0; j < this->height; ++j) {
      this->contents[i][j] = false;
    }
  }

  auto arg = inits.begin();
  auto arg_end = inits.end();

#ifndef NDEBUG
  int count = 0;
#endif

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      if (arg != arg_end) {
#ifndef NDEBUG
	count++;
#endif
	this->set(i, j, *arg);
	arg++;
      } else {
	return;
      }
    }
  }

#ifndef NDEBUG
  Q_ASSERT (count == width * height);
#endif
  Q_ASSERT (arg == arg_end);
}

bool BoolField::lineFits (int y) {
  // does line *formally* fit (history is not checked here)
  for (int i = 0; i < this->width; ++i) {
    if ((this->maxinrow[i] >= y) &&
	(this->maxinrow[i] + 1 >= this->height)) return false;
  }
  return true;
}

bool BoolField::stoneFits(const BoolField& otherField, int leftmostRow,
			  string& notice) {
  // constraint: the leftmost row of otherField *must* contain
  // something!

  // furthermore, the rightmost row MUST NOT be out of the bounds
  // of this field.
  Q_ASSERT(otherField.width + leftmostRow <= this->width);
  Q_ASSERT(otherField.maxinrow[0] != -1);

  // only *formally* check if the stone fits. history is not
  // regarded.

  if (this->maxinrow[leftmostRow] == -1) {
    // also, our row *must* contain something
    notice = "There are not enough bricks to remove in the row.";
    return false;
  } else {
    // the top-most bricks of both must match, so we can set
    int tr_y = this->maxinrow[leftmostRow] - otherField.maxinrow[0];

    if (tr_y < 0) {
      // the stone cannot fit, since it is too high
      return false;
    }

    // the condition is: if a brick was there, all bricks
    // with higher y-coordinate must match.

    bool support = false;
    for (int i = 0; i < otherField.width; ++i) {
      bool wasthere = false;
      for (int j = tr_y; j < this->height; ++j) {
	if (wasthere) {
	  // there has been a brick below this - all
	  // bricks must match
	  bool other =
	    ((j-tr_y) < otherField.height) ?
	    otherField.contents[i][j-tr_y] : false;
	  if (other != this->contents[leftmostRow+i][j]) {
	    notice = "Bricks atop stone / Stone overlaps empty fields (1)";
	    return false;
	  }
	} else {
	  // there has not been a brick in
	  // otherField yet at this x-coordinate.
	  bool other = ((j-tr_y) < otherField.height) ?
	    otherField.contents[i][j-tr_y] : false;
	  if (other) {
	    Q_ASSERT(i >= 0);
	    Q_ASSERT(j >= 0);
	    if (other != this->contents[leftmostRow+i][j]) {
	      notice = "Bricks atop stone / Stone overlaps empty fields (1)";
	      return false;
	    }
	    // if there is one, save that for later ...
	    wasthere = true;
	    support = support || (j == 0) || (this->contents[leftmostRow+i][j-1]);
	  }
	}
      }
    }

    if (!support) {
      notice = "Stone is not supported and would fall down.";
      return false;
    }

    // everything went good ...
    return true;
  }
}
