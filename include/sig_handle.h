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

void	handle_sigint(int signal);

#endif
