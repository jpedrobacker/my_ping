/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:22:49 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 18:51:30 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# define _POSIX_C_SOURCE 200112L

/* intervalo alvo entre dois envios consecutivos (main_loop, main.c);
** não é configurável por flag (o subject não pede um -i). */
# define PING_INTERVAL_SEC 1.0

# include <stdint.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <string.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <netdb.h>
# include <unistd.h>
# include <errno.h>
# include <sys/time.h>
# include <time.h>
# include <signal.h>
# include <float.h>
# include <math.h>

# include "parse.h"
# include "icmp.h"
# include "sig_handle.h"


#endif
