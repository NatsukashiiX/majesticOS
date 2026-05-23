/* =============================================================================
 * MajesticOS - Retro Shell
 * A command-line interface with classic retro-style commands
 * ============================================================================= */

#include "../include/shell.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/kprintf.h"
#include "../include/string.h"
#include "../include/types.h"
#include "../include/ports.h"

/* Command history */
static char history[SHELL_HISTORY][SHELL_INPUT_MAX];
static int  history_count = 0;
static int  history_index = 0;

/* ============================================================
 * ASCII Art Banner
 * ============================================================ */
static void print_banner(void) {
    vga_clear();
    vga_puts_color("", VGA_COLOR_CYAN, VGA_COLOR_BLACK);

    vga_puts_color(
        "  __  __    _    _   _____ ____ _____ ___ ____    ___  ____  \n",
        VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    vga_puts_color(
        " |  \\/  |  / \\  | | | ____/ ___|_   _|_ _/ ___|  / _ \\/ ___| \n",
        VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    vga_puts_color(
        " | |\\/| | / _ \\ | | |  _| \\___ \\ | |  | | |     | | | \\___ \\ \n",
        VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts_color(
        " | |  | |/ ___ \\| |_| |___ ___) || |  | | |___  | |_| |___) |\n",
        VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_puts_color(
        " |_|  |_/_/   \\_\\____|_____|____/ |_| |___\\____|  \\___/|____/ \n",
        VGA_COLOR_BLUE, VGA_COLOR_BLACK);
    vga_puts_color(
        "\n",
        VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    vga_puts_color(
        "  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+\n",
        VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_puts_color(
        "   MajesticOS v1.0   [ A Retro-Style Operating System ]       \n",
        VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts_color(
        "  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+\n",
        VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);

    vga_puts_color(
        "\n  Type 'help' for a list of commands.\n\n",
        VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

/* ============================================================
 * Built-in Commands
 * ============================================================ */

static void cmd_help(void) {
    vga_puts_color("\n+----------------------------------------------------------+\n", VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    vga_puts_color(" MajesticOS Command Reference\n", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts_color("+----------------------------------------------------------+\n", VGA_COLOR_CYAN, VGA_COLOR_BLACK);

    struct { const char *cmd; const char *desc; } cmds[] = {
        { "help",      "Show this help menu" },
        { "clear",     "Clear the screen" },
        { "about",     "About MajesticOS" },
        { "version",   "Show OS version info" },
        { "echo [msg]","Print a message" },
        { "color",     "Show color palette demo" },
        { "banner",    "Redisplay the banner" },
        { "reboot",    "Reboot the system" },
        { "halt",      "Halt the system" },
        { "history",   "Show command history" },
        { "uname",     "System information" },
        { "art",       "Display ASCII art gallery" },
        { "calc [expr]","Simple calculator (+,-,*)" },
        { NULL, NULL }
    };

    for (int i = 0; cmds[i].cmd; i++) {
        vga_puts_color("  ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_puts_color(cmds[i].cmd, VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);

        /* Padding */
        int pad = 16 - (int)strlen(cmds[i].cmd);
        for (int j = 0; j < pad; j++) vga_putchar(' ');

        vga_puts_color(cmds[i].desc, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_putchar('\n');
    }
    vga_putchar('\n');
}

static void cmd_about(void) {
    vga_puts_color("\n  MajesticOS\n", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts_color("  =========\n", VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_puts_color("  A retro-style hobby operating system written in C.\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts_color("  Boots from a custom x86 bootloader into a 32-bit\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts_color("  protected mode kernel with interrupt handling,\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts_color("  VGA text output, PS/2 keyboard driver, and this\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts_color("  interactive shell.\n\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts_color("  Built with love for the old-school hacker spirit.\n\n", VGA_COLOR_CYAN, VGA_COLOR_BLACK);
}

static void cmd_version(void) {
    vga_puts_color("\n  MajesticOS v1.0.0\n", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    kprintf("  Kernel: majestic-kernel 1.0\n");
    kprintf("  Arch:   x86 (32-bit Protected Mode)\n");
    kprintf("  VGA:    80x25 Text Mode\n");
    kprintf("  Build:  " __DATE__ " " __TIME__ "\n\n");
}

static void cmd_color(void) {
    vga_puts_color("\n  Color Palette:\n\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    const char *names[] = {
        "BLACK       ", "BLUE        ", "GREEN       ", "CYAN        ",
        "RED         ", "MAGENTA     ", "BROWN       ", "LIGHT_GREY  ",
        "DARK_GREY   ", "LIGHT_BLUE  ", "LIGHT_GREEN ", "LIGHT_CYAN  ",
        "LIGHT_RED   ", "LT_MAGENTA  ", "YELLOW      ", "WHITE       "
    };
    for (int i = 0; i < 16; i++) {
        vga_puts_color("  ", (vga_color_t)i, VGA_COLOR_BLACK);
        vga_puts_color("  ###  ", (vga_color_t)i, VGA_COLOR_BLACK);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        kprintf(" %d: %s", i, names[i]);
        if (i % 2 == 1) vga_putchar('\n');
    }
    vga_putchar('\n');
}

static void cmd_uname(void) {
    vga_puts_color("\nMajesticOS ", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts_color("majestic-kernel 1.0 ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts_color("x86 i386 MajesticOS\n\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

static void cmd_art(void) {
    vga_puts_color("\n", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts_color(
        "       __|__\n"
        "  --@--@--o--@--@--\n"
        "       |\n"
        "   MajesticOS\n"
        "   Retro Computer\n\n",
        VGA_COLOR_CYAN, VGA_COLOR_BLACK);

    vga_puts_color(
        "   .---.    .---.\n"
        "  /  _  \\  /  _  \\\n"
        " | (___) || (___) |\n"
        "  \\_____/  \\_____/\n"
        "   [  *  ]  [  *  ]\n"
        "   MAJESTIC OS\n\n",
        VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
}

static void cmd_echo(const char *arg) {
    if (!arg || !*arg) {
        vga_putchar('\n');
        return;
    }
    vga_puts_color(arg, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_putchar('\n');
}

static void cmd_history(void) {
    if (history_count == 0) {
        vga_puts_color("  No history yet.\n", VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
        return;
    }
    for (int i = 0; i < history_count; i++) {
        kprintf("  %d  %s\n", i + 1, history[i]);
    }
}

/* Tiny integer expression evaluator: handles a OP b */
static void cmd_calc(const char *expr) {
    if (!expr || !*expr) {
        vga_puts_color("  Usage: calc <num> <op> <num>  e.g. calc 7 + 3\n\n",
                       VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
        return;
    }

    /* Skip leading spaces */
    while (*expr == ' ') expr++;

    int a = atoi(expr);
    while (*expr && *expr != ' ' && *expr != '+' && *expr != '-' &&
           *expr != '*' && *expr != '/') expr++;
    while (*expr == ' ') expr++;
    char op = *expr;
    if (op) expr++;
    while (*expr == ' ') expr++;
    int b = atoi(expr);

    int result = 0;
    int valid  = 1;
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0) {
                vga_puts_color("  Error: Division by zero!\n\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
                return;
            }
            result = a / b;
            break;
        default: valid = 0; break;
    }

    if (!valid) {
        vga_puts_color("  Error: Unknown operator. Use + - * /\n\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }

    kprintf("  %d %c %d = ", a, op, b);
    vga_puts_color("", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    char buf[32];
    itoa(result, buf, 10);
    vga_puts_color(buf, VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts_color("\n\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

static void cmd_reboot(void) {
    vga_puts_color("\n  Rebooting MajesticOS...\n", VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    /* Pulse keyboard controller reset line */
    uint8_t good = 0x02;
    while (good & 0x02)
        good = port_byte_in(0x64);
    port_byte_out(0x64, 0xFE);
    for (;;) __asm__ volatile("hlt");
}

static void cmd_halt(void) {
    vga_clear();
    vga_puts_color("\n\n\n\n", VGA_COLOR_BLACK, VGA_COLOR_BLACK);
    vga_puts_color(
        "   +=============================================+\n"
        "   |                                             |\n"
        "   |      MajesticOS has been halted.            |\n"
        "   |      It is now safe to power off.           |\n"
        "   |                                             |\n"
        "   +=============================================+\n",
        VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    for (;;) __asm__ volatile("hlt");
}

/* ============================================================
 * Command Dispatcher
 * ============================================================ */
void shell_process_command(const char *raw) {
    char cmd_buf[SHELL_INPUT_MAX];
    strncpy(cmd_buf, raw, SHELL_INPUT_MAX - 1);
    cmd_buf[SHELL_INPUT_MAX - 1] = '\0';

    char *cmd = strtrim(cmd_buf);
    if (!cmd || !*cmd) return;

    /* Save to history */
    if (history_count < SHELL_HISTORY) {
        strcpy(history[history_count++], cmd);
    } else {
        /* Shift history */
        for (int i = 0; i < SHELL_HISTORY - 1; i++)
            strcpy(history[i], history[i + 1]);
        strcpy(history[SHELL_HISTORY - 1], cmd);
    }
    history_index = history_count;

    /* Split into command + argument */
    char *arg = cmd;
    while (*arg && *arg != ' ') arg++;
    if (*arg == ' ') { *arg = '\0'; arg++; }
    else arg = NULL;

    /* Dispatch */
    if (strcmp(cmd, "help") == 0)         cmd_help();
    else if (strcmp(cmd, "clear") == 0)   vga_clear();
    else if (strcmp(cmd, "banner") == 0)  print_banner();
    else if (strcmp(cmd, "about") == 0)   cmd_about();
    else if (strcmp(cmd, "version") == 0) cmd_version();
    else if (strcmp(cmd, "color") == 0)   cmd_color();
    else if (strcmp(cmd, "uname") == 0)   cmd_uname();
    else if (strcmp(cmd, "art") == 0)     cmd_art();
    else if (strcmp(cmd, "history") == 0) cmd_history();
    else if (strcmp(cmd, "echo") == 0)    cmd_echo(arg);
    else if (strcmp(cmd, "calc") == 0)    cmd_calc(arg);
    else if (strcmp(cmd, "reboot") == 0)  cmd_reboot();
    else if (strcmp(cmd, "halt") == 0)    cmd_halt();
    else {
        vga_puts_color("  Unknown command: ", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_puts_color(cmd, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_puts_color("  (type 'help' for commands)\n", VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    }
}

void shell_print_prompt(void) {
    vga_puts_color("\nmajestic", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts_color("> ",        VGA_COLOR_YELLOW,       VGA_COLOR_BLACK);
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void shell_init(void) {
    print_banner();
}

/* ============================================================
 * Shell Main Loop - reads input char by char
 * ============================================================ */
void shell_run(void) {
    char input[SHELL_INPUT_MAX];
    int  pos = 0;

    shell_print_prompt();

    while (1) {
        char c = keyboard_getchar();

        if (c == '\n') {
            vga_putchar('\n');
            input[pos] = '\0';
            shell_process_command(input);
            pos = 0;
            shell_print_prompt();

        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                vga_putchar('\b');
            }

        } else if (c >= ' ' && pos < SHELL_INPUT_MAX - 1) {
            input[pos++] = c;
            vga_putchar(c);
        }
    }
}
