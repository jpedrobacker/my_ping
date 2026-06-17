/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:16:22 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/17 19:16:23 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

static unsigned short calc_checksum(void *address, size_t len)
{
	unsigned short *buffer = (unsigned short *)address;
	unsigned int    sum = 0;
	unsigned short  result;

	while (len > 1)
	{
		sum += *buffer++;
		len -= 2;
	}

	if (len == 1)
		sum += *(unsigned char *)buffer;

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	result = ~sum;
	return (result);
}

void send_icmp_packet(t_ping *ping, int sockfd, struct addrinfo *res, int seq)
{
	char packet[64];
	size_t packet_len = sizeof(packet);

	memset(packet, 0, packet_len);

	struct icmphdr *icmp = (struct icmphdr *)packet;


	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = getpid() & 0xFFFF;
	icmp->un.echo.sequence = seq;
	icmp->checksum = 0;
	// adicionar o payload para o bonus

	struct timeval tv;
	gettimeofday(&tv, NULL);
	memcpy(packet + sizeof(struct icmphdr), &tv, sizeof(tv));

	icmp->checksum = calc_checksum(packet, packet_len);

	ssize_t bytes_sent = sendto(sockfd, packet, packet_len, 0, res->ai_addr, res->ai_addrlen);
	if (bytes_sent < 0)
		perror("Send to Failed!\n");
	else
		printf("Successfully sent %ld bytes of ICMP data to %s\n", bytes_sent, ping->raw_ip);
}


void listen_packet_reply(int sockfd, struct timeval *tv_send)
{
	(void) sockfd;
	(void) tv_send;
}
