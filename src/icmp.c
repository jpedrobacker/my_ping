/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 19:16:22 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 00:00:00 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

/*
** Checksum da Internet (RFC 1071): soma todos os "chunks" de 16 bits
** do pacote em um acumulador de 32 bits, depois dobra o overflow de
** volta pros 16 bits baixos (esse é o "while (sum >> 16)" — carry
** wraparound), e por fim inverte todos os bits (~sum).
** O ICMP exige esse checksum no cabeçalho pra o kernel/host de destino
** conseguir detectar corrupção do pacote em trânsito. Ele é calculado
** sobre o pacote inteiro (cabeçalho ICMP + payload) com o próprio
** campo checksum zerado antes do cálculo.
*/
static unsigned short calc_checksum(void *address, size_t len)
{
	unsigned short *buffer = (unsigned short *)address;
	unsigned int sum = 0;
	unsigned short result;

	while (len > 1)
	{
		sum += *buffer++;
		len -= 2;
	}

	/* se sobrar 1 byte ímpar no final, ele entra sozinho na soma */
	if (len == 1)
		sum += *(unsigned char *)buffer;

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	result = ~sum;
	return (result);
}

/*
** Monta e envia um pacote ICMP Echo Request (o "ping" propriamente
** dito). O pacote é: [ struct icmphdr | payload de ping->payload_size
** bytes ]. Os primeiros bytes do payload guardam um struct timeval
** com o instante do envio — é assim que a gente mede o RTT depois,
** sem precisar confiar num relógio "guardado" no processo (ver
** listen_packet_reply, que lê esse timestamp de volta do payload).
*/
void send_icmp_packet(t_ping *ping, int seq)
{
	/* buffer dimensionado pro maior payload possível (-s aceita até
	** ICMP_MAX_PAYLOAD bytes); só os primeiros packet_len bytes são
	** de fato usados/enviados. */
	char packet[sizeof(struct icmphdr) + ICMP_MAX_PAYLOAD];
	size_t packet_len = sizeof(struct icmphdr) + ping->payload_size;
	struct timeval tv;
	ssize_t bytes_sent;

	memset(packet, 0, packet_len);

	struct icmphdr *icmp = (struct icmphdr *)packet;

	icmp->type = ICMP_ECHO;      /* tipo 8 = Echo Request */
	icmp->code = 0;
	/* id identifica esse processo entre outros pings rodando ao mesmo
	** tempo na máquina (usamos o PID); sequence identifica esse pacote
	** dentro da sessão. O kernel/host remoto ecoa os dois de volta na
	** resposta sem alterar. */
	icmp->un.echo.id = getpid() & 0xFFFF;
	icmp->un.echo.sequence = seq;
	icmp->checksum = 0;          /* precisa estar zerado antes de calcular */

	gettimeofday(&tv, NULL);
	memcpy(packet + sizeof(struct icmphdr), &tv, sizeof(tv));

	icmp->checksum = calc_checksum(packet, packet_len);

	bytes_sent = sendto(ping->socket.fd, packet, packet_len, 0, (struct sockaddr *)&ping->socket.remote_addr, ping->socket.addr_len);
	if (bytes_sent < 0)
		perror("sendto failed");
	else
		ping->stats.transmitted++;
}

/*
** Sob -v, mostra qualquer resposta ICMP que não seja um Echo Reply
** (por exemplo um roteador respondendo "Destination Unreachable" ou
** "Time Exceeded" quando o TTL, via -T, expira no meio do caminho).
*/
static void print_error_reply(struct icmphdr *icmp_hdr, struct sockaddr_in *from)
{
	const char *type_str;

	if (icmp_hdr->type == ICMP_DEST_UNREACH)
		type_str = "Destination Unreachable";
	else if (icmp_hdr->type == ICMP_TIME_EXCEEDED)
		type_str = "Time Exceeded";
	else if (icmp_hdr->type == ICMP_SOURCE_QUENCH)
		type_str = "Source Quench";
	else if (icmp_hdr->type == ICMP_REDIRECT)
		type_str = "Redirect";
	else if (icmp_hdr->type == ICMP_PARAMETERPROB)
		type_str = "Parameter Problem";
	else
		type_str = "Unknown ICMP type";

	printf("From %s: %s (type=%d code=%d)\n", inet_ntoa(from->sin_addr),
		type_str, icmp_hdr->type, icmp_hdr->code);
}

/*
** Espera (até SO_RCVTIMEO, configurado em config_addr) por uma
** resposta ICMP e a processa. Chamada uma vez por iteração do
** main_loop, depois de cada send_icmp_packet.
*/
void listen_packet_reply(t_ping *ping)
{
	char buffer[IP_MAXPACKET];
	struct sockaddr_in from;
	socklen_t from_len = sizeof(from);
	ssize_t bytes_received;
	struct ip *ip_hdr;
	int ip_hdr_len;
	struct icmphdr *icmp_hdr;

	/*
	** Num socket raw de ICMP no Linux, o kernel entrega o datagrama
	** COM o cabeçalho IP na frente (diferente de um socket UDP normal,
	** onde você só vê o payload). Por isso precisamos "pular" o
	** cabeçalho IP manualmente pra chegar no cabeçalho ICMP.
	** Se recvfrom estourar o timeout (nenhuma resposta chegou), ela
	** retorna -1 e simplesmente saímos — isso é o que caracteriza
	** "pacote perdido" pro resto do programa.
	*/
	bytes_received = recvfrom(ping->socket.fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);
	if (bytes_received < 0)
		return ;
	/* checagem de tamanho ANTES de ler ip_hdr->ip_hl — sem isso,
	** um datagrama malformado/curto faria a gente ler lixo de memória
	** não inicializada e calcular um ip_hdr_len sem sentido. */
	if (bytes_received < (ssize_t)sizeof(struct ip))
		return ;

	ip_hdr = (struct ip *)buffer;
	/* ip_hl vem em "palavras de 32 bits" (4 bytes cada), por isso *4
	** pra converter pro tamanho em bytes do cabeçalho IP (que pode
	** variar por causa de opções IP, daí não dar pra usar sizeof(struct ip)
	** direto). */
	ip_hdr_len = ip_hdr->ip_hl * 4;

	if (bytes_received < ip_hdr_len + (ssize_t)sizeof(struct icmphdr))
		return ;

	icmp_hdr = (struct icmphdr *)(buffer + ip_hdr_len);

	if (icmp_hdr->type == ICMP_ECHOREPLY)
	{
		struct timeval tv_sent;
		struct timeval tv_recv;
		double rtt;

		/* pode haver outros processos fazendo ping na mesma máquina;
		** o id (nosso PID) garante que só processamos ecos que nós
		** mesmos enviamos. */
		if (icmp_hdr->un.echo.id != (getpid() & 0xFFFF))
			return ;
		if (bytes_received < ip_hdr_len + (ssize_t)sizeof(struct icmphdr) + (ssize_t)sizeof(tv_sent))
			return ;

		/* lê de volta o timestamp que a gente mesmo colocou no payload
		** em send_icmp_packet — assim o RTT é calculado com o instante
		** real de envio DESSE pacote específico, e não com o relógio
		** da última chamada de main_loop (o que seria impreciso se, no
		** futuro, houvesse mais de um pacote "em voo" ao mesmo tempo). */
		memcpy(&tv_sent, buffer + ip_hdr_len + sizeof(struct icmphdr), sizeof(tv_sent));
		gettimeofday(&tv_recv, NULL);
		rtt = (tv_recv.tv_sec - tv_sent.tv_sec) * 1000.0 + (tv_recv.tv_usec - tv_sent.tv_usec) / 1000.0;

		/* acumula as estatísticas usadas no resumo final (print_stats
		** em main.c): min/max/soma/soma dos quadrados (pra desvio
		** padrão via mdev). */
		ping->stats.received++;
		if (ping->stats.received == 1 || rtt < ping->stats.rtt_min)
			ping->stats.rtt_min = rtt;
		if (ping->stats.received == 1 || rtt > ping->stats.rtt_max)
			ping->stats.rtt_max = rtt;
		ping->stats.rtt_sum += rtt;
		ping->stats.rtt_sum2 += rtt * rtt;

		printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
			(long)(bytes_received - ip_hdr_len), inet_ntoa(from.sin_addr),
			icmp_hdr->un.echo.sequence, ip_hdr->ip_ttl, rtt);
	}
	else if (ping->verbose)
		print_error_reply(icmp_hdr, &from);
}
