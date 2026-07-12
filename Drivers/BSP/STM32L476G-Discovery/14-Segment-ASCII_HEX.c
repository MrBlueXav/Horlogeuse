/*
 *  Project     Segmented LED Display - ASCII Library for for STM32L476-DK glass LCD
 *  @author     David Madison converted by Xavier Halgand
 *  @link       github.com/dmadison/Segmented-LED-Display-ASCII
 *  @license    MIT - Copyright (c) 2017 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
/*----------------------------------------------------------------------------*/
#include <stdint.h>

/*------------ Original encoding ---------------------------------------------*/
//const uint16_t FourteenSegmentASCII[96] = {
//	0x0000, /* (space) */
//	0x4006, /* ! */
//	0x0202, /* " */
//	0x12CE, /* # */
//	0x12ED, /* $ */
//	0x3FE4, /* % */
//	0x2359, /* & */
//	0x0200, /* ' */
//	0x2400, /* ( */
//	0x0900, /* ) */
//	0x3FC0, /* * */
//	0x12C0, /* + */
//	0x0800, /* , */
//	0x00C0, /* - */
//	0x4000, /* . */
//	0x0C00, /* / */
//	0x0C3F, /* 0 */
//	0x0406, /* 1 */
//	0x00DB, /* 2 */
//	0x008F, /* 3 */
//	0x00E6, /* 4 */
//	0x2069, /* 5 */
//	0x00FD, /* 6 */
//	0x0007, /* 7 */
//	0x00FF, /* 8 */
//	0x00EF, /* 9 */
//	0x1200, /* : */
//	0x0A00, /* ; */
//	0x2440, /* < */
//	0x00C8, /* = */
//	0x0980, /* > */
//	0x5083, /* ? */
//	0x02BB, /* @ */
//	0x00F7, /* A */
//	0x128F, /* B */
//	0x0039, /* C */
//	0x120F, /* D */
//	0x0079, /* E */
//	0x0071, /* F */
//	0x00BD, /* G */
//	0x00F6, /* H */
//	0x1209, /* I */
//	0x001E, /* J */
//	0x2470, /* K */
//	0x0038, /* L */
//	0x0536, /* M */
//	0x2136, /* N */
//	0x003F, /* O */
//	0x00F3, /* P */
//	0x203F, /* Q */
//	0x20F3, /* R */
//	0x00ED, /* S */
//	0x1201, /* T */
//	0x003E, /* U */
//	0x0C30, /* V */
//	0x2836, /* W */
//	0x2D00, /* X */
//	0x00EE, /* Y */
//	0x0C09, /* Z */
//	0x0039, /* [ */
//	0x2100, /* \ */
//	0x000F, /* ] */
//	0x2800, /* ^ */
//	0x0008, /* _ */
//	0x0100, /* ` */
//	0x1058, /* a */
//	0x2078, /* b */
//	0x00D8, /* c */
//	0x088E, /* d */
//	0x0858, /* e */
//	0x14C0, /* f */
//	0x048E, /* g */
//	0x1070, /* h */
//	0x1000, /* i */
//	0x0A10, /* j */
//	0x3600, /* k */
//	0x0030, /* l */
//	0x10D4, /* m */
//	0x1050, /* n */
//	0x00DC, /* o */
//	0x0170, /* p */
//	0x0486, /* q */
//	0x0050, /* r */
//	0x2088, /* s */
//	0x0078, /* t */
//	0x001C, /* u */
//	0x0810, /* v */
//	0x2814, /* w */
//	0x2D00, /* x */
//	0x028E, /* y */
//	0x0848, /* z */
//	0x0949, /* { */
//	0x1200, /* | */
//	0x2489, /* } */
//	0x0CC0, /* ~ */
//	0x0000, /* (del) */
//};

/* Converted encoding for STM32L476-DK glass LCD */
const uint16_t FourteenSegmentASCII_STM32[113] = {
    0x0000, /*    >> idx 0*/
    0x4202, /*  ! */
    0x4004, /*  " */
    0xE314, /*  # */
    0xAF14, /*  $ */
    0xAAC9, /*  % >> idx 5*/
    0x950D, /*  & */
    0x0004, /*  ' */
    0x0041, /*  ( */
    0x0088, /*  ) */
    0xA0DD, /*  * >> idx 10*/
    0xA014, /*  + */
    0x0080, /*  , */
    0xA000, /*  - */
    0x0002, /*  . */
    0x00C0, /*  / */
    0x5FC0, /*  0 */
    0x4240, /*  1 */
    0xF500, /*  2 */
    0x6700, /*  3 */
    0xEA00, /*  4 >> idx 20*/
    0x8D01, /*  5 */
    0xBF00, /*  6 */
    0x4600, /*  7 */
    0xFF00, /*  8 */
    0xEF00, /*  9 */
    0x0020, /*  : */
    0x00A0, /*  ; */
    0x8041, /*  < */
    0xA100, /*  = */
    0x2088, /*  > >> idx 30*/
    0x6412, /*  ? */
    0x7D04, /*  @ */
    0xFE00, /*  A */
    0x6714, /*  B */
    0x1D00, /*  C */
    0x4714, /*  D */
    0x9D00, /*  E */
    0x9C00, /*  F */
    0x3F00, /*  G */
    0xFA00, /*  H >> idx 40*/
    0x0514, /*  I */
    0x5300, /*  J */
    0x9841, /*  K */
    0x1900, /*  L */
    0x5A48, /*  M */
    0x5A09, /*  N */
    0x5F00, /*  O */
    0xFC00, /*  P */
    0x5F01, /*  Q */
    0xFC01, /*  R >> idx 50*/
    0xAF00, /*  S */
    0x0414, /*  T */
    0x5B00, /*  U */
    0x18C0, /*  V */
    0x5A81, /*  W */
    0x00C9, /*  X */
    0xEB00, /*  Y */
    0x05C0, /*  Z */
    0x1D00, /*  [ */
    0x0009, /*  \ >> idx 60*/
    0x4700, /*  ] */
    0x0081, /*  ^ */
    0x0100, /*  _ */
    0x0008, /*  ` */
    0x9110, /*  a */
    0x9901, /*  b */
    0xB100, /*  c */
    0x6380, /*  d */
    0x9180, /*  e */
    0xA050, /*  f >> idx 70*/
    0x6340, /*  g */
    0x9810, /*  h */
    0x0010, /*  i */
    0x1084, /*  j */
    0x0055, /*  k */
    0x1800, /*  l */
    0xB210, /*  m */
    0x9010, /*  n */
    0xB300, /*  o */
    0x9808, /*  p >> idx 80*/
    0x6240, /*  q */
    0x9000, /*  r */
    0x2101, /*  s */
    0x9900, /*  t */
    0x1300, /*  u */
    0x1080, /*  v */
    0x1281, /*  w */
    0x00C9, /*  x */
    0x6304, /*  y */
    0x8180, /*  z >> idx 90*/
    0x8588, /*  { */
    0x0014, /*  | */
    0x2541, /*  } */
    0xA0C0, /*  ~ */
    0x0000, /*   */
	0xE910, /* SYMBOL0 */
	0x0448, /* SYMBOL1 */
	0x0181, /* SYMBOL2 */
	0x05C9, /* SYMBOL3 */
	0x5A14, /* SYMBOL4 >> idx 100*/
	0xA500, /* SYMBOL5 */
	0xE885, /* SYMBOL6 */
	0xF0C0, /* SYMBOL7 */
	0xA181, /* SYMBOL8 */
	0x5F14, /* SYMBOL9 */
	0x0058, /* SYMBOL10 */
	0x0458, /* SYMBOL11 */
	0x5FC9, /* SYMBOL12 */
	0xEC14, /* SYMBOL13 */
	0x8088, /* SYMBOL14 */ /* >> idx 110 */
	0x2041, /* SYMBOL15 */
	0xA081, /* SYMBOL16 */
};
/*----------------------------------------------------------------------------*/
