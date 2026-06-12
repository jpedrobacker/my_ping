/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:30 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/12 17:18:53 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include <stdio.h>
# include <stdbool.h>
# include <string.h>

typedef struct s_ping
{
	char *hostname;
	bool verbose;
	bool help;

}	t_ping;

int	parse_arguments(int ac, char **av, t_ping *ping);

#endif
