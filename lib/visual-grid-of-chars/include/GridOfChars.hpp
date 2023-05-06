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
// TODO query interface to get the current font, style and color
// TODO query interface to get the cursor position (row, column) relative to
// origin
// TODO query interface to get the cursor origin (row, column)
// TODO query interface to get the glyph at a given position
// TODO query interface to get the grid content (const pointer to const values)
template <typename TypeOfChar, typename TypeOfColor> class GridOfChars {
  using TypeOfGlyph = GlyphModel<TypeOfChar, TypeOfColor>;

private:
  // current settings
  /** @brief current color. */
  TypeOfColor color;
  /** @brief current font. */
  FontIndex font;
  /** @brief current color. */
  StyleFlags style;

  // grid management
  /** @brief default char to use when clearing, contains the default values of
   * the settings. */
  const TypeOfGlyph eraseGlyph;
  /** @brief Buffer for the grid of chars, MUST be allocated by the caller. */
  TypeOfGlyph *const grid;

  // maximum position (excluded)
  /** @brief max value for row (excluded).*/
  const RowIndex maxRowIndexExcluded;
  /** @brief max value for column (excluded).*/
  const ColumnIndex maxColumnIndexExcluded;
  /** @brief max value for cursor (excluded).*/
  const CursorPosition maxCursorExcluded;

  // last position (valid coordinates)
  /** @brief last valid value for row.*/
  const RowIndex lastRowIndex;
  /** @brief last valid value for column.*/
  const ColumnIndex lastColumnIndex;
  /** @brief last valid value for cursor.*/
  const CursorPosition lastCursor;

  // current position (relative to origin)
  /** @brief Current value for row.*/
  RowIndex cursorRow;
  /** @brief Current value for column.*/
  ColumnIndex cursorColumn;
  /** @brief Current value for cursor.*/
  CursorPosition cursor;

  // movable origin -- the grid is seen as a ring
  /** @brief Absolute row index of the grid origin.*/
  RowIndex baseRow;
  /** @brief Absolute column index of the grid origin.*/
  ColumnIndex baseColumn;
  /** @brief Absolute cursor position of the grid origin.*/
  CursorPosition baseCursor;

  void updateCursorPosition() {
    cursor = cursorPositionFrom(cursorRow, cursorColumn);
  }

  CursorPosition void cursorPositionFrom(RowIndex row, ColumnIndex column) {
    return (CursorPosition)row * (CursorPosition)maxColumnIndexExcluded +
           (CursorPosition)column;
  }

  ColumnIndex void columnIndexClippedFrom(ColumnIndex column) {
    return (column < maxColumnIndexExcluded) ? column : lastColumnIndex;
  }

  RowIndex void rowIndexClippedFrom(ColumnIndex row) {
    return (row < maxRowIndexExcluded) ? row : lastRowIndex;
  }

  ColumnIndex void columnIndexMovedBy(ColumnIndex currentValue,
                                      ColumnIndex move) {
    return (currentValue + move) % maxColumnIndexExcluded;
  }

  RowIndex void rowIndexMovedBy(ColumnIndex currentValue, ColumnIndex move) {
    return (currentValue + move) % maxRowIndexExcluded;
  }

public:
  virtual ~GridOfChars();
  GridOfChars(ColumnIndex columns, RowIndex rows, TypeOfGlyph defaultGlyph,
              TypeOfGlyph *buffer)
      : maxRowIndexExcluded(rows), maxColumnIndexExcluded(columns),
        maxCursorExcluded(rows * columns), //
        lastRowIndex(rows - 1), lastColumnIndex(columns - 1),
        lastCursor(rows * columns - 1),           //
        eraseGlyph(defaultGlyph), grid(buffer),   //
        cursorRow(0), cursorColumn(0), cursor(0), //
        baseRow(0), baseColumn(0), baseCursor(0)  //
  {
    color = eraseGlyph.color;
    font = eraseGlyph.font;
    style = eraseGlyph.style;
    clear();
  }

  // ======== Mutation API, written to give a fluent syntax. ========
  /** @brief Erase the whole grid.
   * @return this.
   */
  GridOfChars *clear() {
    clearArea(0, 0, maxColumnIndexExcluded, maxRowIndexExcluded);
    return this;
  }
  /** @brief Erase the given row, relative to the origin.
   * @param r The row to erase.
   * @return this.
   */
  GridOfChars *clearRow(RowIndex r) {
    clearArea(0, r, maxColumnIndexExcluded, r + 1);
    return this;
  }
  /** @brief Erase the given group of rows, relative to the origin.
   * @param fromIncluded The first row to erase.
   * @param toExcluded The row before which the erase will stop.
   * @return this.
   */
  GridOfChars *clearRows(RowIndex fromIncluded, RowIndex toExcluded) {
    clearArea(0, fromIncluded, maxColumnIndexExcluded, toExcluded);
    return this;
  }
  /** @brief Erase the given column, relative to the origin.
   * @param c The column to erase.
   * @return this.
   */
  GridOfChars *clearColumn(ColumnIndex c) {
    clearArea(c, 0, c + 1, maxRowIndexExcluded);
    return this;
  }
  /** @brief Erase the given group of columns, relative to the origin.
   * @param fromIncluded The first column to erase.
   * @param toExcluded The column before which the erase will stop.
   * @return this.
   */
  GridOfChars *clearColumn(ColumnIndex fromIncluded, ColumnIndex toExcluded) {
    clearArea(fromIncluded, 0, toExcluded, maxRowIndexExcluded);
    return this;
  }
  /** @brief Erase the given area, relative to the origin.
   * @param fromColumnIncluded The first column of the area to erase
   * @param fromRowIncluded The first row of the area to erase
   * @param toColumnExcluded The area to erase stops before this column
   * @param toRowExcluded The area to erase stops before this row
   * @return this.
   */
  GridOfChars *clearArea(ColumnIndex fromColumnIncluded,
                         RowIndex fromRowIncluded, ColumnIndex toColumnExcluded,
                         RowIndex toRowExcluded) {
    // clip values to acceptable range.
    ColumnIndex fromCol = columnIndexClippedFrom(fromColumnIncluded);
    ColumnIndex toCol = columnIndexClippedFrom(toColumnExcluded);
    RowIndex fromRow = rowIndexClippedFrom(fromRowIncluded);
    RowIndex toRow = rowIndexClippedFrom(toRowExcluded);

    // do something only when each `from` value is before the corresponding `to`
    // value.
    if (toRow <= fromRow || toCol <= fromCol) {
      return this;
    }

    // compute starting position and offsets
    CursorPosition clrCursor =
        baseCursor + cursorPositionFrom(fromRow, fromColumn);
    ColumnIndex countClrColumns = toColumn - fromColumn;
    CursorPosition offsetNextRow =
        (CursorPosition)(maxColumnIndexExcluded - countClrColumns);

    // perform
    TypeOfGlyph *clearingCursor = grid + clrCursor;
    for (RowIndex clrRow = fromRow; clrRow < toRow; clrRow++) {
      for (ColumnIndex clrColumn = fromCol; clrColumn < toCol; clrColumn++) {
        *grid->codePoint = eraseGlyph.codePoint;
        *grid->color = color;
        *grid->font = font;
        *grid->style = style;
        ++grid;
      }
      grid += offsetNextRow;
    }

    // the cursor is put at the start of the cleared area
    cursor = clrCursor;
    cursorRow = fromRow;
    cursorColumn = fromCol;

    // that's all
    return this;
  }

  /** @brief Move the origin of the grid by a relative move of the origin.
   * @param moveRow move of the row coordinate.
   * @param moveColumn move of the column coordinate.
   * @return this.
   */
  GridOfChars *scrollBy(RowIndex moveRow, ColumnIndex moveColumn) {
    baseRow = rowIndexMovedBy(baseRow, moveRow);
    baseColumn = columnIndexMovedBy(baseColumn, moveColumn);
    baseCursor = cursorPositionFrom(baseRow, baseColumn);
  }
  /** @brief Set the origin of the grid.
   * @param absRow the new row coordinate of the origin.
   * @param absColumn the new column coordinate of the origin.
   * @return this.
   */
  GridOfChars *scrollTo(RowIndex absRow, ColumnIndex absColumn) {
    baseRow = rowIndexClippedFrom(absRow);
    baseColumn = columnIndexClippedFrom(absColumn);
    baseCursor = cursorPositionFrom(baseRow, baseColumn);
  }

  /** @brief Change the current color.
   * @param color the new color value.
   * @return this.
   */
  GridOfChars *withColor(TypeOfColor color) {
    this->color = color;
    return this;
  }

  /** @brief Change the current style.
   * @param style the new style value.
   * @return this.
   */
  GridOfChars *withStyle(StyleFlags style) {
    this->style = style;
    return this;
  }

  /** @brief Change the current style.
   * @param font the new font value.
   * @return this.
   */
  GridOfChars *withFont(FontIndex font) {
    this->font = font;
    return this;
  }
};

#endif