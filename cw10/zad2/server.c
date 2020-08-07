#include <assert.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define MSG_SIZE 60
#define MAX_USERS 16
#define MSG_NAME 1
#define MSG_START 2
#define MSG_MOVE 3
#define MSG_WAIT 4
#define MSG_WIN 5
#define MSG_LOST 6
#define MSG_TIE 7
#define MSG_PING 8
#define MSG_STOP 9

struct message {
  int type;
  int id;
  char msg[MSG_SIZE];
};

int MSIZE = sizeof(struct message);

int sendto_msg(int socket, const struct sockaddr* addr, socklen_t addr_len,
               int type, int id, const char* text) {
  struct message msg;
  msg.type = type;
  msg.id = id;
  strcpy(msg.msg, text);
  return sendto(socket, (void*)&msg, MSIZE, MSG_NOSIGNAL, addr, addr_len);
}
int send_msg(int socket, int type, int id, const char* text) {
  struct message msg;
  msg.type = type;
  msg.id = id;
  strcpy(msg.msg, text);
  return send(socket, (void*)&msg, MSIZE, MSG_NOSIGNAL);
}

struct user {
  char name[20];
  int socket;
  bool free;
  bool active;
  struct sockaddr* addr;
  socklen_t addr_size;
};

struct users {
  struct user users[MAX_USERS];
  int size;
};

bool is_name_free(struct users* users, const char* name) {
  for (int i = 0; i < users->size; i++) {
    if (strcmp(users->users[i].name, name) == 0) return false;
  }
  return true;
}

int add_user(struct users* users, const char* name, int socket,
             struct sockaddr* addr, socklen_t addr_size) {
  if (users->size >= MAX_USERS) return -1;
  struct user* user = &(users->users[users->size]);
  strcpy(user->name, name);
  user->socket = socket;
  user->free = true;
  user->active = true;
  user->addr = calloc(1, sizeof(struct sockaddr));
  user->addr->sa_family = addr->sa_family;
  memcpy(user->addr->sa_data, addr->sa_data, addr_size);
  user->addr_size = addr_size;
  users->size++;
  return users->size - 1;
}

int get_free_user(struct users* users, int skip_id) {
  for (int i = 0; i < users->size; i++) {
    if (i == skip_id) continue;
    if (users->users[i].socket != -1 && users->users[i].free == true) return i;
  }
  return -1;
}

struct game {
  int user1;
  int user2;
};

struct games {
  struct game games[MAX_USERS / 2];
  int size;
};

int add_game(struct games* games, int user1, int user2) {
  if (games->size >= MAX_USERS / 2) return -1;
  struct game* game = &(games->games[games->size]);
  game->user1 = user1;
  game->user2 = user2;
  games->size++;
  return games->size - 1;
}

int get_game_with_user(struct games* games, int user_id) {
  for (int i = 0; i < games->size; i++) {
    if (games->games[i].user1 == user_id || games->games[i].user2 == user_id)
      return i;
  }
  return -1;
}

int get_second_user(struct games* games, int game_id, int user_id) {
  if (games->games[game_id].user1 == user_id)
    return games->games[game_id].user2;
  else if (games->games[game_id].user2 == user_id)
    return games->games[game_id].user1;
  return -1;
}

int server_inet_socket = -1;
int server_unix_socket = -1;
char* unix_socket_path;
int epoll_fd;

pthread_t connection_tid, ping_tid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct users users = {.size = 0};
struct games games = {.size = 0};

void start_server(int port, const char* unix_socket_path) {
  // local
  struct sockaddr_un unix_address;
  unix_address.sun_family = AF_UNIX;
  strcpy(unix_address.sun_path, unix_socket_path);
  unlink(unix_socket_path);

  server_unix_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
  bind(server_unix_socket, (struct sockaddr*)&unix_address,
       sizeof(unix_address));
  printf("UNIX socket started on %s\n", unix_socket_path);

  struct sockaddr_in net_address;
  net_address.sin_family = AF_INET;
  net_address.sin_port = htons(port);
  net_address.sin_addr.s_addr = htonl(INADDR_ANY);

  server_inet_socket = socket(AF_INET, SOCK_DGRAM, 0);
  bind(server_inet_socket, (struct sockaddr*)&net_address, sizeof(net_address));
  printf("INET socket started on port %d\n", port);
  printf("\n");
}

void stop_server() {
  close(epoll_fd);
  pthread_cancel(connection_tid);
  pthread_cancel(ping_tid);

  close(server_unix_socket);
  unlink(unix_socket_path);

  close(server_inet_socket);
}

void stop_game(int socket, int user_id) {
  char* username = users.users[user_id].name;
  printf("Disconnect %s\n", username);
  users.users[user_id].socket = -1;

  int game_id = get_game_with_user(&games, user_id);
  if (game_id != -1) {
    int user2 = get_second_user(&games, game_id, user_id);
    int socket2 = users.users[user2].socket;
    struct sockaddr* addr2 = users.users[user2].addr;
    socklen_t addr_len2 = users.users[user2].addr_size;

    char* username2 = users.users[user2].name;
    printf("Disconnect %s\n", username2);
    users.users[user2].socket = -1;

    sendto_msg(socket2, addr2, addr_len2, MSG_STOP, 0, "");
  }
}

void get_message(int socket) {
  struct message msg;

  struct sockaddr addr;
  socklen_t addr_len;

  int recv_size = recvfrom(socket, (void*)&msg, MSIZE, 0, &addr, &addr_len);
  if (recv_size == 0) {
    printf("recvfrom size 0\n");
  }

  if (msg.type == MSG_NAME) {
    bool is_free = is_name_free(&users, msg.msg);
    if (!is_free) {
      sendto_msg(socket, &addr, addr_len, MSG_NAME, 0, "This name is occupied");
      return;
    }

    int user_id = add_user(&users, msg.msg, socket, &addr, addr_len);
    if (user_id == -1) {
      sendto_msg(socket, &addr, addr_len, MSG_NAME, 0,
                 "No free slot on server");
      return;
    }
    printf("New user, name: %s\n", msg.msg);
    int free_id = get_free_user(&users, user_id);
    if (free_id == -1) {
      sendto_msg(socket, &addr, addr_len, MSG_NAME, user_id,
                 "Wait for opponent");
    } else {
      add_game(&games, user_id, free_id);
      users.users[user_id].free = false;
      users.users[free_id].free = false;

      int socket2 = users.users[free_id].socket;

      struct sockaddr* addr2 = users.users[free_id].addr;
      socklen_t addr_len2 = users.users[free_id].addr_size;

      char sign1, sign2;
      if (rand() % 2 == 0) {
        sign1 = 'X';
        sign2 = 'O';
      } else {
        sign1 = 'O';
        sign2 = 'X';
      }

      char msg1[MSG_SIZE], msg2[MSG_SIZE];
      sprintf(msg1, "You play with %s. Game starts. You are %c",
              users.users[free_id].name, sign1);
      sprintf(msg2, "You play with %s. Game starts. You are %c",
              users.users[user_id].name, sign2);

      sendto_msg(socket, &addr, addr_len, MSG_START, user_id, msg1);
      sendto_msg(socket2, addr2, addr_len2, MSG_START, free_id, msg2);

      sendto_msg(socket, &addr, addr_len, MSG_MOVE, user_id, "");
      sendto_msg(socket2, addr2, addr_len2, MSG_WAIT, free_id, "");
    }
  } else if (msg.type == MSG_MOVE) {
    int pos = atoi(msg.msg);
    char* username = users.users[msg.id].name;
    printf("%s chose pos: %d\n", username, pos);

    int game_id = get_game_with_user(&games, msg.id);
    int user2 = get_second_user(&games, game_id, msg.id);
    int socket2 = users.users[user2].socket;
    struct sockaddr* addr2 = users.users[user2].addr;
    socklen_t addr_len2 = users.users[user2].addr_size;

    sendto_msg(socket, &addr, addr_len, MSG_WAIT, 0, msg.msg);
    sendto_msg(socket2, addr2, addr_len2, MSG_MOVE, 0, msg.msg);
  } else if (msg.type == MSG_WIN) {
    int game_id = get_game_with_user(&games, msg.id);
    int user2 = get_second_user(&games, game_id, msg.id);
    int socket2 = users.users[user2].socket;
    struct sockaddr* addr2 = users.users[user2].addr;
    socklen_t addr_len2 = users.users[user2].addr_size;

    sendto_msg(socket2, addr2, addr_len2, MSG_LOST, 0, msg.msg);
  } else if (msg.type == MSG_LOST) {
    int game_id = get_game_with_user(&games, msg.id);
    int user2 = get_second_user(&games, game_id, msg.id);
    int socket2 = users.users[user2].socket;
    struct sockaddr* addr2 = users.users[user2].addr;
    socklen_t addr_len2 = users.users[user2].addr_size;

    sendto_msg(socket2, addr2, addr_len2, MSG_WIN, 0, msg.msg);
  } else if (msg.type == MSG_TIE) {
    int game_id = get_game_with_user(&games, msg.id);
    int user2 = get_second_user(&games, game_id, msg.id);
    int socket2 = users.users[user2].socket;
    struct sockaddr* addr2 = users.users[user2].addr;
    socklen_t addr_len2 = users.users[user2].addr_size;

    sendto_msg(socket2, addr2, addr_len2, MSG_TIE, 0, msg.msg);
  } else if (msg.type == MSG_PING) {
    printf("Ping from %s\n", users.users[msg.id].name);
    users.users[msg.id].active = true;
  } else if (msg.type == MSG_STOP) {
    printf("Game stopped\n");
    stop_game(socket, msg.id);
  }
}

void* thread_connection(void* val) {
  epoll_fd = epoll_create1(0);
  struct epoll_event event_unix;
  event_unix.events = EPOLLIN;
  event_unix.data.fd = server_unix_socket;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_unix_socket, &event_unix);

  struct epoll_event event_inet;
  event_inet.events = EPOLLIN;
  event_inet.data.fd = server_inet_socket;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_inet_socket, &event_inet);

  struct epoll_event events[MAX_USERS];

  while (1) {
    int occ_events = epoll_wait(epoll_fd, events, MAX_USERS, -1);
    for (int i = 0; i < occ_events; i++) {
      pthread_mutex_lock(&mutex);
      get_message(events[i].data.fd);
      pthread_mutex_unlock(&mutex);
    }
  }

  return NULL;
}
void* thread_ping(void* val) {
  while (1) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < users.size; i++) {
      if (users.users[i].socket != -1 && !users.users[i].active) {
        printf("Ping not received\n");
        stop_game(users.users[i].socket, i);
        continue;
      }

      sendto_msg(users.users[i].socket, users.users[i].addr,
                 users.users[i].addr_size, MSG_PING, 0, "");
      users.users[i].active = false;
    }
    pthread_mutex_unlock(&mutex);

    sleep(15);
  }

  return NULL;
}

void exit_handler() {
  printf("Server is shutting down\n");
  stop_server();
}
void sigint_handler(int sign) {
  printf("\nSIGINT received\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Too few arguments\n");
    return -1;
  }

  int port = atoi(argv[1]);
  if (port < 1024) {
    printf("Port must be >= 1024\n");
    return -1;
  }
  unix_socket_path = argv[2];

  atexit(exit_handler);
  if (signal(SIGINT, sigint_handler) == SIG_ERR) {
    printf("ERROR: signal\n");
    return -1;
  }

  start_server(port, unix_socket_path);

  pthread_create(&connection_tid, NULL, thread_connection, NULL);
  pthread_create(&ping_tid, NULL, thread_ping, NULL);

  pthread_join(connection_tid, NULL);
  pthread_join(ping_tid, NULL);

  return 0;
}