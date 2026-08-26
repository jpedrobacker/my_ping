/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handle.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:49 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 00:00:00 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.h"

/*
** g_run é a flag que mantém o main_loop() (main.c) rodando.
** Precisa ser global porque um signal handler não recebe argumentos
** extras do kernel — a única forma de avisar "pare agora" pro resto
** do programa de dentro de um handler é através de estado compartilhado.
*/
int g_run;

/*
** Essa função roda de forma assíncrona, interrompendo o que o programa
** estava fazendo (ex: bloqueado dentro de recvfrom) no instante em que
** o SIGINT (Ctrl+C) chega. Signal handlers devem fazer o mínimo possível
** e só mexer em coisas seguras no meio de uma instrução — aqui é só
** trocar o valor de uma flag.
** O loop principal percebe g_run == 0 na próxima iteração e sai de
** forma limpa, em vez do processo simplesmente morrer.
*/
static void signal_handle(int sig)
{
	(void)sig;
	g_run = 0;
}

/*
** Registra signal_handle() como o handler do SIGINT usando sigaction
** (a API moderna e portável que substitui o antigo signal()). sigaction
** permite zerar a struct inteira (sig_config = {0}) para que todo campo
** que não nos interessa (flags, máscara de sinais) fique num estado
** bem definido.
*/
void handle_sigint(void)
{
	struct sigaction sig_config = {0};

	sig_config.sa_handler = signal_handle;

	sigaction(SIGINT, &sig_config, NULL);
}
