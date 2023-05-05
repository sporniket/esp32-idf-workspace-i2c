// TODO : GPL notice
#ifndef GLYPH_MODEL_HPP
#define GLYPH_MODEL_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes

/** @brief ASCII character code*/
using AsciiChar = uint8_t ;
/** @brief Unicode character code on 2 bytes */
using UnicodeChar16 = uint16_t ; 
/** @brief Unicode character code on 4 bytes */
using UnicodeChar32 = uint32_t ; 

/** @brief Font to be used, as an index in a registry of fonts. */
using FontIndex = uint8_t ;

/** @brief Styling of a glyph, as a collection of flags, i.e. a bitmap. */
using StyleFlags = uint8_t ;

/** @brief Color as an index in a registry of colors (palette). */
using ColorIndex = uint8_t ;
/** @brief Color as a tuple of primary red, green, blue values (additive synthesis).*/
struct ColorRgb {
    ColorIndex red ;
    ColorIndex green ;
    ColorIndex blue ;
} ;

/** @brief The model of a glyph to display.
 */
template <typename TypeOfChar, typename TypeOfColor>
struct GlyphModel {
    /**@brief the character code*/
    TypeOfChar codePoint ;
    /**@brief the color to use*/
    TypeOfColor color ;
    /**@brief the font to use*/
    FontIndex font;
    /**@brief the style to use*/
    StyleFlags style;
} ;

/** @brief Model for an ascii char with indexed color. */
using AsciiGlyphIndexedColor = GlyphModel<AsciiChar,ColorIndex> ;

/** @brief Model for an ascii char with RGB described color. */
using AsciiGlyphRgbColor = GlyphModel<AsciiChar,ColorRgb> ;

/** @brief Model for a Unicode char on 16 bits with indexed color. */
using Unicode16GlyphIndexedColor = GlyphModel<UnicodeChar16,ColorIndex> ;

/** @brief Model for a Unicode char on 16 bits with RGB described color. */
using Unicode16GlyphIndexedColor = GlyphModel<UnicodeChar16,ColorRgb> ;

/** @brief Model for a Unicode char on 32 bits with indexed color. */
using Unicode32GlyphIndexedColor = GlyphModel<UnicodeChar32,ColorIndex> ;

/** @brief Model for a Unicode char on 32 bits with RGB described color. */
using Unicode32GlyphIndexedColor = GlyphModel<UnicodeChar32,ColorRgb> ;

#endif