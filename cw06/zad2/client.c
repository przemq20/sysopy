#include "structures.h"

int turn;
int client_id;
int pair_id;
int pair_queue_id;
int in_chat = 0;
int queue_id;
int server_queue_id;
int server_pid;
char path[100];
int running = 1;

void close_client() {
  running = 0;
  if (queue_id != -1) {
    message* msg = (message*)calloc(1, sizeof(message));
    msg->mtype = STOP;
    msg->client_id = client_id;

    if (mq_send(server_queue_id, (char*)msg, MSGSZ, 5) == -1) {
      ERROR("Error while sending message to the server!\n");
    }

    kill(server_pid, SIGUSR1);

    usleep(250000);

    if (mq_close(server_queue_id) == -1)
      printf("\nError while closing server queue!\n");
    else {
      printf("\nServer queue closed successfully \n");
    }

    if (mq_close(queue_id) == -1 || mq_unlink(path) == -1)
      printf("Error while closing queue!\n");
    else {
      printf("Queue deleted successfully and client is about to close\n");
    }
  }
}

void process_list() {
  message* msg = (message*)calloc(1, sizeof(message));
  msg->mtype = LIST;
  msg->client_id = client_id;

  if (mq_send(server_queue_id, (char*)msg, MSGSZ, 3) == -1) {
    ERROR("Error while sending message to the server!\n");
  }

  sleep(1);

  message* msg2 = (message*)calloc(1, sizeof(message));
  mq_receive(queue_id, (char*)msg2, MSGSZ, NULL);

  printf("%s", msg2->message);

  free(msg);
  free(msg2);
}

void process_connect() {
  printf("What's the id of the client you want to connect?\n");
  int user2_id;
  scanf("%d", &user2_id);

  message* msg = (message*)calloc(1, sizeof(message));
  msg->mtype = CONNECT;
  msg->client_id = client_id;
  msg->pair_id = user2_id;
  msg->client_pid = getpid();

  if (mq_send(server_queue_id, (char*)msg, MSGSZ, 2) == -1) {
    ERROR("Error while sending message to the server!\n");
  }

  sleep(1);

  message* msg2 = (message*)calloc(1, sizeof(message));
  mq_receive(queue_id, (char*)msg2, MSGSZ, NULL);

  printf("%s", msg2->message);

  pair_id = user2_id;
  char pth[20];
  sprintf(pth, "/%d", msg2->client_pid);
  pair_queue_id = mq_open(pth, O_WRONLY);
  in_chat = 1;
  turn = 1;

  free(msg);
  free(msg2);
}

void process_disconnect() {
  message* msg = (message*)calloc(1, sizeof(message));
  msg->mtype = DISCONNECT;
  msg->client_id = client_id;
  msg->pair_id = pair_id;

  if (mq_send(server_queue_id, (char*)msg, MSGSZ, 4) == -1) {
    ERROR("Error while sending message to the server!\n");
  }

  sleep(1);

  message* msg2 = (message*)calloc(1, sizeof(message));
  mq_receive(queue_id, (char*)msg2, MSGSZ, NULL);

  printf("\n%s", msg2->message);

  free(msg);
  free(msg2);
  in_chat = 0;
}

void process_stop() {
  message* msg = (message*)calloc(1, sizeof(message));
  msg->mtype = STOP;
  msg->client_id = client_id;
  if (mq_send(server_queue_id, (char*)msg, MSGSZ, 5) == -1) {
    ERROR("Error while sending message to the server!\n");
  }

  message* msg2 = (message*)calloc(1, sizeof(message));
  while (1) {
    mq_receive(queue_id, (char*)msg2, MSGSZ, NULL);
    if (msg2->mtype == STOP)
      break;
  }
  printf("\n%s", msg2->message);

  free(msg);
  free(msg2);

  exit(0);
}

void chat() {
  printf("\n\n----------CHAT MODE----------\nWrite DISCONNECT to exit\n");
  size_t m_size = 200;
  char buffer[m_size];

  while (1) {
    if (turn == 1) {
      printf("Client%d: ", client_id);
      fgets(buffer, m_size, stdin);
      message* msg = (message*)calloc(1, sizeof(message));
      msg->mtype = CHAT;
      if (strcmp(buffer, "\n") == 0) {
        printf("Hello Client%d\n", pair_id);
        sprintf(msg->message, "Hello Client%d\n", pair_id);
      } else if (strcmp(buffer, "DISCONNECT\n") != 0)
        strcpy(msg->message, buffer);
      else
        strcpy(msg->message, "Has disconnected from the chat\n");

      if (mq_send(pair_queue_id, (char*)msg, MSGSZ, 1) == -1)
        ERROR("Error while sending message to the client!\n");

      if (strcmp(buffer, "DISCONNECT\n") == 0) {
        process_disconnect();
        break;
      }

      free(msg);

      turn = 0;
    } else {
      struct mq_attr state;
      message rcv;

      while (1) {
        if (mq_getattr(queue_id, &state) == -1)
          ERROR("Error while reading current state!\n");

        if (state.mq_curmsgs > 0) {
          mq_receive(queue_id, (char*)&rcv, MSGSZ, NULL);
          printf("Client%d: %s", pair_id, rcv.message);
          break;
        }
      }
      turn = 1;
    }
  }
  printf("\n\n----------EXITING CHAT----------\n");
  in_chat = 0;
  mq_close(pair_queue_id);
}

void process_input() {
  if (in_chat)
    chat();

  char buffer[100];
  scanf("%s", buffer);

  if (strcmp(buffer, "LIST") == 0) {
    process_list();
  } else if (strcmp(buffer, "CONNECT") == 0) {
    process_connect();
  } else if (strcmp(buffer, "DISCONNECT") == 0) {
    process_disconnect();
  } else if (strcmp(buffer, "STOP") == 0) {
    process_stop();
  } else {
    printf("Wrong input!\n");
  }
}

void handler(int signum) {
  process_stop();
}

void usr1_handler(int signum) {
  message* msg = (message*)calloc(1, sizeof(message));
  mq_receive(queue_id, (char*)msg, MSGSZ, NULL);

  printf("%s", msg->message);

  pair_id = msg->client_id;
  char pth[20];
  sprintf(pth, "/%d", msg->client_pid);
  pair_queue_id = mq_open(pth, O_WRONLY);
  in_chat = 1;
  turn = 0;

  sleep(1);

  if (signal(SIGUSR1, usr1_handler) == SIG_ERR)
    ERROR("Error while establishing handler for SIGUSR1!\n");

  process_input();
}

int main() {
  if (signal(SIGINT, handler) == SIG_ERR)
    ERROR("Error while establishing handler for SIGINT!\n");

  if (signal(SIGUSR1, usr1_handler) == SIG_ERR)
    ERROR("Error while establishing handler for SIGUSR1!\n");

  if (atexit(close_client) == -1)
    ERROR("Error while setting exit function with atexit!\n");

  sprintf(path, "/%d", getpid());

  struct mq_attr attr;
  attr.mq_maxmsg = MAXMSG;
  attr.mq_msgsize = MSGSZ;

  queue_id = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
  if (queue_id == -1)
    ERROR("Error while creating new queue!\n");

  server_queue_id = mq_open(PATH, O_WRONLY);
  if (server_queue_id == -1)
    ERROR("Error while accessing server queue!\n");

  message* msg = (message*)calloc(1, sizeof(message));
  msg->mtype = INIT;
  strcpy(msg->path, path);
  msg->client_pid = getpid();

  if (mq_send(server_queue_id, (char*)msg, MSGSZ, 6) == -1) {
    ERROR("Error while sending message to the server!\n");
  }
  free(msg);

  sleep(1);

  message* msg2 = (message*)calloc(1, sizeof(message));
  mq_receive(queue_id, (char*)msg2, MSGSZ, NULL);
  client_id = msg2->client_id;
  server_pid = msg2->client_pid;

  free(msg2);

  printf("Your ClientID = %d\n", client_id);
  while (!0) {
    if (running) {
      process_input();
    } else
      break;
  }
  return 0;
}