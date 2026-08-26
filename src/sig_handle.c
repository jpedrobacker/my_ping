/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handle.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:49 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/12 19:17:35 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.h"

int g_run;

static void signal_handle(int sig)
{
	(void)sig;
	g_run = 0;
}

void handle_sigint(void)
{
	struct sigaction sig_config = {0};

	sig_config.sa_handler = signal_handle;

	sigaction(SIGINT, &sig_config, NULL);
}

