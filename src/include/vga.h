#pragma once
#include <stdint.h>

struct graphics_info_t {
    uint32_t framebuffer;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    uint16_t pitch;
} __attribute__((packed));

#define GRAPHICS_INFO ((struct graphics_info_t*)0x6000)
#define graphics_framebuffer (GRAPHICS_INFO->framebuffer)
#define graphics_width       (GRAPHICS_INFO->width)
#define graphics_height      (GRAPHICS_INFO->height)
#define graphics_bpp         (GRAPHICS_INFO->bpp)
#define graphics_pitch       (GRAPHICS_INFO->pitch)

void vga_clear_screen(void);

void vga_putc(char c);
void vga_print(const char* pcStr);
void vga_print_hex(uint32_t val);
void vga_print_dec(uint32_t val);