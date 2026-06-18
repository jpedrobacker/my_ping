/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:16:17 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/17 20:09:51 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef ICMP_H
# define ICMP_H

# define _POSIX_C_SOURCE 200112L
# define _GNU_SOURCE
# define _DEFAULT_SOURCE

# include <netdb.h>
# include <netinet/ip_icmp.h>
# include <netinet/ip.h>
# include <sys/time.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <stdint.h>
# include <stddef.h>
# include <arpa/inet.h>
# include "parse.h"

void	send_icmp_packet(t_ping *ping, int sockfd, struct addrinfo *res, int seq);
void	listen_packet_reply(int sockfd, struct timeval *tv_send);

#endif
