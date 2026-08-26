/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   addr_config.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:36 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/12 19:17:21 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

int config_addr(t_ping *ping)
{
	struct sockaddr_storage sock_config = {0};
	struct addrinfo hints, *res;

	res = NULL;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = ping->ip_type;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	//hints.ai_flags = AI_NUMERICHOST;

	if (getaddrinfo(ping->hostname, NULL, &hints, &res) != 0)
	{
		printf("Invalid IP or Hostname not found!\n");
		return (-1);
	}

	memcpy((struct sockaddr *)&sock_config, res->ai_addr, res->ai_addrlen);

	if (ping->ip_type == IPV4)
	{
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
		ping->raw_ip = inet_ntop(AF_INET, &(ipv4->sin_addr),ping->ip_str, sizeof(ping->ip_str));
	}
	else if (ping->ip_type == IPV6)
	{
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
		ping->raw_ip = inet_ntop(AF_INET6, &(ipv6->sin6_addr),ping->ip_str, sizeof(ping->ip_str));
	}

	ping->socket.addr_len = res->ai_addrlen;

	freeaddrinfo(res);

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		printf("Permission denied or cannot open socket");
		return (-1);
	}

	ping->socket.fd = sockfd;
	ping->socket.remote_addr = sock_config;



	return (sockfd);
}
