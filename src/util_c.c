#include "util_c.h"

// C-language utility method definitions.
// Initialize a 96x64-px SSD1331 display.
// TODO: Constants/macros for command values.
void ssd1331_start_sequence(void) {
  // Instead of taking an existing library's word for
  // these, use a command sequence listed in the
  // SSD1331 datasheet's "Command Table", Section 8.
  // (But check out Adafruit for boards and a library!)
  // That table may be sorted by hex value rather than
  // ideal command ordering, but let's see what happens.
  // 'Unlock Display.' - 0xFD/0x16 can 'Lock' it,
  // in which case all other commands are ignored.
  sspi_cmd(0xFD);
  sspi_cmd(0x12);
  // (Turn the display off.)
  sspi_cmd(0xAE);

  // 'Set Column Address' - default is 0-95, which is
  // also what we want.
  sspi_cmd(0x15);
  sspi_cmd(0x00);
  sspi_cmd(0x5F);
  // 'Set Row Address' - default is 0-63, which is good.
  sspi_cmd(0x75);
  sspi_cmd(0x00);
  sspi_cmd(0x3F);

  // 'Set Color A Contrast' - default is 128.
  sspi_cmd(0x81);
  sspi_cmd(0x80);
  // 'Set Color B Contrast' - default is 128, use 96.
  sspi_cmd(0x82);
  sspi_cmd(0x60);
  // 'Set Color C Contrast' - default is 128.
  sspi_cmd(0x83);
  sspi_cmd(0x80);
  // 'Set Master Current Control' - default is 15, but
  // use 8 for ~half. (~= 'Set Brightness')
  sspi_cmd(0x87);
  sspi_cmd(0x08);
  // 'Set Precharge A' - default is 'Color A Contrast'.
  sspi_cmd(0x8A);
  sspi_cmd(0x80);
  // 'Set Precharge B' - default is 'Color B Contrast'.
  sspi_cmd(0x8B);
  sspi_cmd(0x60);
  // 'Set Precharge C' - default is 'Color C Contrast'.
  sspi_cmd(0x8C);
  sspi_cmd(0x80);
  // 'Remap Display Settings' - default is 0x40.
  // Use 0x60 to avoid drawing lines in odd-even order.
  sspi_cmd(0xA0);
  //sspi_cmd(0x60);
  // (0x70 to flip vertically)
  //sspi_cmd(0x70);
  // (And 0x72 to flip horizontally)
  sspi_cmd(0x72);
  // 'Set Display Start Row' - default is 0.
  sspi_cmd(0xA1);
  sspi_cmd(0x00);
  // 'Set Vertical Offset' - default is 0.
  sspi_cmd(0xA2);
  sspi_cmd(0x00);
  // 'Set Display Mode' - default is 'A4'. 'A7' = invert.
  // (The actual command byte sets the mode; no 'arg')
  sspi_cmd(0xA4);
  // 'Set Multiplex Ratio.' I think this is how many
  // rows of pixels are actually enabled; default is 63.
  sspi_cmd(0xA8);
  sspi_cmd(0x3F);
  // (I am going to ignore the 0xAB 'Dim Mode Settings'
  // command - it looks like it only matters if we use
  // the 0xAC 'Dim Display' command; we will use 0xAF.)
  // 'Set Voltage Supply Configuration'. The SSD1331 has
  // no onboard charge pump, so we must use external
  // voltage. (0x8E)
  sspi_cmd(0xAB);
  sspi_cmd(0x8E);
  // 'Set Power Save Mode'. Default enabled; disable it.
  // ('on' is 0x1A, 'off' is 0x0B)
  sspi_cmd(0xB0);
  sspi_cmd(0x0B);
  // 'Adjust Precharge Phases.' Bits [7:4] set the
  // precharge stage 2 period, bits [3:0] set phase 1.
  // Default is 0x74.
  sspi_cmd(0xB1);
  sspi_cmd(0x74);
  // 'Set Clock Divider Frequency'. Bits [7:4] set the
  // oscillator frequency, bits [3:0]+1 set the
  // clock division ratio. Default is 0xD0.
  sspi_cmd(0xB3);
  sspi_cmd(0xD0);
  // (I am going to ignore the 'Set Grayscale Table'
  // command - it has a bunch of gamma curve settings.)
  // So, the 'Reset to Default Grayscale Table'
  // command does make sense to call.
  sspi_cmd(0xB9);
  // 'Set Precharge Level'. Default is 0x3E.
  sspi_cmd(0xBB);
  sspi_cmd(0x3E);
  // 'Set Logic 0 Threshold'. Default is 0x3E = 0.83*VCC.
  sspi_cmd(0xBE);
  sspi_cmd(0x3E);
  // 'Display On'.
  sspi_cmd(0xAF);
}

/*
 * Draw a horizontal line.
 */
inline void oled_draw_h_line(int x, int y,
                             int w, uint8_t color) {
  int x_pos = x;
  int line_end = x + w;
  int fb_ind;
  // Make sure that the line won't overflow.
  if (x > 95) { return; }
  if (line_end > 96) { line_end = 96; }
  // Draw the line.
  for (x_pos = x; x_pos < line_end; ++x_pos) {
    // (2 pixels per byte)
    fb_ind = (x_pos + (y * 96)) / 2;
    oled_fb[fb_ind] &= ~((0x0F) << (4 * !(x_pos % 2)));
    oled_fb[fb_ind] |= (color & 0x0F) << (4 * !(x_pos % 2));
  }
}

/*
 * Draw a veritcal line.
 */
inline void oled_draw_v_line(int x, int y,
                             int h, uint8_t color) {
  int y_pos = y;
  int line_end = y + h;
  int fb_ind;
  // Make sure that the line won't overflow.
  if (y > 63) { return; }
  if (line_end > 64) { line_end = 64; }
  // Draw the line.
  for (y_pos = y; y_pos < line_end; ++y_pos) {
    // (2 pixels per byte)
    fb_ind = (x + (y_pos * 96)) / 2;
    oled_fb[fb_ind] &= ~((0x0F) << (4 * !(x % 2)));
    oled_fb[fb_ind] |= (color & 0x0F) << (4 * !(x % 2));
  }
}

/*
 * Draw a rectangle on the display.
 * I guess just pick the longer dimension, and either draw
 * horizontal or vertical lines.
 * Notable args:
 *   - outline: If <=0, fill the rectangle with 'color'.
 *        If >0, draw an outline inside the dimensions of N pixels.
 *   - color: If 0, clear drawn bits. If not 0, set drawn bits.
 */
inline void oled_draw_rect(int x, int y, int w, int h,
                           int outline, uint8_t color) {
  if (outline > 0) {
    // Draw an outline.
    int o_pos;
    // Top.
    for (o_pos = y; o_pos < (y+outline); ++o_pos) {
      oled_draw_h_line(x, o_pos, w, color);
    }
    // Bottom.
    for (o_pos = (y+h-1); o_pos > (y+h-1-outline); --o_pos) {
      oled_draw_h_line(x, o_pos, w, color);
    }
    // Left.
    for (o_pos = x; o_pos < (x+outline); ++o_pos) {
      oled_draw_v_line(o_pos, y, h, color);
    }
    // Right.
    for (o_pos = (x+w-1); o_pos > (x+w-1-outline); --o_pos) {
      oled_draw_v_line(o_pos, y, h, color);
    }
  }
  else {
    // Draw a filled rectangle.
    if (w > h) {
      // Draw fewer horizontal lines than vertical ones.
      int y_pos;
      for (y_pos = y; y_pos < (y+h); ++y_pos) {
        oled_draw_h_line(x, y_pos, w, color);
      }
    }
    else {
      // Draw fewer (or ==) vertical lines than horizontal ones.
      int x_pos;
      for (x_pos = x; x_pos < (x+w); ++x_pos) {
        oled_draw_v_line(x_pos, y, h, color);
      }
    }
  }
}

/*
 * Write a pixel in the current OLED framebuffer.
 * Note that the positioning is a bit odd; each byte is a VERTICAL column
 * of 8 pixels, but each successive byte increments the row position by 1.
 * This means that the buffer is 8x 128-byte pages stacked on top of one
 * another. To set an (x, y) pixel, we |= one position in one byte.
 *   Byte offset = x + ((y / 8) * 128)
 *   Bit offset  = (y & 0x07)
 * 'color' indicates whether to set or unset the pixel. 0 means 'unset.'
 */
inline void oled_write_pixel(int x, int y, uint8_t color) {
  int fb_ind = (x + (y * 96)) / 2;
  oled_fb[fb_ind] &= ~((0x0F) << (4 * !(x % 2)));
  oled_fb[fb_ind] |= (color & 0x0F) << (4 * !(x % 2));
}

void oled_draw_letter(int x, int y, unsigned int w0, unsigned int w1, uint8_t color, char size) {
  // TODO: Make this more efficient than drawing
  // pixels one-by-one.
  int w_iter = 0;
  int cur_x = x;
  int cur_y = y;
  unsigned int aw0 = w0;
  unsigned int aw1 = w1;
  if (!color) {
    aw0 = ~aw0;
    aw1 = ~aw1;
  }
  int px_incr = 1;
  int line_h = 8;
  unsigned char t_col = 0x00;
  int cx = cur_x;
  int cy = cur_y;
  if (size == 'L') {
    px_incr = 2;
    line_h = 16;
  }
  for (w_iter = 31; w_iter >= 0; --w_iter) {
    t_col = !(!(aw0 & (1 << w_iter)));
    for (cx = cur_x; cx < cur_x + px_incr; ++cx) {
      for (cy = cur_y; cy < cur_y + px_incr; ++cy) {
        if (t_col) {
          oled_write_pixel(cx, cy, color);
        }
      }
    }
    cur_y += px_incr;
    if (cur_y == y+line_h) {
      cur_y = y;
      cur_x += px_incr;
    }
  }
  for (w_iter = 15; w_iter >= 0; --w_iter) {
    t_col = !(!(aw1 & (1 << w_iter)));
    for (cx = cur_x; cx < cur_x + px_incr; ++cx) {
      for (cy = cur_y; cy < cur_y + px_incr; ++cy) {
        if (t_col) {
          oled_write_pixel(cx, cy, color);
        }
      }
    }
    cur_y += px_incr;
    if (cur_y == y+line_h) {
      cur_y = y;
      cur_x += px_incr;
    }
  }
}

void oled_draw_letter_c(int x, int y, char c, uint8_t color, char size) {
  unsigned int w0 = 0x00;
  unsigned int w1 = 0x00;
  if (c == 'A') {
    w0 = OLED_CH_A0;
    w1 = OLED_CH_A1B1 >> 16;
  }
  else if (c == 'B') {
    w0 = OLED_CH_B0;
    w1 = OLED_CH_A1B1 & 0x0000FFFF;
  }
  else if (c == 'C') {
    w0 = OLED_CH_C0;
    w1 = OLED_CH_C1D1 >> 16;
  }
  else if (c == 'D') {
    w0 = OLED_CH_D0;
    w1 = OLED_CH_C1D1 & 0x0000FFFF;
  }
  else if (c == 'E') {
    w0 = OLED_CH_E0;
    w1 = OLED_CH_E1F1 >> 16;
  }
  else if (c == 'F') {
    w0 = OLED_CH_F0;
    w1 = OLED_CH_E1F1 & 0x0000FFFF;
  }
  else if (c == 'G') {
    w0 = OLED_CH_G0;
    w1 = OLED_CH_G1H1 >> 16;
  }
  else if (c == 'H') {
    w0 = OLED_CH_H0;
    w1 = OLED_CH_G1H1 & 0x0000FFFF;
  }
  else if (c == 'I') {
    w0 = OLED_CH_I0;
    w1 = OLED_CH_I1J1 >> 16;
  }
  else if (c == 'J') {
    w0 = OLED_CH_J0;
    w1 = OLED_CH_I1J1 & 0x0000FFFF;
  }
  else if (c == 'K') {
    w0 = OLED_CH_K0;
    w1 = OLED_CH_K1L1 >> 16;
  }
  else if (c == 'L') {
    w0 = OLED_CH_L0;
    w1 = OLED_CH_K1L1 & 0x0000FFFF;
  }
  else if (c == 'M') {
    w0 = OLED_CH_M0;
    w1 = OLED_CH_M1N1 >> 16;
  }
  else if (c == 'N') {
    w0 = OLED_CH_N0;
    w1 = OLED_CH_M1N1 & 0x0000FFFF;
  }
  else if (c == 'O') {
    w0 = OLED_CH_O0;
    w1 = OLED_CH_O1P1 >> 16;
  }
  else if (c == 'P') {
    w0 = OLED_CH_P0;
    w1 = OLED_CH_O1P1 & 0x0000FFFF;
  }
  else if (c == 'Q') {
    w0 = OLED_CH_Q0;
    w1 = OLED_CH_Q1R1 >> 16;
  }
  else if (c == 'R') {
    w0 = OLED_CH_R0;
    w1 = OLED_CH_Q1R1 & 0x0000FFFF;
  }
  else if (c == 'S') {
    w0 = OLED_CH_S0;
    w1 = OLED_CH_S1T1 >> 16;
  }
  else if (c == 'T') {
    w0 = OLED_CH_T0;
    w1 = OLED_CH_S1T1 & 0x0000FFFF;
  }
  else if (c == 'U') {
    w0 = OLED_CH_U0;
    w1 = OLED_CH_U1V1 >> 16;
  }
  else if (c == 'V') {
    w0 = OLED_CH_V0;
    w1 = OLED_CH_U1V1 & 0x0000FFFF;
  }
  else if (c == 'W') {
    w0 = OLED_CH_W0;
    w1 = OLED_CH_W1X1 >> 16;
  }
  else if (c == 'X') {
    w0 = OLED_CH_X0;
    w1 = OLED_CH_W1X1 & 0x0000FFFF;
  }
  else if (c == 'Y') {
    w0 = OLED_CH_Y0;
    w1 = OLED_CH_Y1Z1 >> 16;
  }
  else if (c == 'Z') {
    w0 = OLED_CH_Z0;
    w1 = OLED_CH_Y1Z1 & 0x0000FFFF;
  }
  else if (c == 'a') {
    w0 = OLED_CH_a0;
    w1 = OLED_CH_a1b1 >> 16;
  }
  else if (c == 'b') {
    w0 = OLED_CH_b0;
    w1 = OLED_CH_a1b1 & 0x0000FFFF;
  }
  else if (c == 'c') {
    w0 = OLED_CH_c0;
    w1 = OLED_CH_c1d1 >> 16;
  }
  else if (c == 'd') {
    w0 = OLED_CH_d0;
    w1 = OLED_CH_c1d1 & 0x0000FFFF;
  }
  else if (c == 'e') {
    w0 = OLED_CH_e0;
    w1 = OLED_CH_e1f1 >> 16;
  }
  else if (c == 'f') {
    w0 = OLED_CH_f0;
    w1 = OLED_CH_e1f1 & 0x0000FFFF;
  }
  else if (c == 'g') {
    w0 = OLED_CH_g0;
    w1 = OLED_CH_g1h1 >> 16;
  }
  else if (c == 'h') {
    w0 = OLED_CH_h0;
    w1 = OLED_CH_g1h1 & 0x0000FFFF;
  }
  else if (c == 'i') {
    w0 = OLED_CH_i0;
    w1 = OLED_CH_i1j1 >> 16;
  }
  else if (c == 'j') {
    w0 = OLED_CH_j0;
    w1 = OLED_CH_i1j1 & 0x0000FFFF;
  }
  else if (c == 'k') {
    w0 = OLED_CH_k0;
    w1 = OLED_CH_k1l1 >> 16;
  }
  else if (c == 'l') {
    w0 = OLED_CH_l0;
    w1 = OLED_CH_k1l1 & 0x0000FFFF;
  }
  else if (c == 'm') {
    w0 = OLED_CH_m0;
    w1 = OLED_CH_m1n1 >> 16;
  }
  else if (c == 'n') {
    w0 = OLED_CH_n0;
    w1 = OLED_CH_m1n1 & 0x0000FFFF;
  }
  else if (c == 'o') {
    w0 = OLED_CH_o0;
    w1 = OLED_CH_o1p1 >> 16;
  }
  else if (c == 'p') {
    w0 = OLED_CH_p0;
    w1 = OLED_CH_o1p1 & 0x0000FFFF;
  }
  else if (c == 'q') {
    w0 = OLED_CH_q0;
    w1 = OLED_CH_q1r1 >> 16;
  }
  else if (c == 'r') {
    w0 = OLED_CH_r0;
    w1 = OLED_CH_q1r1 & 0x0000FFFF;
  }
  else if (c == 's') {
    w0 = OLED_CH_s0;
    w1 = OLED_CH_s1t1 >> 16;
  }
  else if (c == 't') {
    w0 = OLED_CH_t0;
    w1 = OLED_CH_s1t1 & 0x0000FFFF;
  }
  else if (c == 'u') {
    w0 = OLED_CH_u0;
    w1 = OLED_CH_u1v1 >> 16;
  }
  else if (c == 'v') {
    w0 = OLED_CH_v0;
    w1 = OLED_CH_u1v1 & 0x0000FFFF;
  }
  else if (c == 'w') {
    w0 = OLED_CH_w0;
    w1 = OLED_CH_w1x1 >> 16;
  }
  else if (c == 'x') {
    w0 = OLED_CH_x0;
    w1 = OLED_CH_w1x1 & 0x0000FFFF;
  }
  else if (c == 'y') {
    w0 = OLED_CH_y0;
    w1 = OLED_CH_y1z1 >> 16;
  }
  else if (c == 'z') {
    w0 = OLED_CH_z0;
    w1 = OLED_CH_y1z1 & 0x0000FFFF;
  }
  else if (c == '0') {
    w0 = OLED_CH_00;
    w1 = OLED_CH_0111 >> 16;
  }
  else if (c == '1') {
    w0 = OLED_CH_10;
    w1 = OLED_CH_0111 & 0x0000FFFF;
  }
  else if (c == '2') {
    w0 = OLED_CH_20;
    w1 = OLED_CH_2131 >> 16;
  }
  else if (c == '3') {
    w0 = OLED_CH_30;
    w1 = OLED_CH_2131 & 0x0000FFFF;
  }
  else if (c == '4') {
    w0 = OLED_CH_40;
    w1 = OLED_CH_4151 >> 16;
  }
  else if (c == '5') {
    w0 = OLED_CH_50;
    w1 = OLED_CH_4151 & 0x0000FFFF;
  }
  else if (c == '6') {
    w0 = OLED_CH_60;
    w1 = OLED_CH_6171 >> 16;
  }
  else if (c == '7') {
    w0 = OLED_CH_70;
    w1 = OLED_CH_6171 & 0x0000FFFF;
  }
  else if (c == '8') {
    w0 = OLED_CH_80;
    w1 = OLED_CH_8191 >> 16;
  }
  else if (c == '9') {
    w0 = OLED_CH_90;
    w1 = OLED_CH_8191 & 0x0000FFFF;
  }
  else if (c == ':') {
    w0 = OLED_CH_col0;
    w1 = OLED_CH_col1per1 >> 16;
  }
  else if (c == '.') {
    w0 = OLED_CH_per0;
    w1 = OLED_CH_col1per1 & 0x0000FFFF;
  }
  else if (c == '!') {
    w0 = OLED_CH_exc0;
    w1 = OLED_CH_exc1fws1 >> 16;
  }
  else if (c == '/') {
    w0 = OLED_CH_fws0;
    w1 = OLED_CH_exc1fws1 & 0x0000FFFF;
  }
  else if (c == '-') {
    w0 = OLED_CH_hyp0;
    w1 = OLED_CH_hyp1pls1 >> 16;
  }
  else if (c == '+') {
    w0 = OLED_CH_pls0;
    w1 = OLED_CH_hyp1pls1 & 0x0000FFFF;
  }
  else if (c == '<') {
    w0 = OLED_CH_lct0;
    w1 = OLED_CH_lct1rct1 >> 16;
  }
  else if (c == '>') {
    w0 = OLED_CH_rct0;
    w1 = OLED_CH_lct1rct1 & 0x0000FFFF;
  }
  oled_draw_letter(x, y, w0, w1, color, size);
}

void oled_draw_letter_i(int x, int y, int ic, uint8_t color, char size) {
  int magnitude = 1000000000;
  int cur_x = x;
  int first_found = 0;
  int proc_val = ic;
  if (proc_val < 0) {
    proc_val = proc_val * -1;
    oled_draw_letter_c(cur_x, y, '-', color, size);
    if (size == 'S') {
      cur_x += 6;
    }
    else if (size == 'L') {
      cur_x += 12;
    }
  }
  for (magnitude = 1000000000; magnitude > 0; magnitude = magnitude / 10) {
    int m_val = proc_val / magnitude;
    proc_val -= (m_val * magnitude);
    if (m_val > 0 || first_found || magnitude == 1) {
      first_found = 1;
      char mc = ' ';
      if (m_val == 0) {
        mc = '0';
      }
      else if (m_val == 1) {
        mc = '1';
      }
      else if (m_val == 2) {
        mc = '2';
      }
      else if (m_val == 3) {
        mc = '3';
      }
      else if (m_val == 4) {
        mc = '4';
      }
      else if (m_val == 5) {
        mc = '5';
      }
      else if (m_val == 6) {
        mc = '6';
      }
      else if (m_val == 7) {
        mc = '7';
      }
      else if (m_val == 8) {
        mc = '8';
      }
      else if (m_val == 9) {
        mc = '9';
      }
      oled_draw_letter_c(cur_x, y, mc, color, size);
      if (size == 'S') {
        cur_x += 6;
      }
      else if (size == 'L') {
        cur_x += 12;
      }
      if (cur_x >= 128) { return; }
    }
  }
}

void oled_draw_text(int x, int y, char* cc, uint8_t color, char size) {
  int i = 0;
  int offset = 0;
  while (cc[i] != '\0') {
    oled_draw_letter_c(x + offset, y, cc[i], color, size);
    if (size == 'S') {
      offset += 6;
    }
    else if (size == 'L') {
      offset += 12;
    }
    ++i;
  }
}

void sspi_stream_framebuffer(void) {
  uint16_t px_i = 0;
  uint16_t px_val = 0;
  uint8_t px_col = 0;
  // Draw the buffer.
  for (px_i = 0; px_i < OLED_FB_SIZE; ++px_i) {
    px_col = oled_fb[px_i] >> 4;
    px_val = oled_colors[px_col];
    sspi_w(px_val >> 8);
    sspi_w(px_val & 0x00FF);
    px_col = oled_fb[px_i] & 0x0F;
    px_val = oled_colors[px_col];
    sspi_w(px_val >> 8);
    sspi_w(px_val & 0x00FF);
  }
}
