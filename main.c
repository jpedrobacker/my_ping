/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:35 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/12 18:56:34 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/main.h"

int config_addr(t_ping *ping);

int g_run;

void print_help(void)
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf("  -v         verbose output\n");
	printf("  -?         give this help list\n");
}

void main_loop(t_ping *ping, int sockfd)
{
	g_run = 1;
	handle_sigint(SIGINT);
	int seq = 0;
	if (ping->verbose == true)
	{
		printf("PING %s (%s): %ld data bytes\n", ping->hostname, ping->raw_ip, (long)(ICMP_PACKET_SIZE - sizeof(struct icmphdr)));
	}
	else
	{

	}
	while(g_run)
	{
		struct timeval tv_send;
		gettimeofday(&tv_send, NULL);

		send_icmp_packet(ping, sockfd, seq++);

		listen_packet_reply(sockfd, &tv_send, ping->verbose);

		sleep(1);
	}
}

void init_ping(t_ping *ping)
{
	memset(ping, 0, sizeof(*ping));
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

	if (ping.help == true)
	{
		print_help();
		return (0);
	}

	int sockfd = config_addr(&ping);
	if (sockfd == -1)
		return (-1);

	main_loop(&ping, sockfd);

	return (0);
}
