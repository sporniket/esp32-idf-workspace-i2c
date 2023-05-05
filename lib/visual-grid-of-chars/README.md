# Visual grid of chars

A high level to represent the content of a display based on a 2d grid (with some rows and columns) of characters.

* Each character is coded as Ascii character (single byte), or a Unicode character (2 or 4 bytes).
* Each character is qualified with presentation attribute : font index (one byte, up to 256 font), color (either a color index on a single byte, or an RGB codification on 3 bytes), style (a bitmap on a single byte)
* the grid expose an api akin to a classical terminal or console to change its content : clear, print, locate, ...