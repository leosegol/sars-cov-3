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
/*
struct IP_header
{
    uint8_t   ver_hlen;     // Header version and length (dwords). 
    uint8_t   service;      // Service type. 
    uint16_t  length;       // Length of datagram (bytes). 
    uint16_t  ident;        // Unique packet identification. 
    uint16_t  fragment;     // Flags; Fragment offset. 
    uint8_t   timetolive;   // Packet time to live (in network). 
    uint8_t   protocol;     // Upper level protocol (UDP, TCP). 
    uint16_t  checksum;     // IP header checksum. 
    uint32_t  src_addr;     // Source IP address. 
    uint32_t  dest_addr;    // Destination IP address. 
};
*/

struct IP_header
{
    uint8_t ip_header_len:4;      // 4-bit header length (in 32-bit words)
    uint8_t ip_version:4;         // 4-bit IPv4 version
    uint8_t ip_tos;             // IP type of service
    uint16_t ip_total_length;   // Total length
    uint16_t ip_id;             // Unique identifier

    uint8_t ip_frag_offset : 5; // Fragment offset field
    uint8_t ip_more_fragment : 1;
    uint8_t ip_dont_fragment : 1;
    uint8_t ip_reserved_zero : 1;
    uint8_t ip_frag_offset1;    //fragment offset

    uint8_t ip_ttl;             // Time to live
    uint8_t ip_protocol;        // Protocol(TCP,UDP etc)
    uint16_t ip_checksum;       // IP checksum
    uint32_t ip_srcaddr;        // Source address
    uint32_t ip_destaddr;       // Source address
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

struct Ethernet_header {
    uint8_t dest_addr[6]; /* Destination hardware address */
    uint8_t src_addr[6];  /* Source hardware address */
    uint16_t frame_type;    /* Ethernet frame type */
};
