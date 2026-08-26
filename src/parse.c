/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:38 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/12 16:07:05 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parse.h"


static bool has_char(const char *str, char c)
{
	return (strchr(str, c) != NULL);
}

int parse_arguments(int ac, char **av, t_ping *ping)
{
	int i = 1;
	char *flags = "v?";

	while (i < ac)
	{
		if (av[i][0] == '-')
		{
			if (av[i][1] == 'v')
			{
				if (ping->verbose == false)
					ping->verbose = true;
				else
					return (printf("Flag verbose already called!\n"), 1);
			}
			else if (av[i][1] == '?')
			{
				if (ping->help == false)
					ping->help = true;
				else
					return (printf("Flag help already called!\n"), 1);
			}
			else if (!has_char(flags, av[i][1]))
				return (printf("Wrong type of flag!\n"), 1);
		}
		else
		{
			if (ping->hostname != NULL)
				return (printf("Hostname already filled!\n"), 1);
			ping->hostname = av[i];
		}
		i++;
	}
	if (ping->help == true)
		return (0);
	if (ping->hostname == NULL)
		return (printf("No hostname found!\n"), 1);
	return (0);
}
