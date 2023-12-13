#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net/gcoap.h"
#include "od.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                          const sock_udp_ep_t *remote);

// Set address and port for the server
static char set_addr[] = "2600:1900:4150:7757::";
static char set_port[] = "8683";

// Counts requests sent by CLI
static uint16_t req_count = 0;

// Response handler
static void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                          const sock_udp_ep_t *remote)
{
    (void)remote;       /* not interested in the source currently */

    if (memo->state == GCOAP_MEMO_TIMEOUT) {
        printf("gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
        return;
    }
    else if (memo->state == GCOAP_MEMO_ERR) {
        printf("gcoap: error in response\n");
        return;
    }

    coap_block1_t block;
    if (coap_get_block2(pdu, &block) && block.blknum == 0) {
        puts("--- blockwise start ---");
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    printf("gcoap: response %s, code %1u.%02u", class_str,
                                                coap_get_code_class(pdu),
                                                coap_get_code_detail(pdu));
    if (pdu->payload_len) {
        unsigned content_type = coap_get_content_type(pdu);
        if (content_type == COAP_FORMAT_TEXT
                || content_type == COAP_FORMAT_LINK
                || coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE
                || coap_get_code_class(pdu) == COAP_CLASS_SERVER_FAILURE) {
            /* Expecting diagnostic payload in failure cases */
            printf(", %u bytes\n%.*s\n", pdu->payload_len, pdu->payload_len,
                                                          (char *)pdu->payload);
        }
        else {
            printf(", %u bytes\n", pdu->payload_len);
            od_hex_dump(pdu->payload, pdu->payload_len, OD_WIDTH_DEFAULT);
        }
    }
    else {
        printf(", empty payload\n");
    }
}

static size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str)
{
    ipv6_addr_t addr;
    size_t bytes_sent;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;

    /* parse for interface */
    char *iface = ipv6_addr_split_iface(addr_str);
    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            remote.netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            puts("gcoap_cli: interface not valid");
            return 0;
        }
        remote.netif = pid;
    }
    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("gcoap_cli: unable to parse destination address");
        return 0;
    }
    if ((remote.netif == SOCK_ADDR_ANY_NETIF) && ipv6_addr_is_link_local(&addr)) {
        puts("gcoap_cli: must specify interface for link local target");
        return 0;
    }
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    /* parse port */
    remote.port = atoi(port_str);
    if (remote.port == 0) {
        puts("gcoap_cli: unable to parse destination port");
        return 0;
    }

    bytes_sent = gcoap_req_send(buf, len, &remote, _resp_handler, NULL);

    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}

int gcoap_cli_send(char method[], char *data, char *resource)
{
    // Only acceptable methods are shown below
    char *method_codes[] = {"get", "post", "put"};
    coap_pkt_t pdu;
    size_t len;
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];

    // Check if method is one in method_codes
    int code_pos = -1;
    for (size_t i = 0; i < ARRAY_SIZE(method_codes); i++) {
        if (strcmp(method, method_codes[i]) == 0) {
            code_pos = i;
        }
    }

    if (code_pos == -1) {
        return 0;
    }

    gcoap_req_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE, code_pos + 1, resource);
    coap_hdr_set_type(pdu.hdr, COAP_TYPE_NON);
    coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);
    len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);

    memcpy(pdu.payload, data, strlen(data));
    len += strlen(data);

    printf("gcoap_cli: sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
               (unsigned) len);
    if (_send(&buf[0], len, set_addr, set_port) == 0) {
        puts("gcoap_cli: msg send failed");
        return 0;
    }
    return 1;
}