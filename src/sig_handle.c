/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handle.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:49 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/27 16:20:24 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.h"

void	signal_handle(int sig) {
	extern bool g_run;

	(void)sig;
	g_run = false;

	return;
}

void handle_sigint(int signal)
{
	struct sigaction sig_config = {0};

	sig_config.sa_handler = signal_handle;

	sigaction(SIGINT, &sig_config, NULL);
}

