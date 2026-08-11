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

typedef enum e_ipType
{
	IPV4 = AF_INET,
	IPV6 = AF_INET6
}	ipType;

typedef struct s_sock_configs
{
	int			fd;
	socklen_t		addr_len;
	struct sockaddr_in	*ipv4_addr;
	struct sockaddr_in6	*ipv6_addr;
	struct sockaddr_storage	remote_addr;

} t_sock_configs;

typedef struct s_ping
{
	char *hostname;
	bool verbose;
	bool help;
	const char *raw_ip;
	ipType ip_type;
	char ip_str[INET_ADDRSTRLEN];
	t_sock_configs socket;

}	t_ping;

int	parse_arguments(int ac, char **av, t_ping *ping);

#endif
