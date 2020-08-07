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
  char msg[MSG_SIZE];
};

int MSIZE = sizeof(struct message);

struct user {
  char name[20];
  int socket;
  bool free;
  bool active;
};

struct users {
  struct user users[MAX_USERS];
  int size;
};

struct game {
  int user1;
  int user2;
};

struct games {
  struct game games[MAX_USERS / 2];
  int size;
};

int send_msg(int socket, int type, const char* text) {
  struct message msg;
  msg.type = type;
  strcpy(msg.msg, text);
  return send(socket, (void*)&msg, MSIZE, MSG_NOSIGNAL);
}

bool is_name_free(struct users* users, const char* name) {
  for (int i = 0; i < users->size; i++) {
    if (strcmp(users->users[i].name, name) == 0) return false;
  }
  return true;
}

int add_user(struct users* users, const char* name, int socket) {
  if (users->size >= MAX_USERS) return -1;
  struct user* user = &(users->users[users->size]);
  strcpy(user->name, name);
  user->socket = socket;
  user->free = true;
  user->active = true;
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

int get_user_with_socket(struct users* users, int socket) {
  for (int i = 0; i < users->size; i++) {
    if (users->users[i].socket == socket) return i;
  }
  return -1;
}

char* get_username_with_socket(struct users* users, int socket) {
  int id = get_user_with_socket(users, socket);
  if (id != -1)
    return users->users[id].name;
  else
    return NULL;
}

int add_game(struct games* games, int user1, int user2) {
  if (games->size >= MAX_USERS / 2) return -1;
  struct game* game = &(games->games[games->size]);
  game->user1 = user1;
  game->user2 = user2;
  games->size++;
  return games->size - 1;
}

int get_game_with_user_socket(struct games* games, struct users* users,
                              int socket) {
  for (int i = 0; i < games->size; i++) {
    if (users->users[games->games[i].user1].socket == socket ||
        users->users[games->games[i].user2].socket == socket)
      return i;
  }
  return -1;
}
int get_second_socket(struct games* games, struct users* users, int socket) {
  for (int i = 0; i < games->size; i++) {
    if (users->users[games->games[i].user1].socket == socket)
      return users->users[games->games[i].user2].socket;
    else if (users->users[games->games[i].user2].socket == socket)
      return users->users[games->games[i].user1].socket;
  }
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
  struct sockaddr_un unix_address;
  unix_address.sun_family = AF_UNIX;
  strcpy(unix_address.sun_path, unix_socket_path);
  unlink(unix_socket_path);

  server_unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  bind(server_unix_socket, (struct sockaddr*)&unix_address,
       sizeof(unix_address));
  listen(server_unix_socket, MAX_USERS);
  printf("UNIX socket started on %s\n", unix_socket_path);

  struct sockaddr_in net_address;
  net_address.sin_family = AF_INET;
  net_address.sin_port = htons(port);
  net_address.sin_addr.s_addr = htonl(INADDR_ANY);

  server_inet_socket = socket(AF_INET, SOCK_STREAM, 0);
  bind(server_inet_socket, (struct sockaddr*)&net_address, sizeof(net_address));
  listen(server_inet_socket, MAX_USERS);
  printf("INET socket started on port %d\n", port);
  printf("\n");
}

void stop_server() {
  close(epoll_fd);
  pthread_cancel(connection_tid);
  pthread_cancel(ping_tid);

  shutdown(server_unix_socket, SHUT_RDWR);
  close(server_unix_socket);
  unlink(unix_socket_path);

  shutdown(server_inet_socket, SHUT_RDWR);
  close(server_inet_socket);
}

void close_connection(int socket) {
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket, NULL);
  shutdown(socket, SHUT_RDWR);
  close(socket);
  int id = get_user_with_socket(&users, socket);
  if (id != -1) users.users[id].socket = -1;
}

void stop_game(int socket) {
  int socket2 = get_second_socket(&games, &users, socket);
  send_msg(socket, MSG_STOP, "");
  if (socket2 != -1) send_msg(socket2, MSG_STOP, "");
  char* username = get_username_with_socket(&users, socket);
  printf("Disconnect %s\n", username);
  if (socket2 != -1)
    printf("Disconnect %s\n", get_username_with_socket(&users, socket2));
  close_connection(socket);
  if (socket2 != -1) close_connection(socket2);
}

void get_message(int socket) {
  struct message msg;

  int recv_size = recv(socket, (void*)&msg, MSIZE, 0);
  if (recv_size == 0) {
    close_connection(socket);
    return;
  }

  if (msg.type == MSG_NAME) {
    bool is_free = is_name_free(&users, msg.msg);
    if (!is_free) {
      send_msg(socket, MSG_NAME, "This name is occupied");
      close_connection(socket);
      return;
    }
    int user_id = add_user(&users, msg.msg, socket);
    if (user_id == -1) {
      send_msg(socket, MSG_NAME, "No free slot on server");
      close_connection(socket);
      return;
    }
    printf("New user, name: %s\n", msg.msg);
    int free_id = get_free_user(&users, user_id);
    if (free_id == -1) {
      send_msg(socket, MSG_NAME, "Wait for opponent");
    } else {
      add_game(&games, user_id, free_id);
      users.users[user_id].free = false;
      users.users[free_id].free = false;

      int socket2 = users.users[free_id].socket;

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

      send_msg(socket, MSG_START, msg1);
      send_msg(socket2, MSG_START, msg2);

      send_msg(socket, MSG_MOVE, "");
      send_msg(socket2, MSG_WAIT, "");
    }
  } else if (msg.type == MSG_MOVE) {
    int pos = atoi(msg.msg);
    char* username = get_username_with_socket(&users, socket);
    printf("%s chose pos: %d\n", username, pos);

    int socket2 = get_second_socket(&games, &users, socket);
    send_msg(socket, MSG_WAIT, msg.msg);
    send_msg(socket2, MSG_MOVE, msg.msg);
  } else if (msg.type == MSG_WIN) {
    int socket2 = get_second_socket(&games, &users, socket);
    send_msg(socket2, MSG_LOST, msg.msg);
  } else if (msg.type == MSG_LOST) {
    int socket2 = get_second_socket(&games, &users, socket);
    send_msg(socket2, MSG_WIN, msg.msg);
  } else if (msg.type == MSG_TIE) {
    int socket2 = get_second_socket(&games, &users, socket);
    send_msg(socket2, MSG_TIE, msg.msg);
  } else if (msg.type == MSG_PING) {
    int user_id = get_user_with_socket(&users, socket);
    printf("Ping from %s\n", users.users[user_id].name);
    users.users[user_id].active = true;
  } else if (msg.type == MSG_STOP) {
    printf("Game stopped\n");
    stop_game(socket);
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
      int socket = events[i].data.fd;
      if (socket == server_unix_socket || socket == server_inet_socket) {
        int client_socket = accept(socket, NULL, NULL);

        struct epoll_event client_event;
        client_event.events = EPOLLIN | EPOLLRDHUP;
        client_event.data.fd = client_socket;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &client_event);
      } else {
        pthread_mutex_lock(&mutex);
        if (events[i].events != EPOLLIN) {
          printf("Event stop\n");
          stop_game(socket);
          continue;
        }
        get_message(socket);
        pthread_mutex_unlock(&mutex);
      }
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
        stop_game(users.users[i].socket);
        continue;
      }
      send_msg(users.users[i].socket, MSG_PING, "");
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
