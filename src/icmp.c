/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:16:22 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 18:59:07 by jbergfel         ###   ########.fr       */
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

void send_icmp_packet(t_ping *ping, int seq)
{
	char packet[sizeof(struct icmphdr) + ICMP_MAX_PAYLOAD];
	size_t packet_len = sizeof(struct icmphdr) + ping->payload_size;
	struct timeval tv;
	ssize_t bytes_sent;

	memset(packet, 0, packet_len);

	struct icmphdr *icmp = (struct icmphdr *)packet;

	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = getpid() & 0xFFFF;
	icmp->un.echo.sequence = seq;
	icmp->checksum = 0;

	gettimeofday(&tv, NULL);
	memcpy(packet + sizeof(struct icmphdr), &tv, sizeof(tv));

	icmp->checksum = calc_checksum(packet, packet_len);

	bytes_sent = sendto(ping->socket.fd, packet, packet_len, 0, (struct sockaddr *)&ping->socket.remote_addr, ping->socket.addr_len);
	if (bytes_sent < 0)
		perror("sendto failed");
	else
		ping->stats.transmitted++;
}

static void print_error_reply(struct icmphdr *icmp_hdr, struct sockaddr_in *from)
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

	printf("From %s: %s (type=%d code=%d)\n", inet_ntoa(from->sin_addr),
		type_str, icmp_hdr->type, icmp_hdr->code);
}

void listen_packet_reply(t_ping *ping)
{
	char buffer[IP_MAXPACKET];
	struct sockaddr_in from;
	socklen_t from_len = sizeof(from);
	ssize_t bytes_received;
	struct ip *ip_hdr;
	int ip_hdr_len;
	struct icmphdr *icmp_hdr;

	bytes_received = recvfrom(ping->socket.fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);
	if (bytes_received < 0)
		return ;
	if (bytes_received < (ssize_t)sizeof(struct ip))
		return ;

	ip_hdr = (struct ip *)buffer;
	ip_hdr_len = ip_hdr->ip_hl * 4;

	if (bytes_received < ip_hdr_len + (ssize_t)sizeof(struct icmphdr))
		return ;

	icmp_hdr = (struct icmphdr *)(buffer + ip_hdr_len);

	if (icmp_hdr->type == ICMP_ECHOREPLY)
	{
		struct timeval tv_sent;
		struct timeval tv_recv;
		double rtt;

		if (icmp_hdr->un.echo.id != (getpid() & 0xFFFF))
			return ;
		if (bytes_received < ip_hdr_len + (ssize_t)sizeof(struct icmphdr) + (ssize_t)sizeof(tv_sent))
			return ;

		memcpy(&tv_sent, buffer + ip_hdr_len + sizeof(struct icmphdr), sizeof(tv_sent));
		gettimeofday(&tv_recv, NULL);
		rtt = (tv_recv.tv_sec - tv_sent.tv_sec) * 1000.0 + (tv_recv.tv_usec - tv_sent.tv_usec) / 1000.0;

		ping->stats.received++;
		if (ping->stats.received == 1 || rtt < ping->stats.rtt_min)
			ping->stats.rtt_min = rtt;
		if (ping->stats.received == 1 || rtt > ping->stats.rtt_max)
			ping->stats.rtt_max = rtt;
		ping->stats.rtt_sum += rtt;
		ping->stats.rtt_sum2 += rtt * rtt;

		printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
			(long)(bytes_received - ip_hdr_len), inet_ntoa(from.sin_addr),
			icmp_hdr->un.echo.sequence, ip_hdr->ip_ttl, rtt);
	}
	else if (ping->verbose)
		print_error_reply(icmp_hdr, &from);
}
