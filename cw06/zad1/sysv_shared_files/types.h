#include "../shared/shared.h"

#define MTYPE_STOP 1
#define MTYPE_LIST 2
#define MTYPE_FRIENDS 3
#define MTYPE_ADD 4
#define MTYPE_DEL 5
#define MTYPE_ECHO 6
#define MTYPE_2ALL 7
#define MTYPE_2FRIENDS 8
#define MTYPE_2ONE 9
#define MTYPE_INIT 10
#define MTYPE_MAX 10

typedef struct msgbuf_t {
  long mtype;
  char mtext[MAX_STATEMENT_SIZE];
  int mid;
} msgbuf_t;

typedef struct cmsgbuf_t {
  long mtype;
  char mtext[MAX_STATEMENT_SIZE];
} cmsgbuf_t;

