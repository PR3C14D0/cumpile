#include "vga.h"
#include "font.h"
#include <stdint.h>



/* Text cursor position in columns/rows */
static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

/* Premium slate color palette */
#define BG_COLOR 0x1E1E2E
#define FG_COLOR 0xF4F4F4

static void graphics_scroll() {
    uint32_t* fb = (uint32_t*)graphics_framebuffer;
    uint32_t row_pixels = (uint32_t)graphics_width * 8; // 8 scanlines
    uint32_t total_pixels = (uint32_t)graphics_width * graphics_height;
    
    /* Shift screen pixels up by 8 vertical scanlines */
    for (uint32_t i = 0; i < total_pixels - row_pixels; i++) {
        fb[i] = fb[i + row_pixels];
    }
    
    /* Clear the new bottom line with background color */
    for (uint32_t i = total_pixels - row_pixels; i < total_pixels; i++) {
        fb[i] = BG_COLOR;
    }
    
    cursor_y = (graphics_height / 8) - 1;
}

static void draw_char(uint32_t cx, uint32_t cy, char c, uint32_t fg, uint32_t bg) {
    uint32_t* fb = (uint32_t*)graphics_framebuffer;
    uint32_t pixel_x = cx * 8;
    uint32_t pixel_y = cy * 8;
    
    for (uint32_t row = 0; row < 8; row++) {
        uint8_t bits = font_8x8[(unsigned char)c][row];
        for (uint32_t col = 0; col < 8; col++) {
            uint32_t color = (bits & (0x80 >> col)) ? fg : bg;
            fb[(pixel_y + row) * graphics_width + (pixel_x + col)] = color;
        }
    }
}

void vga_putc(char c) {
    uint32_t cols = graphics_width / 8;
    uint32_t rows = graphics_height / 8;
    
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else {
        draw_char(cursor_x, cursor_y, c, FG_COLOR, BG_COLOR);
        cursor_x++;
    }
    
    if (cursor_x >= cols) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= rows) {
        graphics_scroll();
    }
}

void vga_print(const char* pcStr) {
    while (*pcStr) {
        vga_putc(*pcStr++);
    }
}

void vga_print_hex(uint32_t val) {
    char szHex[] = "0123456789ABCDEF";
    vga_print("0x");
    for (int i = 28; i >= 0; i -= 4) {
        vga_putc(szHex[(val >> i) & 0x0F]);
    }
}

void vga_print_dec(uint32_t val) {
    if (val == 0) {
        vga_putc('0');
        return;
    }
    char szBuf[12];
    int i = 10;
    szBuf[11] = '\0';
    while (val > 0) {
        szBuf[--i] = '0' + (val % 10);
        val /= 10;
    }
    vga_print(&szBuf[i]);
}