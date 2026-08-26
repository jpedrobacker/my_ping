/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbergfel <jbergfel@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:21:38 by jbergfel          #+#    #+#             */
/*   Updated: 2026/08/26 00:00:00 by jbergfel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parse.h"

static int parse_long(const char *arg, const char *name, long min, long max, long *out)
{
	char *end;
	long value;

	errno = 0;
	value = strtol(arg, &end, 10);
	if (end == arg || *end != '\0' || errno == ERANGE || value < min || value > max)
		return (printf("Invalid value for %s: %s\n", name, arg), 1);
	*out = value;
	return (0);
}

static int parse_double(const char *arg, const char *name, double min, double *out)
{
	char *end;
	double value;

	errno = 0;
	value = strtod(arg, &end);
	if (end == arg || *end != '\0' || errno == ERANGE || value < min)
		return (printf("Invalid value for %s: %s\n", name, arg), 1);
	*out = value;
	return (0);
}

static int apply_value_flag(char c, const char *value, t_ping *ping)
{
	long lval;

	if (c == 'T')
	{
		if (parse_long(value, "-T/--ttl", 1, 255, &lval))
			return (1);
		ping->ttl = (int)lval;
	}
	else if (c == 'W')
	{
		if (parse_double(value, "-W", 0.0, &ping->timeout_sec))
			return (1);
	}
	else if (c == 'w')
	{
		if (parse_double(value, "-w", 0.0, &ping->deadline_sec))
			return (1);
	}
	else
	{
		if (parse_long(value, "-s", 0, ICMP_MAX_PAYLOAD, &lval))
			return (1);
		if ((size_t)lval < sizeof(struct timeval))
			return (printf("-s value too small, must be at least %zu\n", sizeof(struct timeval)), 1);
		ping->payload_size = (size_t)lval;
	}
	return (0);
}

static int is_value_flag(char c)
{
	return (c == 'T' || c == 'W' || c == 'w' || c == 's');
}

static int parse_short_cluster(int ac, char **av, int *i, t_ping *ping)
{
	int j = 1;
	char c;
	const char *value;

	while (av[*i][j] != '\0')
	{
		c = av[*i][j];
		if (c == 'v')
			ping->verbose = true;
		else if (c == 'n')
			ping->numeric = true;
		else if (is_value_flag(c))
		{
			value = av[*i] + j + 1;
			if (*value == '\0')
			{
				if (*i + 1 >= ac)
					return (printf("Option -%c requires a value!\n", c), 1);
				(*i)++;
				value = av[*i];
			}
			return (apply_value_flag(c, value, ping));
		}
		else
			return (printf("Wrong type of flag!\n"), 1);
		j++;
	}
	return (0);
}

static int parse_long_option(int ac, char **av, int *i, t_ping *ping)
{
	if (strcmp(av[*i], "--ttl") == 0)
	{
		if (*i + 1 >= ac)
			return (printf("Option --ttl requires a value!\n"), 1);
		(*i)++;
		return (apply_value_flag('T', av[*i], ping));
	}
	if (strncmp(av[*i], "--ttl=", 6) == 0)
		return (apply_value_flag('T', av[*i] + 6, ping));
	return (printf("Wrong type of flag!\n"), 1);
}

int parse_arguments(int ac, char **av, t_ping *ping)
{
	int i = 1;

	while (i < ac)
	{
		if (strcmp(av[i], "-?") == 0)
			return (ping->help = true, 0);
		else if (av[i][0] == '-' && av[i][1] == '-')
		{
			if (parse_long_option(ac, av, &i, ping))
				return (1);
		}
		else if (av[i][0] == '-' && av[i][1] != '\0')
		{
			if (parse_short_cluster(ac, av, &i, ping))
				return (1);
		}
		else
		{
			if (ping->hostname != NULL)
				return (printf("Hostname already filled!\n"), 1);
			ping->hostname = av[i];
		}
		i++;
	}
	if (ping->hostname == NULL)
		return (printf("No hostname found!\n"), 1);
	return (0);
}
