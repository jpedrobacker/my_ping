/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:30 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/27 16:22:15 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "main.h"

# define DEFAULT_TIMEOUT_SEC 1.0
# define DEFAULT_PAYLOAD_SIZE 56

typedef struct s_sock_configs
{
	int			fd;
	socklen_t		addr_len;
	struct sockaddr_storage	remote_addr;

} t_sock_configs;

typedef struct s_stats
{
	unsigned int	transmitted;
	unsigned int	received;
	double		rtt_min;
	double		rtt_max;
	double		rtt_sum;
	double		rtt_sum2;

}	t_stats;

typedef struct s_ping
{
	char *hostname;
	bool verbose;
	bool help;
	bool numeric;
	int ttl;
	double timeout_sec;
	double deadline_sec;
	size_t payload_size;
	const char *raw_ip;
	char ip_str[INET_ADDRSTRLEN];
	t_sock_configs socket;
	t_stats stats;

}	t_ping;

int	parse_arguments(int ac, char **av, t_ping *ping);

#endif
