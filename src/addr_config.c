/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   addr_config.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:36 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 18:46:29 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

int config_addr(t_ping *ping)
{
	struct sockaddr_storage sock_config = {0};
	struct addrinfo hints, *res;
	struct sockaddr_in *ipv4;
	int rc;

	res = NULL;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;

	rc = getaddrinfo(ping->hostname, NULL, &hints, &res);
	if (rc != 0)
	{
		printf("Invalid IP or Hostname not found: %s\n", gai_strerror(rc));
		return (-1);
	}

	memcpy((struct sockaddr *)&sock_config, res->ai_addr, res->ai_addrlen);

	ipv4 = (struct sockaddr_in *)res->ai_addr;
	ping->raw_ip = inet_ntop(AF_INET, &(ipv4->sin_addr), ping->ip_str, sizeof(ping->ip_str));

	ping->socket.addr_len = res->ai_addrlen;

	freeaddrinfo(res);

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		printf("Permission denied or cannot open socket\n");
		return (-1);
	}

	if (ping->ttl > 0 && setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0)
	{
		perror("setsockopt IP_TTL failed");
		close(sockfd);
		return (-1);
	}

	struct timeval timeout;
	timeout.tv_sec = (time_t)ping->timeout_sec;
	timeout.tv_usec = (suseconds_t)((ping->timeout_sec - timeout.tv_sec) * 1000000);
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		perror("setsockopt SO_RCVTIMEO failed");
		close(sockfd);
		return (-1);
	}

	ping->socket.fd = sockfd;
	ping->socket.remote_addr = sock_config;

	return (sockfd);
}
