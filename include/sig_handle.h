/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handle.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:42 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/12 19:17:04 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SIG_HANDLE_H
# define SIG_HANDLE_H

# include <signal.h>

/* flag compartilhada entre o signal handler (sig_handle.c) e o
** main_loop (main.c) — ver comentário completo em sig_handle.c. */
extern int g_run;

void	handle_sigint(void);

#endif
