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

/* defaults do ping de verdade: 1s de espera por resposta e 56 bytes de
** payload ICMP (56 + 8 do cabeçalho = os clássicos "64 bytes"). */
# define DEFAULT_TIMEOUT_SEC 1.0
# define DEFAULT_PAYLOAD_SIZE 56

/* tudo que diz respeito ao socket em si: o fd aberto em config_addr,
** o tamanho do sockaddr resolvido, e o endereço de destino já
** preenchido (usado em todo sendto). */
typedef struct s_sock_configs
{
	int			fd;
	socklen_t		addr_len;
	struct sockaddr_storage	remote_addr;

} t_sock_configs;

/*
** Contadores acumulados durante o main_loop, usados só no final para
** montar o resumo "--- host ping statistics ---" (ver print_stats em
** main.c). rtt_sum/rtt_sum2 permitem calcular média e desvio padrão
** (mdev) sem guardar cada RTT individual numa lista.
*/
typedef struct s_stats
{
	unsigned int	transmitted;
	unsigned int	received;
	double		rtt_min;
	double		rtt_max;
	double		rtt_sum;
	double		rtt_sum2;

}	t_stats;

/*
** Estado completo de uma execução do ft_ping: o que foi pedido na
** linha de comando (verbose/numeric/ttl/timeout/deadline/payload_size),
** o que foi resolvido a partir do hostname (raw_ip/ip_str/socket), e o
** que foi observado durante a execução (stats).
*/
typedef struct s_ping
{
	char *hostname;
	bool verbose;
	bool help;
	bool numeric;
	int ttl;              /* -1 = não definido pelo usuário (-T/--ttl) */
	double timeout_sec;    /* -W: tempo de espera por resposta */
	double deadline_sec;   /* -w: 0 = sem prazo; senão, prazo total em segundos */
	size_t payload_size;   /* -s: bytes de dados ICMP (sem contar o cabeçalho) */
	const char *raw_ip;
	char ip_str[INET_ADDRSTRLEN];
	t_sock_configs socket;
	t_stats stats;

}	t_ping;

int	parse_arguments(int ac, char **av, t_ping *ping);

#endif
