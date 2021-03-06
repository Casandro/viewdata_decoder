## How to use as a user:
```
cd decoder
make
./viewdata_ncurses nx.nxtel.org 23280
```

## How to use to write your own terminal emulator:

See `viewdata_ncurses.c` as a basic example.

- Use `viewdata_init_and_connect(hostname, port)` to initialize the viewdata screen and connect to a host.
- Write an input loop that checks for characters from our input device and calls `viewdata_handle_stuff()` with the characters you want to send.
  - Minimal useful input is digits 0-9 (0x30-0x39) and both ⚹ (0x2A) and ⌗ (0x5F, different from US-ASCII).
  - 0-9 indicate options. ⚹ starts an absolute page number. ⌗ ends the absolute page number or goes to the next page.
  - Exact shapes for this can be found in ITU-T E.161 figure 2 and 3.
  - Keypad input should be aranged like this: (If simulating the keypad with a d-pad, it might make sense to use a "cursor" on it, and to either return to # or have a separate button for it.)
```
	1 2 3
	4 5 6
	7 8 9
	⚹ 0 ⌗
```
  - Call `viewdata_handle_stuff()` periodically with -1 to process data that comes from the network.
- If `viewdata_handle_stuff()` returns with a non-zero value, update your screen.
  - Rows can be considered to be individual objects.
  - To get decoded character cells of a row, each containing the attributes of that cell use `viewdata_convert_row(row, cells)`;
  - To fill a bitplane bitmap use `viewdata_240_update_row`. Data for one text row (10 pixel lines) will be available in a `vd_240_line_t` object. 
  - The image data is stored in 3 member arrays: `red`, `green` and `blue`. The element type is `uint8_t`;
  - Each bit represents a pixel. 0 is dark and 1 is lit.
  - Most significant bit is left.
