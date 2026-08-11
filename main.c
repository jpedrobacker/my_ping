/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:35 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/27 16:25:20 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/main.h"

int config_addr(t_ping *ping);

int g_run;

void main_loop(t_ping *ping, int sockfd)
{
	g_run = 1;
	signal(SIGINT, handle_sigint);
	int seq = 0;
	if (ping->verbose == true)
	{
		int pid = getpid();
		printf("PING %s (%s): %ld data bytes\n");
	}
	else
	{

	}
	while(g_run)
	{
		struct timeval tv_send;
		gettimeofday(&tv_send, NULL);

		send_icmp_packet(ping, sockfd, res, seq++);

		listen_packet_reply(sockfd, &tv_send);

		sleep(1);
	}
}

void init_ping(t_ping *ping)
{
	memset(ping, 0, sizeof(ping));
	ping->help = false;
	ping->verbose = false;
	ping->hostname = NULL;
	ping->ip_type = IPV4;
	ping->raw_ip = NULL;
}

int main(int ac, char **av)
{
	t_ping ping;

	init_ping(&ping);

	if (parse_arguments(ac, av, &ping) != 0)
	{
		printf("Error!\n");
		return (-1);
	}

	int sockfd = config_addr(&ping);
	if (sockfd == -1)
		return (-1);

	main_loop(&ping, sockfd);

	return (0);
}
