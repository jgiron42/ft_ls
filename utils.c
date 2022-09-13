#include "ft_ls.h"

void	my_perror(char *prefix)
{
	ft_fprintf(ft_stderr, "%s: %s: %m\n", config.program_name, prefix);
}

int compare_time(struct timespec l, struct timespec r)
{
	long int ret;
	if (l.tv_sec == r.tv_sec)
		ret =  (l.tv_nsec - r.tv_nsec);
	else
		ret =  (l.tv_sec - r.tv_sec);
	return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

int	t_file_compare(type_metadata prop, void *l, void *r)
{
	(void)prop;
	long int	ret;
	if (config.flags['t'] || ((config.flags['c'] || config.flags['u']) && !(config.flags['l']))) {
		if (config.flags['c'])
			ret = compare_time((*(t_file **)r)->lstat.st_ctim, (*(t_file **)l)->lstat.st_ctim);
		else if (config.flags['u'])
			ret = compare_time((*(t_file **)r)->lstat.st_atim, (*(t_file **)l)->lstat.st_atim);
		else
			ret = compare_time((*(t_file **)r)->lstat.st_mtim, (*(t_file **)l)->lstat.st_mtim);
		if (!ret)
			 ret = ft_strcmp((*(t_file **)l)->name, (*(t_file **)r)->name);
	}
	else
		ret = ft_strcmp((*(t_file **)l)->name, (*(t_file **)r)->name);
	if (config.flags['r'])
		ret *= -1;
	return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}