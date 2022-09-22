#include "ft_ls.h"

int compare_time(struct timespec l, struct timespec r)
{
	long int ret;
	if (l.tv_sec == r.tv_sec)
		ret =  (l.tv_nsec - r.tv_nsec);
	else
		ret =  (l.tv_sec - r.tv_sec);
	return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

int id_table_entry_compare(type_metadata prop, void *l, void *r)
{
	(void)prop;
	id_table_entry *el = (*(id_table_entry **)l);
	id_table_entry *er = (*(id_table_entry **)r);
	return ((int)el->id - (int)er->id);
}

int	t_file_compare(type_metadata prop, void *l, void *r)
{
	(void)prop;
	long int	ret;
	t_file		*fr = (*(t_file **)r);
	t_file		*fl = (*(t_file **)l);

	if (config.flags['t'] || ((config.flags['c'] || config.flags['u']) && !(config.flags['l']))) {
		if (config.flags['c'])
			ret = compare_time(fr->lstat.st_ctim, fl->lstat.st_ctim);
		else if (config.flags['u'])
			ret = compare_time(fr->lstat.st_atim, fl->lstat.st_atim);
		else
			ret = compare_time(fr->lstat.st_mtim, fl->lstat.st_mtim);
		if (!ret)
#ifdef USE_LOCALES
			 ret = strcoll(fl->name, fr->name);
#else
			 ret = ft_strcmp(fl->name, fr->name);
#endif
	}
	else
#ifdef USE_LOCALES
		ret = strcoll(fl->name, fr->name);
#else
		ret = ft_strcmp(fl->name, fr->name);
#endif
	if (config.flags['r'])
		ret *= -1;
	return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

#ifdef USE_LOCALES
void __attribute__((constructor)) init_locales(void)
{
	char *tmp = ft_getenv("LC_ALL");
	if (!tmp)
		tmp = ft_getenv("LC_COLLATE");
	if (!tmp)
		tmp = ft_getenv("LANG");
	if (tmp)
		setlocale(LC_COLLATE, tmp);
}
#endif