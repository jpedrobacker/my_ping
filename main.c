/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:35 by jbergfel          #+#    #+#             */
/*   Updated: 2026/06/12 17:56:08 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/main.h"

int main(int ac, char **av)
{
	t_ping ping = {0};

	//memset(&ping, 0, sizeof(t_ping));

	if (parse_arguments(ac, av, &ping) != 0)
	{
		printf("Error!\n");
		return (1);
	}

	return (0);
}
