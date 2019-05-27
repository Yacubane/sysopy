#pragma once

void* que_create(int item_size, int max_size);
int que_clear(void* queue, int item_size, int max_size);
int que_put(void* queue, void* item_ptr);
int que_get(void* queue, void* item_ptr);
int que_peek(void* queue, int elem_num, void* item_ptr);
int que_size(void* queue_ptr);
