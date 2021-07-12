#pragma once

#define TCP_SERVER_IP_ADDR "127.0.0.1" // Loopback Address
#define TCP_SERVER_PORT_NUMBER 8888
#define MAX_NUM_TCP_CONN_REQS_IN_QUE 5
#define THREAD_POOL_NUM_THREADS 3
#define TCP_SERVER_TX_MSG "Response from TCP Server to TCP Client"
#define TCP_SERVER_RUNNING_DURATION 40 // Unit: second

#define TCP_CLIENT_IP_ADDR "127.0.0.1" // Loopback Address
#define TCP_CLIENT_PORT_NUMBER_BASE 33500
#define TCP_CLIENT_TX_MSG_NUM 5
#define TCP_CLIENT_TX_MSG_INTERVAL 3 // Unit: second
#define TCP_CLIENT_TX_MSG "Request (%u) from TCP Client (port: %u) to TCP Server"
#define NUM_TCP_CLIENTS 5

#define UDP_ENDPOINT_A_IP_ADDR "127.0.0.1" // Loopback Address
#define UDP_ENDPOINT_A_PORT_NUMBER 8888
#define UDP_ENDPOINT_A_TX_MSG "Response from UDP Endpoint A to UDP Endpoint B"
#define UDP_ENDPOINT_A_RUNNING_DURATION 40 // Unit: second

#define UDP_ENDPOINT_B_IP_ADDR "127.0.0.1" // Loopback Address
#define UDP_ENDPOINT_B_PORT_NUMBER_BASE 33500
#define UDP_ENDPOINT_B_TX_MSG_NUM 5
#define UDP_ENDPOINT_B_TX_MSG_INTERVAL 3 // Unit: second
#define UDP_ENDPOINT_B_TX_MSG "Request (%u) from UDP Endpoint B (port: %u) to UDP Endpoint A"
#define NUM_UDP_ENDPOINTS_B 5

//#define SOFTWARE_VERSION "V10.01.00.00"

/* End of File */
