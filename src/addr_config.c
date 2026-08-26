/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   addr_config.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:36 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 00:00:00 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/icmp.h"
#include "../include/parse.h"

/*
** Resolve ping->hostname para um endereço IPv4 e abre o socket raw de
** ICMP que o ft_ping vai usar pra enviar/receber. Tudo que o resto do
** programa precisa (fd do socket, sockaddr de destino, string do IP
** pra impressão) é preenchido em `ping` aqui, uma única vez, antes do
** loop de envio/recepção começar.
*/
int config_addr(t_ping *ping)
{
	struct sockaddr_storage sock_config = {0};
	struct addrinfo hints, *res;
	struct sockaddr_in *ipv4;
	int rc;

	res = NULL;

	/*
	** getaddrinfo() é a forma moderna de transformar um hostname OU
	** uma string de IP literal em um ou mais endereços de socket
	** utilizáveis — ele trata os dois casos de forma transparente, por
	** isso o requisito do obrigatório "FQDN ou IP puro" não precisa de
	** tratamento especial aqui.
	** hints diz o que estamos procurando: AF_INET restringe os
	** resultados a IPv4 (o socket raw de ICMP aqui só suporta IPv4,
	** ver README).
	*/
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;

	rc = getaddrinfo(ping->hostname, NULL, &hints, &res);
	if (rc != 0)
	{
		/* gai_strerror transforma o código de erro do getaddrinfo em
		** texto (NÃO usa errno/perror — é um domínio de erro separado,
		** porque falha de DNS não é uma falha de syscall). */
		printf("Invalid IP or Hostname not found: %s\n", gai_strerror(rc));
		return (-1);
	}

	/* res é uma lista encadeada de endereços candidatos; só precisamos
	** do primeiro. Copiamos pra um sockaddr_storage (grande o
	** suficiente pra qualquer família de endereço) pra sobreviver
	** depois do freeaddrinfo() logo abaixo. */
	memcpy((struct sockaddr *)&sock_config, res->ai_addr, res->ai_addrlen);

	/*
	** inet_ntop() converte o endereço binário resolvido de volta pra
	** uma string legível ("93.184.216.34"), usada no cabeçalho
	** "PING host (ip): ..." e guardada em ping->raw_ip. Esse é o ÚNICO
	** lugar onde transformamos o destino em texto — as respostas são
	** casadas e impressas usando essa mesma string numérica, nunca com
	** um segundo lookup de DNS, exatamente o que o subject exige (sem
	** DNS reverso no caminho da resposta).
	*/
	ipv4 = (struct sockaddr_in *)res->ai_addr;
	ping->raw_ip = inet_ntop(AF_INET, &(ipv4->sin_addr), ping->ip_str, sizeof(ping->ip_str));

	ping->socket.addr_len = res->ai_addrlen;

	/* getaddrinfo() aloca a lista encadeada no heap; já copiamos o que
	** precisávamos acima, então ela tem que ser liberada aqui
	** independente do resultado. */
	freeaddrinfo(res);

	/*
	** SOCK_RAW + IPPROTO_ICMP: é isso que torna o ft_ping "raw" —
	** montamos o pacote ICMP inteiro na mão (src/icmp.c) em vez de usar
	** um socket UDP/TCP normal. O kernel ainda escreve o cabeçalho IP
	** pra gente no envio, e (no Linux) devolve esse cabeçalho IP junto
	** no recebimento, por isso listen_packet_reply() precisa pular ele
	** manualmente.
	** Essa chamada exige root ou a capability CAP_NET_RAW — por isso o
	** ft_ping precisa rodar com `sudo`.
	*/
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		printf("Permission denied or cannot open socket\n");
		return (-1);
	}

	/*
	** Bonus -T/--ttl: IP_TTL é uma opção de socket, não algo que
	** escrevemos na mão no pacote — o kernel preenche o campo TTL do
	** cabeçalho IP de todo pacote enviado por esse socket a partir de
	** agora.
	** ping->ttl começa em -1 (ver init_ping em main.c), significando
	** "deixa no padrão do SO", então só mexemos nisso se o usuário
	** realmente passou -T/--ttl.
	*/
	if (ping->ttl > 0 && setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0)
	{
		perror("setsockopt IP_TTL failed");
		close(sockfd);
		return (-1);
	}

	/*
	** Bonus -W: SO_RCVTIMEO faz o recvfrom() (em listen_packet_reply)
	** desistir e retornar -1/EAGAIN depois desse tempo, em vez de
	** bloquear pra sempre quando uma resposta nunca chega — é isso que
	** permite o ft_ping detectar perda de pacote em vez de travar num
	** host morto.
	** ping->timeout_sec é um double (ex: 1.5s); dividir em segundos
	** inteiros + microssegundos é necessário porque struct timeval não
	** tem um único campo de segundos fracionários.
	*/
	struct timeval timeout;
	timeout.tv_sec = (time_t)ping->timeout_sec;
	timeout.tv_usec = (suseconds_t)((ping->timeout_sec - timeout.tv_sec) * 1000000);
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		perror("setsockopt SO_RCVTIMEO failed");
		close(sockfd);
		return (-1);
	}

	ping->socket.fd = sockfd;
	ping->socket.remote_addr = sock_config;

	return (sockfd);
}
