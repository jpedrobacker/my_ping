/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:35 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 00:00:00 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/main.h"

int config_addr(t_ping *ping);

void print_help(void)
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf("  -v             verbose output\n");
	printf("  -n             numeric output only (no reverse DNS is ever done)\n");
	printf("  -T, --ttl=N    set the IP time to live\n");
	printf("  -W SECONDS     time to wait for a reply\n");
	printf("  -w SECONDS     stop after this many seconds have elapsed\n");
	printf("  -s BYTES       set the number of ICMP data bytes to send\n");
	printf("  -?             give this help list\n");
}

/*
** Dorme só o tempo que falta para completar PING_INTERVAL_SEC desde o
** último envio. Sem isso, um pacote perdido custaria SO_RCVTIMEO
** (esperando a resposta) + esse sleep inteiro — quase o dobro do
** intervalo normal entre envios. Calculando o "restante", o intervalo
** entre pacotes fica ~constante independente de a resposta chegar
** rápido ou de dar timeout.
** nanosleep aceita segundos + nanossegundos separados (struct
** timespec), por isso a conta quebra `remaining` em parte inteira
** (tv_sec) e fracionária (tv_nsec).
*/
static void sleep_remaining(double elapsed_sec)
{
	struct timespec ts;
	double remaining = PING_INTERVAL_SEC - elapsed_sec;

	if (remaining <= 0)
		return;
	ts.tv_sec = (time_t)remaining;
	ts.tv_nsec = (long)((remaining - (double)ts.tv_sec) * 1e9);
	nanosleep(&ts, NULL);
}

/*
** Imprime o resumo final, no formato "--- host ping statistics ---"
** do ping de verdade. Só calcula rtt/mdev se pelo menos uma resposta
** chegou (dividir por stats.received == 0 seria divisão por zero).
** mdev (desvio médio) é aproximado aqui como desvio padrão
** populacional: sqrt(E[rtt²] - E[rtt]²), a forma "online" de calcular
** variância sem guardar todos os RTTs individuais — só precisamos
** acumular soma e soma dos quadrados a cada resposta (ver icmp.c).
*/
static void print_stats(t_ping *ping)
{
	unsigned int lost;
	double loss_pct;

	lost = ping->stats.transmitted - ping->stats.received;
	loss_pct = 0.0;
	if (ping->stats.transmitted > 0)
		loss_pct = (double)lost * 100.0 / (double)ping->stats.transmitted;

	printf("\n--- %s ping statistics ---\n", ping->hostname);
	printf("%u packets transmitted, %u received, %.0f%% packet loss\n",
		ping->stats.transmitted, ping->stats.received, loss_pct);

	if (ping->stats.received > 0)
	{
		double avg = ping->stats.rtt_sum / ping->stats.received;
		double variance = ping->stats.rtt_sum2 / ping->stats.received - avg * avg;
		/* por causa de arredondamento de ponto flutuante, variance
		** pode dar um número negativo bem pequeno quando o RTT é quase
		** constante; sqrt de negativo seria NaN, então tratamos como 0. */
		double mdev = 0.0;

		if (variance > 0)
			mdev = sqrt(variance);

		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			ping->stats.rtt_min, avg, ping->stats.rtt_max, mdev);
	}
}

/*
** Loop principal: envia um Echo Request, espera a resposta (ou o
** timeout de -W), dorme o restante do intervalo, e repete até:
**   - o usuário apertar Ctrl+C (g_run vira 0 dentro do signal handler), ou
**   - o prazo de -w (deadline_sec) ser atingido.
** tv_start é fixado uma vez, no começo, só para medir o tempo total
** decorrido (usado pelo -w); tv_send é recalculado a cada volta, para
** medir quanto essa iteração específica levou (usado por
** sleep_remaining).
*/
void main_loop(t_ping *ping)
{
	struct timeval tv_start;
	struct timeval tv_send;
	struct timeval tv_now;
	int seq = 0;

	g_run = 1;
	handle_sigint();
	gettimeofday(&tv_start, NULL);

	/* header impresso sempre, mesmo sem -v — é isso que o subject
	** pede (indentação/saída igual ao inetutils-2.0). */
	printf("PING %s (%s): %zu data bytes\n", ping->hostname, ping->raw_ip, ping->payload_size);

	while (g_run)
	{
		double elapsed;
		double total_elapsed;

		gettimeofday(&tv_send, NULL);

		send_icmp_packet(ping, seq++);
		listen_packet_reply(ping);

		gettimeofday(&tv_now, NULL);
		elapsed = (tv_now.tv_sec - tv_send.tv_sec) + (tv_now.tv_usec - tv_send.tv_usec) / 1000000.0;
		sleep_remaining(elapsed);

		/* bonus -w: para de ENVIAR novos pacotes depois do prazo, mas
		** ainda espera a última resposta pendente (o listen_packet_reply
		** dessa mesma volta já rodou acima) antes de sair do loop. */
		if (ping->deadline_sec > 0)
		{
			total_elapsed = (tv_now.tv_sec - tv_start.tv_sec) + (tv_now.tv_usec - tv_start.tv_usec) / 1000000.0;
			if (total_elapsed >= ping->deadline_sec)
				break;
		}
	}

	print_stats(ping);
}

/*
** Zera a struct inteira primeiro (memset) e só depois define os
** valores padrão — assim qualquer campo novo adicionado no futuro
** nasce em zero/NULL por segurança, e só os campos que realmente
** precisam de um valor diferente de zero são explicitados aqui
** (ttl = -1 sentinela "não definido pelo usuário", timeout/payload com
** os defaults do ping de verdade).
*/
void init_ping(t_ping *ping)
{
	memset(ping, 0, sizeof(*ping));
	ping->help = false;
	ping->verbose = false;
	ping->numeric = false;
	ping->hostname = NULL;
	ping->raw_ip = NULL;
	ping->ttl = -1;
	ping->timeout_sec = DEFAULT_TIMEOUT_SEC;
	ping->deadline_sec = 0;
	ping->payload_size = DEFAULT_PAYLOAD_SIZE;
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

	/* -? sempre atalha aqui, antes de qualquer socket ser aberto —
	** por isso funciona mesmo sem um hostname na linha de comando. */
	if (ping.help == true)
	{
		print_help();
		return (0);
	}

	if (config_addr(&ping) == -1)
		return (-1);

	main_loop(&ping);

	close(ping.socket.fd);

	/* convenção do ping de verdade: código de saída 1 quando nenhum
	** pacote voltou (perda de 100%), mesmo sem nenhum erro "fatal". */
	if (ping.stats.transmitted > 0 && ping.stats.received == 0)
		return (1);
	return (0);
}
