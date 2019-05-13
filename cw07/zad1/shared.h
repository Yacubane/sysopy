#pragma once

int sha_newsha(int key, int size);
int sha_opnsha(int key, int size);
void *sha_mapsha(int id, int size);
void sha_unmapsha(void *ptr, int size);
void sha_rmsha(int key, int id);

int sha_newsem(int key);
int sha_opnsem(int key);
void sha_clsem(int id);
void sha_rmsem(int key, int id);
int sha_lcksem(int id);
int sha_unlcksem(int id);
