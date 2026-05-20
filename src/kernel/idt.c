#include "idt.h"
#include "vga.h"

extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10();
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_21();
extern void isr_22();
extern void isr_23();
extern void isr_24();
extern void isr_25();
extern void isr_26();
extern void isr_27();
extern void isr_28();
extern void isr_29();
extern void isr_30();
extern void isr_31();

extern void isr_0x80();

struct idt_entry idt[256];
struct idt_ptr pIdt;

static void set_gate(int n, uint32_t nHandler) {
    idt[n].low = nHandler & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].zero = 0;
    idt[n].flags = 0x8E;
    idt[n].high = nHandler >> 16;
}

static const char* exception_messages[] = {
    "Division By Zero (#DE)",
    "Debug (#DB)",
    "Non Maskable Interrupt",
    "Breakpoint (#BP)",
    "Into Detected Overflow (#OF)",
    "Out of Bounds (#BR)",
    "Invalid Opcode (#UD)",
    "No Coprocessor (#NM)",
    "Double Fault (#DF)",
    "Coprocessor Segment Overrun",
    "Bad TSS (#TS)",
    "Segment Not Present (#NP)",
    "Stack Fault (#SS)",
    "General Protection Fault (#GP)",
    "Page Fault (#PF)",
    "Unknown Interrupt",
    "Coprocessor Fault (#MF)",
    "Alignment Check (#AC)",
    "Machine Check (#MC)",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

struct cpu_state {
    uint32_t edi, esi, ebp, esp_val, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} __attribute__((packed));

void exception_handler(struct cpu_state* r) {
    vga_print("\n\n========================================\n");
    vga_print("!!! KERNEL PANIC: CPU EXCEPTION !!!\n");
    vga_print("========================================\n");
    
    if (r->int_no < 32) {
        vga_print("Exception: ");
        vga_print(exception_messages[r->int_no]);
    } else {
        vga_print("Unknown exception vector");
    }
    
    vga_print("\nDetails:\n");
    vga_print("  Int No:   "); vga_print_dec(r->int_no); vga_print("\n");
    vga_print("  Err Code: "); vga_print_hex(r->err_code); vga_print("\n");
    vga_print("  EIP:      "); vga_print_hex(r->eip); vga_print("\n");
    vga_print("  CS:       "); vga_print_hex(r->cs); vga_print("\n");
    vga_print("  EFLAGS:   "); vga_print_hex(r->eflags); vga_print("\n");
    
    vga_print("\nRegisters:\n");
    vga_print("  EAX: "); vga_print_hex(r->eax); vga_print("  EBX: "); vga_print_hex(r->ebx); vga_print("\n");
    vga_print("  ECX: "); vga_print_hex(r->ecx); vga_print("  EDX: "); vga_print_hex(r->edx); vga_print("\n");
    vga_print("  ESI: "); vga_print_hex(r->esi); vga_print("  EDI: "); vga_print_hex(r->edi); vga_print("\n");
    vga_print("  EBP: "); vga_print_hex(r->ebp); vga_print("  ESP: "); vga_print_hex(r->esp_val); vga_print("\n");
    vga_print("========================================\n");
    vga_print("System halted.");

    while (1) {
        asm volatile("hlt");
    }
}

void idt_init() {
    pIdt.limit = sizeof(idt) - 1;
    pIdt.base = (uint32_t)&idt;

    set_gate(0, (uint32_t)isr_0);
    set_gate(1, (uint32_t)isr_1);
    set_gate(2, (uint32_t)isr_2);
    set_gate(3, (uint32_t)isr_3);
    set_gate(4, (uint32_t)isr_4);
    set_gate(5, (uint32_t)isr_5);
    set_gate(6, (uint32_t)isr_6);
    set_gate(7, (uint32_t)isr_7);
    set_gate(8, (uint32_t)isr_8);
    set_gate(9, (uint32_t)isr_9);
    set_gate(10, (uint32_t)isr_10);
    set_gate(11, (uint32_t)isr_11);
    set_gate(12, (uint32_t)isr_12);
    set_gate(13, (uint32_t)isr_13);
    set_gate(14, (uint32_t)isr_14);
    set_gate(15, (uint32_t)isr_15);
    set_gate(16, (uint32_t)isr_16);
    set_gate(17, (uint32_t)isr_17);
    set_gate(18, (uint32_t)isr_18);
    set_gate(19, (uint32_t)isr_19);
    set_gate(20, (uint32_t)isr_20);
    set_gate(21, (uint32_t)isr_21);
    set_gate(22, (uint32_t)isr_22);
    set_gate(23, (uint32_t)isr_23);
    set_gate(24, (uint32_t)isr_24);
    set_gate(25, (uint32_t)isr_25);
    set_gate(26, (uint32_t)isr_26);
    set_gate(27, (uint32_t)isr_27);
    set_gate(28, (uint32_t)isr_28);
    set_gate(29, (uint32_t)isr_29);
    set_gate(30, (uint32_t)isr_30);
    set_gate(31, (uint32_t)isr_31);

    set_gate(0x80, (uint32_t)isr_0x80);

    asm volatile("lidt %0" : : "m"(pIdt));
}