#include <Keyboard.h>

#define PRESS LOW
#define RELEASE HIGH

const int ROWS[] = { 14, 10, 15, 16, A3, A0, A1, A2 };
const int COLS[] = { 7, 5, 3, 4, 6, 2 };
const int ROWS_SZ = sizeof(ROWS) / sizeof(ROWS[0]);
const int COLS_SZ = sizeof(COLS) / sizeof(COLS[0]);

/*
 * I wired the keyboard in a 8x6 matrix to minimize the number of pins (8 + 6 = 14 versus 4 + 12 = 16).
 * The following macro is used to map the wire coordinate system into the layout coordinate system.
 */
#define MAP(l, i, j) (l)  [ (i) / 2 ]  [ (i%2) ? 11 - (j) : (j) ]

/*
 * A layout is a matrix of key codes.
 */
typedef int layout_t[4][12];

const layout_t layout0 = {
  { KEY_TAB,        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', KEY_BACKSPACE  },
  { KEY_ESC,        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\''           },
  { KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_RETURN     },
  { ' ', KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_GUI, 0x1, ' ', ' ', 0x2, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW },
};

const layout_t layout3 = {
  { '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', KEY_BACKSPACE  },
  { KEY_DELETE, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, '-', '=', '[', ']', '\\'},
  { KEY_LEFT_SHIFT, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, '3', '\\', KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_RETURN },
  { ' ', KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_GUI, 0x1, ' ', ' ', 0x2, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW },
};

const layout_t layout4 = {
  { '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', KEY_BACKSPACE  },
  { KEY_DELETE, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, '-', '=', '[', ']', '\\'},
  { KEY_LEFT_SHIFT, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, '3', '\\', KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_RETURN },
  { ' ', KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_GUI, 0x1, ' ', ' ', 0x2, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW },
};

/*
 * The current active layout is just a pointer to the layout itself.
 */
const layout_t * _current_layout = &layout0;

/*
 * `state' of each key: HIGH => released, LOW => pressed.
 */
int state[ROWS_SZ][COLS_SZ] = {
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
};

void setup() {
  Keyboard.begin();
  for (int i = 0; i < ROWS_SZ; i++) pinMode(ROWS[i], OUTPUT);
  for (int j = 0; j < COLS_SZ; j++) pinMode(COLS[j], INPUT_PULLUP);
}

/*
 * Raises voltage on all rows, except the k-th row.
 */
void setRow(int k) {
  for (int i = 0; i < ROWS_SZ; i++)
    digitalWrite(ROWS[i], HIGH);
  digitalWrite(ROWS[k], LOW);
}

void loop() {
  for (int i = 0; i < ROWS_SZ; i++) {
    /* Sets the i-th row and check all the columns */
    setRow(i);
    delay(1);
    for (int j = 0; j < COLS_SZ; j++) {
      int sig = digitalRead(COLS[j]);
      if (state[i][j] != sig) {
        int keycode = MAP(*_current_layout, i, j);
        if (sig == PRESS) {
          /* These keycodes are my special lower & raise keys */
          if (keycode == 0x1 || keycode == 0x2) {
            _current_layout = &layout3;
            RXLED1;
          } else {
            Keyboard.press(keycode);
          }
        } else {
          if (keycode == 0x1 || keycode == 0x2) {
            _current_layout = &layout0;
            RXLED0;
          } else {
            Keyboard.release(keycode);
          }
        }
        state[i][j] = sig;
      }
    }
  }
}
