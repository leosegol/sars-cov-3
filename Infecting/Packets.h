#include<iostream>

struct DHCP_header
{
    uint8_t op;         // 1 is request, 2 is reply
    uint8_t htype;      // type of hardware: 1 is Ether, 2 is wlan
    uint8_t hlen;       // length of the address
    uint8_t hops;       // indicates nomber of hops
    uint32_t xid;       // identification
    uint16_t secs;      // time since discover by the client
    uint16_t flags;     // 1 for a discover packet
    uint32_t ciaddr;    // client's ip if he has one
    uint32_t yiaddr;    // ip set by the server
    uint32_t siaddr;    // server's ip
    uint32_t giaddr;    // gateway's ip
    char chaddr[16];    // client's mac address
    char sname[64];     // server's name or DNS address
    char file[128];     // type of boot
    char magic[4];      // DHCP options
    char opt[3];        // DHCP options
};

struct IP_header
{
    uint8_t   ver_hlen;     /* Header version and length (dwords). */
    uint8_t   service;      /* Service type. */
    uint16_t  length;       /* Length of datagram (bytes). */
    uint16_t  ident;        /* Unique packet identification. */
    uint16_t  fragment;     /* Flags; Fragment offset. */
    uint8_t   timetolive;   /* Packet time to live (in network). */
    uint8_t   protocol;     /* Upper level protocol (UDP, TCP). */
    uint16_t  checksum;     /* IP header checksum. */
    uint32_t  src_addr;     /* Source IP address. */
    uint32_t  dest_addr;    /* Destination IP address. */
};

struct UDP_header
{
    uint16_t src_port;      /* Source port */
    uint16_t dst_port;      /* Destination port */
    uint16_t len;           /* Length */
    uint16_t chksum;        /* Checksum */
};

struct DNS_header
{
    uint16_t id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answers;
    uint16_t authority;
    uint16_t additional;
};