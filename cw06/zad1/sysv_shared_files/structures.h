#pragma once
#include "../shared_files/config.h"

typedef struct msgbuf_t {
  long mtype;
  char mtext[MAX_STATEMENT_SIZE];
  int mid;
} msgbuf_t;

typedef struct cmsgbuf_t {
  long mtype;
  char mtext[MAX_STATEMENT_SIZE];
} cmsgbuf_t;
