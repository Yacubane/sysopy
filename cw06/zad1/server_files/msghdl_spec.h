int init_msghdl_spec();
int handle_init(char* message);

int handle_echo(int client_id, char* message);
int handle_list(int client_id);
int handle_friends(int client_id,
                   int* clients_id,
                   int friends_size,
                   int array_size);
int handle_add(int client_id, int to_add_client_id);
int handle_del(int client_id, int to_del_client_id);
int handle_to_all(int client_id, char* message);
int handle_to_friends(int client_id, char* message);
int handle_to_one(int from_client_id, int to_client_id, char* message);
int handle_stop(int client_id);

int shutdown();
int get_clients_num();
