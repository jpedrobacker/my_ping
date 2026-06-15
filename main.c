/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:35 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/12 18:12:16 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/main.h"

int config_addr(t_ping *ping, struct addrinfo *hints, struct addrinfo *res);

int g_run;

void main_loop(struct addrinfo *res, int sockfd)
{
	g_run = 1;
	signal(SIGINT, handle_sigint);
	int seq = 0;
	while(g_run)
	{
		struct timeval tv_send;
		gettimeofday(&tv_send, NULL);

		send_icmp_packet(sockfd, res, seq++);

		listen_packet_reply(sockfd, &tv_send);

		sleep(1);
	}
}

int main(int ac, char **av)
{
	t_ping ping = {0};

	//memset(&ping, 0, sizeof(t_ping));

	if (parse_arguments(ac, av, &ping) != 0)
	{
		printf("Error!\n");
		return (1);
	}

	struct addrinfo hints, *res;

	int sockfd = config_addr(&ping, &hints, res);

	main_loop(res, sockfd);

	freeaddrinfo(res);
	return (0);
}
