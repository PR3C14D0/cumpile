#include "vga.h"
#include "font.h"
#include <stdint.h>

#define BACK_BUFFER ((uint8_t*)0x4000000)

/* Text cursor position in columns/rows */
static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

/* Premium slate color palette */
#define BG_COLOR 0x1E1E2E
#define FG_COLOR 0xF4F4F4

static inline void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= graphics_width || y >= graphics_height) return;
    
    uint8_t* fb = BACK_BUFFER;
    uint32_t offset = y * graphics_pitch + x * (graphics_bpp / 8);
    
    if (graphics_bpp == 32) {
        *(uint32_t*)(fb + offset) = color;
    } else if (graphics_bpp == 24) {
        fb[offset]     = color & 0xFF;        // Blue
        fb[offset + 1] = (color >> 8) & 0xFF;  // Green
        fb[offset + 2] = (color >> 16) & 0xFF; // Red
    } else if (graphics_bpp == 16) {
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        uint16_t color16 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        *(uint16_t*)(fb + offset) = color16;
    }
}

void vga_flip(void) {
    uint8_t* src = BACK_BUFFER;
    uint8_t* dst = (uint8_t*)graphics_framebuffer;
    uint32_t size = graphics_pitch * graphics_height;

    for(uint32_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}

void vga_clear_screen(void) {
    uint8_t* fb = BACK_BUFFER;
    uint32_t bytes_per_pixel = graphics_bpp / 8;
    
    for (uint32_t y = 0; y < graphics_height; y++) {
        for (uint32_t x = 0; x < graphics_width; x++) {
            uint32_t offset = y * graphics_pitch + x * bytes_per_pixel;
            if (graphics_bpp == 32) {
                *(uint32_t*)(fb + offset) = BG_COLOR;
            } else if (graphics_bpp == 24) {
                fb[offset]     = BG_COLOR & 0xFF;
                fb[offset + 1] = (BG_COLOR >> 8) & 0xFF;
                fb[offset + 2] = (BG_COLOR >> 16) & 0xFF;
            } else if (graphics_bpp == 16) {
                uint8_t r = (BG_COLOR >> 16) & 0xFF;
                uint8_t g = (BG_COLOR >> 8) & 0xFF;
                uint8_t b = BG_COLOR & 0xFF;
                uint16_t color16 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                *(uint16_t*)(fb + offset) = color16;
            }
        }
    }
}

static void graphics_scroll() {
    uint8_t* fb = BACK_BUFFER;
    uint32_t shift_bytes = (uint32_t)graphics_pitch * 8; // 8 scanlines
    uint32_t total_bytes = (uint32_t)graphics_pitch * graphics_height;
    
    /* Shift screen pixels up by 8 vertical scanlines */
    for (uint32_t i = 0; i < total_bytes - shift_bytes; i++) {
        fb[i] = fb[i + shift_bytes];
    }
    
    /* Clear the new bottom line with background color */
    uint32_t bytes_per_pixel = graphics_bpp / 8;
    for (uint32_t y = graphics_height - 8; y < graphics_height; y++) {
        for (uint32_t x = 0; x < graphics_width; x++) {
            uint32_t offset = y * graphics_pitch + x * bytes_per_pixel;
            if (graphics_bpp == 32) {
                *(uint32_t*)(fb + offset) = BG_COLOR;
            } else if (graphics_bpp == 24) {
                fb[offset]     = BG_COLOR & 0xFF;
                fb[offset + 1] = (BG_COLOR >> 8) & 0xFF;
                fb[offset + 2] = (BG_COLOR >> 16) & 0xFF;
            } else if (graphics_bpp == 16) {
                uint8_t r = (BG_COLOR >> 16) & 0xFF;
                uint8_t g = (BG_COLOR >> 8) & 0xFF;
                uint8_t b = BG_COLOR & 0xFF;
                uint16_t color16 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                *(uint16_t*)(fb + offset) = color16;
            }
        }
    }
    
    cursor_y = (graphics_height / 8) - 1;
}

static void draw_char(uint32_t cx, uint32_t cy, char c, uint32_t fg, uint32_t bg) {
    uint32_t pixel_x = cx * 8;
    uint32_t pixel_y = cy * 8;
    
    for (uint32_t row = 0; row < 8; row++) {
        uint8_t bits = font_8x8[(unsigned char)c][row];
        for (uint32_t col = 0; col < 8; col++) {
            uint32_t color = (bits & (0x80 >> col)) ? fg : bg;
            put_pixel(pixel_x + col, pixel_y + row, color);
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