#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils/http_parser.h"
#include "utils/http_builder.h"
#include "utils/debug_print.h"

#define RETSIGTYPE void
#define LISTEN_DOMAIN "localhost"
#define LISTEN_HOST 10000
