#include <stddef.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "errors.h"
#include "shared.h"

int sha_newsha(int key, int size) {
  int id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
  if (id == -1)
    return perr("Can't create shared memory", -1);

  return id;
}

int sha_opnsha(int key, int size) {
  int id = shmget(key, size, 0);
  if (id == -1)
    return perr("Can't open shared memory", -1);

  return id;
}

void* sha_mapsha(int id, int size) {
  void* ptr = shmat(id, NULL, 0);
  if (ptr == (void*)-1)
    err("Can't map shared memory", -1);

  return ptr;
}

void sha_unmapsha(void* ptr, int size) {
  if (shmdt(ptr) == -1)
    err("Can't unmap shared memory", -1);
}

void sha_rmsha(int key, int id) {
  if (shmctl(id, IPC_RMID, NULL) == -1)
    perr("Can't remove shared memory", -1);
}

int sha_newsem(int key) {
  int id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
  if (id == -1)
    return perr("Can't create semaphore", -1);

  if (semctl(id, 0, SETVAL, 1) == -1)
    return perr("Can't initialize semaphore", -1);

  return id;
}

int sha_opnsem(int key) {
  int id = semget(key, 1, 0);
  if (id == -1)
    return perr("Can't open semaphore", -1);

  return id;
}

void sha_clsem(int id) {
  return;
}

void sha_rmsem(int key, int id) {
  if (semctl(id, 0, IPC_RMID) == -1)
    err("Can't remove semaphore", -1);
}

int sha_lcksem(int id) {
  struct sembuf sembuf;
  sembuf.sem_op = -1;
  sembuf.sem_num = 0;
  sembuf.sem_flg = 0;

  if (semop(id, &sembuf, 1) == -1)
    return err("Can't lock semaphore", -1);
  return 0;
}

int sha_unlcksem(int id) {
  struct sembuf sembuf;
  sembuf.sem_op = 1;
  sembuf.sem_num = 0;
  sembuf.sem_flg = 0;

  if (semop(id, &sembuf, 1) == -1)
    return err("Can't lock semaphore", -1);
  return 0;
}