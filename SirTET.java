// java -Djava.awt.headless=true -cp /usr/share/brltty/lib/brlapi.jar:lanterna-2.1.7.jar:. SirTET

//BRLAPI_HOST=127.0.0.1:1 java -ea -Djava.library.path=/usr/lib/brltty/ -Djava.awt.headless=true -cp /usr/share/brltty/lib/brlapi.jar:lanterna-2.1.7.jar:. SirTET
import org.a11y.BrlAPI.*;
import com.googlecode.lanterna.terminal.*;
import com.googlecode.lanterna.*;
import java.util.*;

public class SirTET {
    static class BoolField {
	int width, height;
	boolean[][] contents;
	int[] maxinrow;

	BoolField (int width, int height, boolean... contents) {
	    this.width = width;
	    this.height = height;
	    this.contents = new boolean[width][height];
	    this.maxinrow = new int[width];
	    for (int i = 0; i < this.width; ++i) {
		this.maxinrow[i] = -1;
		for (int j = 0; j < this.height; ++j) {
		    //this.contents[i][j] = false; // default value
		    int k = j*width+i;
		    if (k < contents.length) {
			this.set(i, j, contents[k]);
		    } else {
			this.set(i, j, false);
		    }
		}
	    }
	}

	boolean lineFits (int y) {
	    // does line *formally* fit (history is not checked here)
	    for (int i = 0; i < this.width; ++i) {
		if ((this.maxinrow[i] >= y) &&
		    (this.maxinrow[i] + 1 >= this.height)) return false;
	    }
	    return true;
	}

	void set (int x, int y, boolean val) {
	    if (this.contents[x][y] != val) {
		if (val) {
		    if (y > this.maxinrow[x]) this.maxinrow[x] = y;
		} else {
		    if (y == this.maxinrow[x]) {
			// we are the largest - find the next one below us
			int z = y-1;
			while ((z >= 0) && (!this.contents[x][z])) { z--; }
			this.maxinrow[x] = z;
		    }
		}
		this.contents[x][y] = val;
	    }
	}

	void notice (String bla) {
	}

	boolean stoneFits(BoolField otherField, int leftmostRow) {
	    // constraint: the leftmost row of otherField *must* contain
	    // something!

	    // furthermore, the rightmost row MUST NOT be out of the bounds
	    // of this field.
	    assert otherField.width + leftmostRow <= this.width;
	    assert otherField.maxinrow[0] != -1;

	    // only *formally* check if the stone fits. history is not
	    // regarded.

	    if (this.maxinrow[leftmostRow] == -1) {
		// also, our row *must* contain something
		notice ("There are not enough bricks to remove in the row.");
		return false;
	    } else {
		// the top-most bricks of both must match, so we can set
		int tr_y = this.maxinrow[leftmostRow] - otherField.maxinrow[0];

		if (tr_y < 0) {
		    // the stone cannot fit, since it is too high
		    return false;
		}

		// the condition is: if a brick was there, all bricks
		// with higher y-coordinate must match.

		boolean support = false;
		for (int i = 0; i < otherField.width; ++i) {
		    boolean wasthere = false;
		    for (int j = tr_y; j < this.height; ++j) {
			if (wasthere) {
			    // there has been a brick below this - all
			    // bricks must match
			    boolean other =
				((j-tr_y) < otherField.height) ?
				otherField.contents[i][j-tr_y] : false;
			    if (other != this.contents[leftmostRow+i][j]) {
				notice ("Bricks atop stone / Stone overlaps empty fields (1)");
				return false;
			    }
			} else {
			    // there has not been a brick in
			    // otherField yet at this x-coordinate.
			    boolean other = ((j-tr_y) < otherField.height) ?
				otherField.contents[i][j-tr_y] : false;
			    if (other) {
				assert i >= 0;
				assert j >= 0;
				if (other != this.contents[leftmostRow+i][j]) {
				    notice ("Bricks atop stone / Stone overlaps empty fields (1)");
				    return false;
				}
				// if there is one, save that for later ...
				wasthere = true;
				support = support || (j == 0) || (this.contents[leftmostRow+i][j-1]);
			    }
			}
		    }
		}

		if (!support) {
		    notice("Stone is not supported and would fall down.");
		    return false;
		}

		// everything went good ...
		return true;
	    }
	};
    }

    /* the stones and their rotations */
    public static final BoolField L[] =
    { new BoolField (2, 3,
		     true, false,
		     true, false,
		     true, true),
      new BoolField (3, 2,
		     true, true, true,
		     true, false, false),
      new BoolField (2, 3,
		     true, true,
		     false, true,
		     false, true),
      new BoolField (3, 2,
		     false, false, true,
		     true, true, true) };

    public static final BoolField J[] =
    { new BoolField (2, 3,
		     false, true,
		     false, true,
		     true, true),
      new BoolField (3, 2,
		     true, false, false,
		     true, true, true),
      new BoolField (2, 3,
		     true, true,
		     true, false,
		     true, false),
      new BoolField (3, 2,
		     true, true, true,
		     false, false, true) };

    public static final BoolField S[] =
    { new BoolField (2, 3,
		     true, false,
		     true, true,
		     false, true),
      new BoolField (3, 2,
		     false, true, true,
		     true, true, false) };

    public static final BoolField Z[] =
    { new BoolField (2, 3,
		     false, true,
		     true, true,
		     true, false),
      new BoolField (3, 2,
		     true, true, false,
		     false, true, true) };

    public static final BoolField O[] =
    { new BoolField (2, 2,
		     true, true,
		     true, true) };

    public static final BoolField T[] =
    { new BoolField (3, 2,
		     true, true, true,
		     false, true, false),
      new BoolField (2, 3,
		     false, true,
		     true, true,
		     false, true),
      new BoolField (3, 2,
		     false, true, false,
		     true, true, true),
      new BoolField (2, 3,
		     true, false,
		     true, true,
		     true, false) };

    public static final BoolField I[] =
    { new BoolField (1, 4,
		     true,
		     true,
		     true,
		     true),
      new BoolField (4, 1,
		     true, true, true, true) };

    public static final BoolField[][] stones = {L, J, O, T, I, S, Z};

    static class GameState {
	static final int BLACK = 0;
	static final int WHITE = 1;
	class HistoryItem {
	    static final int STONE = 0;
	    static final int LINE = 1;
	    BoolField stone;
	    int type;
	    int crow;
	    int ccol;
	    HistoryItem prev;
	}
	int width, height, braille_len;
	HistoryItem history = null;
	Terminal terminal;
	BoolField cbox;
	int cstone = 0, crot = 0, crow = 0, ccol = 0, cblink = 0;
	BoolField stone = null;
	boolean fits = false;
	Brlapi brl;
	int brlln = 0; // 0-2, line in braille
	byte[][] braille = new byte[3][40];

	int[][] brl_flags = new int[][] {
	    { 1 << 0, 1 << 3 },
	    { 1 << 1, 1 << 4 },
	    { 1 << 2, 1 << 5 },
	    { 1 << 6, 1 << 7 }
	};

	GameState (int width, int height, Terminal term, Brlapi brl) {
	    this.width = width;
	    this.height = height;
	    this.cbox = new BoolField(width, height);
	    this.terminal = term;
	    this.brl = brl;
	    recalculate ();
	}

	void setPixel (int x, int y, int color) {
	    /* draw only, do not change the actual playfield */

	    terminal.moveCursor(y, x);

	    int brlx = x / 4;
	    int brly = y / 2;
	    int flag = brl_flags[x%4][y%2];

	    switch (color) {
	    case BLACK:
		terminal.putCharacter('#');
		braille[brlx][brly] |= flag;
		break;
	    case WHITE:
		terminal.putCharacter(' ');
		braille[brlx][brly] &= ~flag;
		break;
	    default:
		assert false;
	    }
	    terminal.flush();

	    if (brl != null) {
		try {
		    brl.writeDots(braille[brlln]);
		} catch (org.a11y.BrlAPI.Error e) {
		    System.err.println("Error writing to braille terminal: " + e.toString());
		}
	    }
	}
	
	void setBrick (int x, int y, int color) {
	    /* set play field */
	    this.setPixel(x, y, color);
	    this.cbox.set(x, y, color == this.BLACK);
	}

	boolean undoMove () {
	    if (history == null) {
		return false;
	    } else {
		if (history.type == HistoryItem.STONE) {
		    BoolField stone = history.stone;
		    int crow = history.crow;
		    int ccol = history.ccol;
		    for (int i = 0; i < stone.width; ++i) {
			for (int j = 0; j < stone.height; ++j) {
			    if (stone.contents[i][j]) {
				this.setBrick(crow+i,ccol+j,this.BLACK);
			    }
			}
		    }
		    history = history.prev;
		    this.draw(true, true);
		    this.recalculate();
		} else if (history.type == HistoryItem.LINE) {
		    int ccol = history.ccol;
		    this.draw(true, true);
		    for (int i = 0; i < width; ++i) {
			for (int j = ccol; j < height-1; ++j) {
			    // TODO: also lazy here
			    if (this.cbox.contents[i][j] != this.cbox.contents[i][j+1]) {
				this.setBrick(i, j, this.cbox.contents[i][j+1] ? this.BLACK : this.WHITE);
			    }
			}
			this.setBrick(i, height-1, this.WHITE);
		    }
		    history = history.prev;
		    this.draw(true, true);
		    this.recalculate();
		} else {
		    assert false;
		}
		return true;
	    }
	}

	boolean doMove () {
	    /* do the current move if possible and return true. return
	     * false otherwise. */
	    if (this.fits) {
		if (this.cstone < 7) {
		    for (int i = 0; i < this.stone.width; ++i) {
			for (int j = 0; j < this.stone.height; ++j) {
			    int x = this.crow + i;
			    int y = this.ccol + j;
			    if (this.stone.contents[i][j] &&
				(0 <= x) && (x < width) &&
				(0 <= y) && (y < height)) {
				this.setBrick(x, y, this.WHITE);
			    }
			}
		    }
		
		    HistoryItem oldHistory = history;
		    history = new HistoryItem();
		    history.prev = oldHistory;
		    history.type = HistoryItem.STONE;
		    history.stone = this.stone;
		    history.crow = this.crow;
		    history.ccol = this.ccol;
		    this.draw(true, true);
		    this.recalculate();
		    return true;
		} else {
		    // *assumes* that lineFits was checked
		    this.draw(true,true);
		    for (int i = 0; i < width; ++i) {
			for (int j = height - 1; j > this.ccol; --j) {
			    this.setBrick(i, j, this.cbox.contents[i][j-1] ? this.BLACK : this.WHITE);
			}
			this.setBrick(i, this.ccol, this.BLACK);
		    }

		    /*		for (var i = 0; i < width; ++i) {
				var out = "";
				for (var j = 0; j < height; ++j) {
				out += (this.cbox.contents[i][j]) ? ".." : "||";
				}
				console.log(out);
				}*/

		    HistoryItem oldHistory = history;
		    history = new HistoryItem();
		    history.prev = oldHistory;
		    history.type = HistoryItem.LINE;
		    history.ccol = this.ccol;
		    this.draw(true,true);
		    this.recalculate();
		}
		return true;
	    } else {
		return false;
	    }
	}

	boolean stoneFits () {
	    if (this.cbox.stoneFits(this.stone, this.crow)) {
		/* the stone fits. up to four former history items can be
		 * lines. the current stone must meet them all. */
		HistoryItem chist = history;
		while ((chist != null) && (chist.type == HistoryItem.LINE)) {
		    int lnum = chist.ccol;
		    int cy = this.cbox.maxinrow[this.crow] - this.stone.maxinrow[0];
		    if ((cy <= lnum) && (lnum < cy + this.stone.height)) {
			// good
			chist = chist.prev;
		    } else {
			//notice ("all introduced line MUST be split");
			return false;
		    }
		}
		return true;
	    }
	    return false;
	}

	boolean lineFits (int y) {
	    // TODO: remove notices when not necessary.
	    HistoryItem chist = history;
	    int num = 0;
	    int min = y;
	    int max = y;

	    int[] ccols = new int[4];
	    ccols[0] = y;

	    while ((chist != null) && (chist.type == HistoryItem.LINE)) {
		if (++num > 3) {
		    //notice("At most four lines can be introduced before they must be split.");
		    return false;
		} else {
		    ccols[num] = chist.ccol;
		    chist = chist.prev;
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
		//notice("Lines would go out of the borders.");
		return false;
	    }
	    if (max - min >= 4) {
		//notice("The lines are too far apart to be split at once.");
		return false;
	    }
	    return this.cbox.lineFits(this.ccol);
	}

	void recalculate () {
	    /* do ... stuff */
	    if (this.cstone > 7) {
		this.cstone = 0; this.recalculate(); return;
	    } else if (this.cstone == 7) {
		// Line
		this.ccol = this.ccol % height;
		this.fits = this.lineFits(this.ccol);
		return;
	    } else {
		BoolField[] cstones = stones[this.cstone];
		this.stone = cstones[this.crot%cstones.length];
		if (this.stone.width + this.crow > width) {
		    this.crow = width - this.stone.width;
		    this.recalculate();
		    return;
		} else if (this.crow < 0) {
		    this.crow = 0;
		    this.recalculate();
		    return;
		}

		if (this.fits = this.stoneFits()) {
		    this.ccol = this.cbox.maxinrow[this.crow] - this.stone.maxinrow[0];
		} else {
		    this.ccol = 13; /* TODO */
		}
	    }
	}

	void draw (boolean un, boolean col) {
	    if (this.cstone < 7) {
		for (int i = 0; i < this.stone.width; ++i) {
		    for (int j = 0; j < this.stone.height; ++j) {
			int x = this.crow + i;
			int y = this.ccol + j;
			if (this.stone.contents[i][j] &&
			    (0 <= x) && (x < width) &&
			    (0 <= y) && (y < height)) {

			    if (un) {
				this.setPixel(x, y,
					      this.cbox.contents[x][y] ? this.BLACK : this.WHITE);
			    } else {
				this.setPixel(x, y, col ? this.WHITE : this.BLACK);
			    }
			}
		    }
		}
	    } else {
		for (int i = 0; i < width; ++i) {
		    if (un) {
			this.setPixel(i, this.ccol, this.cbox.contents[i][this.ccol] ? this.BLACK : this.WHITE);
		    } else {
			this.setPixel(i, this.ccol, col ? this.WHITE : this.BLACK);
		    }
		}
	    }
	}

	void nextRow () {
	    this.draw(true, true);
	    this.crow++;
	    this.recalculate();
	}

	void prevRow () {
	    this.draw(true, true);
	    this.crow--;
	    this.recalculate();
	}

	void nextStone () {
	    // todo: vielleicht true, true
	    this.draw(true, false);
	    this.cstone++;
	    this.recalculate();
	}

	void nextCol () {
	    if (this.cstone == 7) {
		this.draw(true, true);
		this.ccol++;
		if (this.ccol >= height) this.ccol = height - 1;
		this.recalculate();
	    }
	}

	void prevCol () {
	    if (this.cstone == 7) {
		this.draw(true, true);
		this.ccol--;
		if (this.ccol < 0) this.ccol = 0;
		this.recalculate();
	    }
	}

	void nextRotation () {
	    this.draw(true, true);
	    this.crot++;
	    // todo: wat?
	    this.crot %= 20;
	    this.recalculate();
	}

	void lineDown () {
	    brlln = (brlln+1)%3;
	}

	void lineUp () {
	    brlln = brlln > 0 ? brlln - 1 : brlln;
	}

	void tick () {
	    /* do blinking and ... stuff */
	    if (this.fits) {
		if (this.cblink >> 1 == 0) {
		    this.draw(false, true);
		} else {
		    this.draw(false, false);
		}
	    } else {
		/* doesn't fit ... blink faster ... (TODO: blink differently for matching bricks) */
		if (this.cblink %2 == 0) {
		    this.draw(false, true);
		} else {
		    this.draw(false, false);
		}
	    }
	    this.cblink = (this.cblink + 1)%4;
	}
    }

    public static void main (String[] args) {
	//	try {
	ConnectionSettings cs = new ConnectionSettings ();
	Brlapi brl = null;
	try {
	    brl = new Brlapi (cs);
	    brl.enterTtyMode();
	} catch (org.a11y.BrlAPI.Error exc) {
	    System.err.println("Could not open BRLTTY device: " + exc.toString());
	}


	Terminal terminal = TerminalFacade.createTerminal();
	terminal.enterPrivateMode();

	GameState field = new GameState(10, 20, terminal, brl);

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
	
	com.googlecode.lanterna.input.Key k;

	bmark: while (true) {
	    if ((k = terminal.readInput()) != null) {
		if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.Enter) {
		    field.doMove();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.NormalKey) {
		    if (k.getCharacter() == ' ') {
			field.nextRotation();
		    }
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.ArrowLeft) {
		    field.prevCol();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.ArrowUp) {
		    field.prevRow();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.ArrowRight) {
		    field.nextCol();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.ArrowDown) {
		    field.nextRow();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.Backspace) {
		    field.undoMove();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.Tab) {
		    field.nextStone();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.Escape) {
		    break bmark;
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.PageUp) {
		    field.lineUp();
		} else if (k.getKind() == com.googlecode.lanterna.input.Key.Kind.PageDown) {
		    field.lineDown();
		}
	    }
	    field.tick();
	    try {
		Thread.sleep(100);
	    } catch (InterruptedException exc) {
		// don't care
	    }
	}

	try {
	    brl.leaveTtyMode();
	    brl.closeConnection();
	} catch (org.a11y.BrlAPI.Error e) {
	    // don't care
	} catch (NullPointerException e) {
	    // don't care
	}

	terminal.exitPrivateMode();
    }
}
