#include "structures.h"

int queue_id;
int number_of_clients = 0;
int running = 1;
client clients[CLIENT_LIMIT];

void process_init(message* rcv) {
  clients[number_of_clients].client_id = number_of_clients;

  clients[number_of_clients].active = 1;
  clients[number_of_clients].disconnected = 0;
  clients[number_of_clients].client_pid = rcv->client_pid;
  number_of_clients++;
  clients[number_of_clients - 1].queue_id = mq_open(rcv->path, O_WRONLY);
  if (clients[number_of_clients - 1].queue_id == -1)
    ERROR("Error while opening client's queue!\n");
  message* to_client = (message*)calloc(1, sizeof(message));
  to_client->mtype = INIT;
  to_client->client_id = number_of_clients - 1;
  to_client->client_pid = getpid();
  strcpy(to_client->message,
         "You have been successfully connected to the server!\n");

  if (mq_send(clients[number_of_clients - 1].queue_id, (char*)to_client, MSGSZ,
              6) == -1)
    ERROR("Error while sending message to the client!\n");
  free(to_client);
  printf("New client with id = %d has connected to the server\n",
         number_of_clients - 1);
}

void process_stop(message* rcv) {
  clients[rcv->client_id].disconnected = 1;
  message* to_client = (message*)calloc(1, sizeof(message));
  to_client->mtype = STOP;
  to_client->client_id = rcv->client_id;
  strcpy(to_client->message,
         "You have been successfully disconnected from the server!\n");

  if (mq_send(clients[rcv->client_id].queue_id, (char*)to_client, MSGSZ, 5) ==
      -1)
    ERROR("Error while sending message to the client!\n");

  free(to_client);

  printf("Client with id = %d has disconnected from the server\n",
         rcv->client_id);
}

void process_disconnect(message* rcv) {
  clients[rcv->client_id].active = 1;
  message* to_client = (message*)calloc(1, sizeof(message));
  to_client->mtype = DISCONNECT;
  to_client->client_id = rcv->client_id;
  strcpy(to_client->message,
         "You have been successfully disconnected from the server!\n");
  if (mq_send(clients[rcv->client_id].queue_id, (char*)to_client, MSGSZ, 4) ==
      -1)
    ERROR("Error while sending message to the client!\n");

  free(to_client);

  printf(
      "Client with id = %d has disconnected from the chat with client with id "
      "= %d\n",
      rcv->client_id, rcv->pair_id);
}

void process_list(message* rcv) {
  message* to_client = (message*)calloc(1, sizeof(message));
  to_client->mtype = LIST;

  for (int i = 0; i < number_of_clients; i++) {
    if (clients[i].disconnected)
      continue;
    sprintf(to_client->message + strlen(to_client->message),
            "Client with ID = %d ", clients[i].client_id);
    if (clients[i].active)
      sprintf(to_client->message + strlen(to_client->message), "is active\n");
    else
      sprintf(to_client->message + strlen(to_client->message), "is unactive\n");
  }

  if (mq_send(clients[rcv->client_id].queue_id, (char*)to_client, MSGSZ, 3) ==
      -1)
    ERROR("Error while sending message to the client!\n");

  free(to_client);

  printf("Client with id = %d has asked to see the list of all online users\n",
         rcv->client_id);
}

void process_connect(message* rcv) {
  int queue_id_to_find = -1;
  pid_t user1_pid;
  pid_t user2_pid;

  for (int i = 0; i < number_of_clients; i++) {
    if (clients[i].active && clients[i].client_id == rcv->pair_id) {
      user2_pid = clients[i].client_pid;
      queue_id_to_find = clients[i].queue_id;
    }
  }

  message* to_client = (message*)calloc(1, sizeof(message));
  to_client->mtype = CONNECT;
  to_client->client_id = rcv->pair_id;
  to_client->queue_id = queue_id_to_find;
  to_client->client_pid = user2_pid;
  if (queue_id_to_find != -1)
    sprintf(to_client->message,
            "You have been successfully connected to client with ID = %d\n",
            rcv->pair_id);
  if (mq_send(clients[rcv->client_id].queue_id, (char*)to_client, MSGSZ, 2) ==
      -1)
    ERROR("Error while sending message to the client!\n");
  free(to_client);
  if (queue_id_to_find == -1)
    return;
  queue_id_to_find = -1;
  for (int i = 0; i < number_of_clients; i++) {
    if (clients[i].active && clients[i].client_id == rcv->client_id) {
      queue_id_to_find = clients[i].queue_id;
      user1_pid = clients[i].client_pid;
    }
  }

  message* to_client2 = (message*)calloc(1, sizeof(message));
  to_client2->mtype = CONNECT;
  to_client2->client_id = rcv->client_id;
  to_client2->queue_id = queue_id_to_find;
  to_client2->client_pid = user1_pid;
  if (queue_id_to_find != -1)
    sprintf(to_client2->message,
            "You have been successfully connected to client with ID = %d\n",
            rcv->client_id);

  if (mq_send(clients[rcv->pair_id].queue_id, (char*)to_client2, MSGSZ, 2) ==
      -1)
    ERROR("Error while sending message to the client!\n");

  kill(user2_pid, SIGUSR1);

  free(to_client2);

  clients[rcv->client_id].active = 0;
  clients[rcv->pair_id].active = 0;

  printf(
      "Client with id = %d has connected to the chat with client with id = "
      "%d\n",
      rcv->client_id, rcv->pair_id);
}

void process_msg(message* rcv) {
  if (rcv != NULL) {
    switch (rcv->mtype) {
      case INIT: {
        process_init(rcv);
        break;
      }
      case STOP: {
        process_stop(rcv);
        break;
      }
      case DISCONNECT: {
        process_disconnect(rcv);
        break;
      }
      case LIST: {
        process_list(rcv);
        break;
      }
      case CONNECT: {
        process_connect(rcv);
        break;
      }
      default: {
        break;
      }
    }
  }
}

void close_clients() {
  printf("\n");
  for (int i = 0; i < number_of_clients; i++) {
    if (!clients[i].disconnected) {
      clients[i].disconnected = 1;
      struct mq_attr attr;
      kill(clients[i].client_pid, SIGINT);
      while (1) {
        mq_getattr(queue_id, &attr);
        if (attr.mq_curmsgs > 0)
          break;
      }
      message rcv;
      mq_receive(queue_id, (char*)&rcv, MSGSZ, NULL);
      process_msg(&rcv);
    }
  }
}

void close_server() {
  close_clients();
  sleep(1);
  if (queue_id != -1) {
    if (mq_close(queue_id) == -1 || mq_unlink(PATH) == -1)
      printf("Error while closing queue!\n");
    else {
      printf("\nQueue deleted successfully and server is about to close\n");
    }
  }
}

void usr1_handler(int signum) {
  message rcv;
  mq_receive(queue_id, (char*)&rcv, MSGSZ, NULL);
  mq_close(clients[rcv.client_id].queue_id);

  if (signal(SIGUSR1, usr1_handler) == SIG_ERR)
    ERROR("Error while establishing handler for SIGUSR1!\n");
}

void handler(int signum) {
  running = 0;
}

int main() {
  if (signal(SIGINT, handler) == SIG_ERR)
    ERROR("Error while establishing handler for SIGINT!\n");
  if (signal(SIGUSR1, usr1_handler) == SIG_ERR)
    ERROR("Error while establishing handler for SIGUSR1!\n");

  if (atexit(close_server) == -1)
    ERROR("Error while setting exit function with atexit!\n");
  struct mq_attr attr;
  attr.mq_maxmsg = MAXMSG;
  attr.mq_msgsize = MSGSZ;
  queue_id = mq_open(PATH, O_CREAT | O_RDONLY | O_EXCL, 0666, &attr);
  if (queue_id == -1)
    ERROR("Error while creating new queue!\n");

  printf("The server is running...\n");
  message rcv;
  struct mq_attr state;
  while (!0) {
    if (running) {
      if (mq_getattr(queue_id, &state) == -1)
        ERROR("Error while reading current state!\n");
      if (state.mq_curmsgs > 0) {
        mq_receive(queue_id, (char*)&rcv, MSGSZ, NULL);
        process_msg(&rcv);
      }
    } else
      break;
  }
  return 0;
}
