/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handle.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 15:31:42 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/27 16:18:18 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SIG_HANDLE_H
# define SIG_HANDLE_H

# define _XOPEN_SOURCE 700
# define _POSIX_C_SOURCE 199309L

# include <signal.h>

void	handle_sigint(int signal);

#endif
