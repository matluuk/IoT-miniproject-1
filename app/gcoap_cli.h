#ifndef GCOAP_CLI_H
#define GCOAP_CLI_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net/gcoap.h"
#include "od.h"

#include "debug.h"

int gcoap_access(char method[], char *data, char *resource);

#endif