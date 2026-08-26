/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:16:22 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/12 19:17:28 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

static unsigned short calc_checksum(void *address, size_t len)
{
	unsigned short *buffer = (unsigned short *)address;
	unsigned int sum = 0;
	unsigned short result;

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

void send_icmp_packet(t_ping *ping, int sockfd, int seq)
{
	char packet[ICMP_PACKET_SIZE];
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

	ssize_t bytes_sent = sendto(sockfd, packet, packet_len, 0, (struct sockaddr *)&ping->socket.remote_addr, ping->socket.addr_len);
	if (bytes_sent < 0)
		perror("Send to Failed!\n");
	else
		printf("Successfully sent %ld bytes of ICMP data to %s\n", bytes_sent, ping->raw_ip);
}

void listen_packet_reply(int sockfd, struct timeval *tv_send, bool verbose)
{
	char buffer[1024];
	struct sockaddr_in from;
	socklen_t from_len = sizeof(from);

	struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		perror("setsockpt Failed!");
		return;
	}

	ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);

	if (bytes_received < 0)
		return;

	struct ip *ip_hdr = (struct ip *)buffer;
	int ip_hdr_len = ip_hdr->ip_hl * 4;

	if (bytes_received < ip_hdr_len + (ssize_t)sizeof(struct icmphdr))
		return;

	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + ip_hdr_len);

	if (icmp_hdr->type == ICMP_ECHOREPLY)
	{
		if (icmp_hdr->un.echo.id == (getpid() & 0xFFFF))
		{
			struct timeval tv_recv;
			gettimeofday(&tv_recv, NULL);

			double rtt = (tv_recv.tv_sec - tv_send->tv_sec) * 1000.0 + (tv_recv.tv_usec - tv_send->tv_usec) / 1000.0;

			printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n", bytes_received - ip_hdr_len, inet_ntoa(from.sin_addr), icmp_hdr->un.echo.sequence, ip_hdr->ip_ttl, rtt);
		}
	}
	else if (verbose == true)
	{
		const char *type_str;

		if (icmp_hdr->type == ICMP_DEST_UNREACH)
			type_str = "Destination Unreachable";
		else if (icmp_hdr->type == ICMP_TIME_EXCEEDED)
			type_str = "Time Exceeded";
		else if (icmp_hdr->type == ICMP_SOURCE_QUENCH)
			type_str = "Source Quench";
		else if (icmp_hdr->type == ICMP_REDIRECT)
			type_str = "Redirect";
		else if (icmp_hdr->type == ICMP_PARAMETERPROB)
			type_str = "Parameter Problem";
		else
			type_str = "Unknown ICMP type";

		printf("From %s: %s (type=%d code=%d)\n", inet_ntoa(from.sin_addr), type_str, icmp_hdr->type, icmp_hdr->code);
	}
}
