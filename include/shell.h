#ifndef SHELL_H
#define SHELL_H

#include "types.h"

#define SHELL_INPUT_MAX  256
#define SHELL_HISTORY    16
#define SHELL_PROMPT     "majestic> "

void shell_init(void);
void shell_run(void);
void shell_process_command(const char *cmd);
void shell_print_prompt(void);

#endif /* SHELL_H */
