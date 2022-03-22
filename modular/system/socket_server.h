#pragma once

#include "pipe.h"
#include "spoll.h"
#include "spinlock.h"
#include "socket_info.h"
#include "socket_buffer.h"

#define SOCKET_DATA     0
#define SOCKET_CLOSE    1
#define SOCKET_OPEN     2
#define SOCKET_ACCEPT   3
#define SOCKET_ERR      4
#define SOCKET_EXIT     5
#define SOCKET_UDP      6
#define SOCKET_WARNING  7

// Only for internal use
#define SOCKET_RST      8
#define SOCKET_MORE     9

struct socket_server;

struct socket_message {
    int id;
    uintptr_t opaque;
    // for accept, ud is new connection id; for data, ud is size of data
    int ud;
    char * data;
};

extern struct socket_server * socket_server_create(uint64_t realtime);
extern void socket_server_release(struct socket_server * ss);
extern void socket_server_updatetime(struct socket_server * ss, uint64_t realtime);
extern int socket_server_poll(struct socket_server * ss, struct socket_message * result, int * more);

extern void socket_server_exit(struct socket_server * ss);
extern void socket_server_close(struct socket_server * ss, uintptr_t opaque, int id);
extern void socket_server_shutdown(struct socket_server * ss, uintptr_t opaque, int id);
extern void socket_server_start(struct socket_server * ss, uintptr_t opaque, int id);
extern void socket_server_pause(struct socket_server * ss, uintptr_t opaque, int id);

// return -1 when error
extern int socket_server_send(struct socket_server * ss, struct socket_sendbuffer * buffer);
int socket_server_send_lowpriority(struct socket_server * ss, struct socket_sendbuffer * buffer);

// ctrl command below returns id
extern int socket_server_listern(struct socket_server * ss, uintptr_t opaque, const char * addr, int port, int backlog);
extern int socket_server_connect(struct socket_server * ss, uintptr_t opaque, const char * addr, int port);
extern int socket_server_bind(struct socket_server * ss, uintptr_t opaque, int fd);

// for tcp
extern void socket_server_nodelay(struct socket_server * ss, int id);

struct socket_udp_address;

// create an upd socket handle, attach opaque with it. udp socket don't need call socket_server_start to recv message
// if port != 0, bind the socket. if addr == NULL, bind ipv4 0.0.0.0. If you want to use ipv6, addr can be "::" and port 0.
extern int socket_server_udp(struct socket_server * ss, uintptr_t opaque, const char * addr, int port);
// set default dest address, return 0 when success
extern int socket_server_udp_connect(struct socket_server * ss, int id, const char * addr, int port);
// If the socket_udp_address is NULL, use last call socket_server_udp_connect address instead
// You can also use socket_server_send
extern int socket_server_udp_send(struct socket_server * ss, const struct socket_udp_address * addr, struct socket_sendbuffer * buffer);
// extract the address of the message, struct socket_message * should be SOCKET_UDP
extern const struct socket_udp_address * socket_server_udp_address(struct socket_message * msg, int * addrsz);

struct socket_object_interface {
    const void * (* buffer)(const void * ptr);
    size_t (* size)(const void * ptr);
    void (* free)(void * ptr);
};

// if you seed package with type SOCKET_BUFFER_OBJECT, use soi
void socket_server_userobject(struct socket_server * ss, struct socket_object_interface * soi);

extern struct socket_info * socket_server_info(struct socket_server * ss);
