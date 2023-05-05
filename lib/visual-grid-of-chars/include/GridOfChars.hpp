#ifndef GRID_OF_CHARS_HPP
#define GRID_OF_CHARS_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "GlyphModel.hpp"

using RowIndex = uint8_t;
using ColumnIndex = uint8_t;
using CursorPosition = uint16_t;

/** @brief What the class is for.
 */
template <typename TypeOfChar, typename TypeOfColor> class GridOfChars {
  using TypeOfGlyph = GlyphModel<TypeOfChar, TypeOfColor>;

private:
  /** @brief current color. */
  TypeOfColor color;
  /** @brief current font. */
  FontIndex font;
  /** @brief current color. */
  StyleFlags style;
  /** @brief default char to use when clearing. */
  const TypeOfGlyph eraseGlyph;
  /** @brief Buffer for the grid of chars, MUST be allocated by the caller. */
  TypeOfGlyph *const grid;
  /** @brief max value for row (excluded).*/
  const RowIndex maxRowIndexExcluded;
  /** @brief max value for column (excluded).*/
  const ColumnIndex maxColumnIndexExcluded;
  /** @brief max value for cursor (excluded).*/
  const CursorPosition maxCursor;
  /** @brief Current value for row.*/
  RowIndex cursorRow;
  /** @brief Current value for column.*/
  ColumnIndex cursorColumn;
  /** @brief Absolute row index of the grid origin.*/
  RowIndex baseRow;
  /** @brief Absolute column index of the grid origin.*/
  ColumnIndex baseColumn;
  /** @brief Current value for cursor.*/
  CursorPosition cursor;

  void updateCursorPosition() {
    cursor =
        (CursorPosition)cursorRow * (CursorPosition)maxColumnIndexExcluded +
        (CursorPosition)cursorColumn;
  }

public:
  virtual ~GridOfChars();
  GridOfChars(RowIndex rows, ColumnIndex columns, TypeOfGlyph defaultGlyph,
              TypeOfGlyph *buffer)
      : maxRowIndexExcluded(rows), maxColumnIndexExcluded(columns),
        eraseGlyph(defaultGlyph), grid(buffer), cursorRow(0), cursorColumn(0),
        cursor(0) {
    color = eraseGlyph.color;
    font = eraseGlyph.font;
    style = eraseGlyph.style;
    clear();
  }
  GridOfChars *init();
  GridOfChars *clear() {
    clearArea(0, 0, maxColumnIndexExcluded, maxRowIndexExcluded);
    return this;
  }
  GridOfChars *clearRow(RowIndex r) {
    clearArea(0, r, maxColumnIndexExcluded, r + 1);
    return this;
  }
  GridOfChars *clearRows(RowIndex fromIncluded, RowIndex toExcluded) {
    clearArea(0, fromIncluded, maxColumnIndexExcluded, toExcluded);
    return this;
  }
  GridOfChars *clearColumn(ColumnIndex c) {
    clearArea(c, 0, c + 1, maxRowIndexExcluded);
    return this;
  }
  GridOfChars *clearColumn(ColumnIndex fromIncluded, ColumnIndex toExcluded) {
    clearArea(fromIncluded, 0, toExcluded, maxRowIndexExcluded);
    return this;
  }
  GridOfChars *clearArea(ColumnIndex fromColumnIncluded,
                         RowIndex fromRowIncluded, ColumnIndex toColumnExcluded,
                         RowIndex toRowExcluded) {
    // TO DO : use grid origin to perform, perform
    return this;
  }
  GridOfChars *scrollBy(ColumnIndex deltaCols, RowIndex deltaRows);
  GridOfChars *scrollTo(ColumnIndex deltaCols, RowIndex deltaRows);
  GridOfChars *withColor(TypeOfColor color) {
    this->color = color;
    return this;
  }
  GridOfChars *withStyle(StyleFlags style) {
    this->style = style;
    return this;
  }
  GridOfChars *withFont(FontIndex font) {
    this->font = font;
    return this;
  }
};

#endif