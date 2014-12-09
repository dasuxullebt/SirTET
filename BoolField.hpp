#pragma once
#include <vector>
#include <initializer_list>
#include <string>

class BoolField {
public:
  std::vector<int> maxinrow;
  std::vector<std::vector<bool> > contents;
  int width, height;

  bool inBounds (int x, int y);

  bool get (int x, int y);

  void set (int x, int y, bool val);

  BoolField (int width, int height, std::initializer_list<bool> inits = {});

  bool lineFits (int y);

  bool stoneFits(const BoolField& otherField, int leftmostRow,
		 std::string& notice);
};
