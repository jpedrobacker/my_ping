/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   addr_config.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:36 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/16 15:19:45 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

int config_addr(t_ping *ping, struct addrinfo *hints, struct addrinfo **res)
{
	memset(hints, 0, sizeof(struct addrinfo));

	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_RAW;
	hints->ai_protocol = IPPROTO_ICMP;
	hints->ai_flags = AI_NUMERICHOST;

	if (getaddrinfo(ping->hostname, NULL, hints, res) != 0)
	{
		hints->ai_flags = 0;
		if (getaddrinfo(ping->hostname, NULL, hints, res) != 0)
		{
			printf("Invalid IP or Hostname not found!\n");
			return (-1);
		}
	}

	struct sockaddr_in *ipv4 = (struct sockaddr_in *)(*res)->ai_addr;
	char ip_str[INET_ADDRSTRLEN];

	ping->raw_ip = inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, sizeof(ip_str));
	if (ping->raw_ip != NULL)
		printf("PING %s (%s)\n", ping->hostname, ip_str);

	int sockfd = socket((*res)->ai_family, (*res)->ai_socktype, (*res)->ai_protocol);
	if (sockfd < 0)
	{
		printf("Permission denied or cannot open socket");
		freeaddrinfo(*res);
		return (-1);
	}

	return (sockfd);
}
