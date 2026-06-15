/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   addr_config.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:36 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/15 15:31:37 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

int config_addr(t_ping *ping, struct addrinfo *hints, struct addrinfo *res)
{
	memset(&hints, 0, sizeof(hints));

	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_RAW;
	hints->ai_protocol = IPPROTO_ICMP;

	if (getaddrinfo(ping->hostname, NULL, &hints, &res) != 0)
	{
		printf("Hostname not found!\n");
		return (1);
	}

	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0)
	{
		printf("Permission denied or cannot open socket");
		return (1);
	}

	return (sockfd);
}
