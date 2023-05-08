#ifndef SEVEN_SEGMENTS_FONT_HPP
#define SEVEN_SEGMENTS_FONT_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "SevenSegmentsTypes.hpp"

/** @brief Digit data to use when a font does not covers a given code point.*/
const SevenSegmentsDigit DEFAULT_GLYPH = 0 ;

/** @brief Description of a font for a 7 segments display.
 * A font covers a range of 'code points' (e.g. ascii code), starting from
 * `firstCodePoint` included to `firstCodePoint + glyphCount` excluded.
 * 
 * Outside the covered range, the value to be use is `DEFAULT_GLYPH`.
 */
struct SevenSegmentFont {
  /** @brief The first code point covered by this font. */
  uint8_t firstCodePoint;
  /** @brief The number of glyphes. */
  uint8_t glyphCount;
  /** @brief The actual data for each glyph.*/
  const SevenSegmentsDigit * const glyphData;
};

// write code here
extern const SevenSegmentFont SevenSegmentsFontUsAscii ;

#endif