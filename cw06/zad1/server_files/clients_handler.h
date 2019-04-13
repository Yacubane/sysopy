#include "../shared_files/errors.h"
#include "../shared_files/utils.h"
#include "../shared_files/config.h"

int init_clients_handler();
int get_client_arr_id(int id);
int send_message(int array_id, char *message);
int init_client(char *message);

int get_client_array_size();
int is_client_friend(int array_id);
int set_client_friend(int array_id, int is_friend);
int get_client_id(int array_id);
int is_client(int array_id);
int free_client(int array_id);