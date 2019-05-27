#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../zad1/errors.h"
#include "../zad1/shared.h"

#define MAX_SEMAPHORS 10

sem_t* sem_array[MAX_SEMAPHORS];
int sem_index;
void init() {
  sem_index = 0;
}

int sha_newsha(int key, int size) {
  char path[64];
  sprintf(path, "/%d", key);

  int id = shm_open(path, O_RDWR | O_CREAT | O_EXCL, 0666);
  if (id == -1)
    return perr("Can't create shared memory", -1);

  int res = ftruncate(id, size);
  if (res == -1)
    return perr("Can't resize shared memory", size);

  return id;
}

int sha_opnsha(int key, int size) {
  char path[64];
  sprintf(path, "/%d", key);

  int id = shm_open(path, O_RDWR, 0);
  if (id == -1)
    return perr("Can't open shared memory", -1);

  return id;
}

void* sha_mapsha(int id, int size) {
  void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0);
  if (ptr == (void*)-1) {
    err("Can't map shared memory", -1);
    return NULL;
  }
  return ptr;
}

void sha_unmapsha(void* ptr, int size) {
  if (munmap(ptr, size) == -1)
    err("unable to unmap shared memory", -1);
}

void sha_rmsha(int key, int id) {
  char path[64];
  sprintf(path, "/%d", key);

  if (shm_unlink(path) == -1)
    err("Can't remove shared memory", -1);
}

int sha_newsem(int key) {
  if (sem_index == MAX_SEMAPHORS)
    return err("Can't make more semaphors", -1);
  char path[64];
  sprintf(path, "/%d", key);

  sem_t* id = sem_open(path, O_RDWR | O_CREAT | O_EXCL, 0666);
  sem_array[sem_index++] = id;
  if (id == SEM_FAILED)
    return perr("Can't create semaphore", -1);

  sha_unlcksem(sem_index - 1);
  return sem_index - 1;
}

int sha_opnsem(int key) {
  char path[64];
  sprintf(path, "/%d", key);

  sem_t* id = sem_open(path, O_RDWR, 0666);
  sem_array[sem_index++] = id;
  if (id == SEM_FAILED)
    return perr("Can't open semaphore", -1);

  return sem_index - 1;
}

void sha_clsem(int id) {
  if (sem_close(sem_array[id]) == -1)
    perr("Can't close semaphore", -1);
}

void sha_rmsem(int key, int id) {
  sha_clsem(id);
  char path[64];
  sprintf(path, "/%d", key);

  if (sem_unlink(path) == -1)
    perr("Can't remove semaphore", -1);
}

int sha_lcksem(int id) {
  if (sem_wait(sem_array[id]) == -1)
    return perr("Can't lock semaphore", -1);
  return 0;
}

int sha_unlcksem(int id) {
  if (sem_post(sem_array[id]) == -1)
    return perr("Can't unlock semaphore", -1);
  return 0;
}