#pragma once

// Simple Unicode icons (rendered with standard fonts, no external font needed)
// These use standard Unicode blocks that are included in the CJK font ranges.
// For full Font Awesome icons, merge Font Awesome TTF into the main font.

#define ICON_HOME    "\xEE\x80\xA5"  // Using simple ASCII-art style
#define ICON_ESP     "\xEE\x80\xA6"
#define ICON_AIM     "\xEE\x80\xA7"
#define ICON_MEM     "\xEE\x80\xA8"
#define ICON_SET     "\xEE\x80\xA9"

// Use simple text symbols that render in ANY font
// These are clean, consistent, and guaranteed to work
#define ICO_HOME  "[H]"
#define ICO_ESP   "[E]"
#define ICO_AIM   "[A]"
#define ICO_MEM   "[M]"
#define ICO_SET   "[S]"

// Better approach: use common Unicode symbols that ARE in CJK fonts
// Geometric Shapes block (U+25A0-U+25FF) - widely supported
#define ICO_POWER  "\u23FB"  // ⏻ power symbol (may not render)
#define ICO_EYE    "\u25C9"  // ◉ eye-like
#define ICO_CROSS  "\u2716"  // ✖ cross
#define ICO_STAR   "\u2605"  // ★ star
#define ICO_GEAR   "\u2699"  // ⚙ gear
#define ICO_SQUARE "\u25A1"  // □ square
#define ICO_CIRCLE "\u25CB"  // ○ circle
#define ICO_DIAMOND "\u25C7" // ◇ diamond
#define ICO_TRIANGLE "\u25B3" // △ triangle
#define ICO_ARROW_R "\u25B6" // ▶ right arrow
#define ICO_ARROW_D "\u25BC" // ▼ down arrow
#define ICO_PLUS   "\u271A"  // ✚ plus
#define ICO_MINUS  "\u2796"  // ➖ minus
#define ICO_CHECK  "\u2714"  // ✔ check
#define ICO_X      "\u2718"  // ✘ cross
#define ICO_HEART  "\u2665"  // ♥ heart
#define ICO_BULLET "\u25CF"  // ● filled circle
#define ICO_HALF   "\u25D0"  // ◐ half circle
#define ICO_SHIELD "\u25C8"  // ◈ shield-like
#define ICO_WARN   "\u26A0"  // ⚠ warning
#define ICO_INFO   "\u2139"  // ℹ info
#define ICO_PLAY   "\u25B6"  // ▶ play
#define ICO_STOP   "\u25A0"  // ■ stop
#define ICO_RECORD "\u25C9"  // ◉ record
#define ICO_REFRESH "\u21BB" // ↻ refresh
#define ICO_EXIT   "\u2716"  // ✖ exit

// For sections - use clean text with brackets
#define SEC_HOME   "\u2302"  // ⌂ house (may not render, fallback to text)
#define SEC_ESP    "\u25C9"  // ◉
#define SEC_AIM    "\u25CE"  // ◎ bullseye
#define SEC_MEM    "\u25A3"  // ▣ memory
#define SEC_SET    "\u2699"  // ⚙ gear
