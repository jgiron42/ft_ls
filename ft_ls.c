#include "ft_ls.h"

char	*getlink(char *name)
{
	static char link[PATH_MAX + 1];
	ssize_t ret;
	if ((ret = readlink(name, link, PATH_MAX)) == -1)
	{
		ft_fprintf(ft_stderr, "%s: cannot read symbolic link '%s': %m\n", config.program_name, name);
		return NULL;
	}
	link[ret] = 0;
	return (link);
}

status	ft_ls_file(t_file f, dir_stats *d, container *current_path)
{
	(void)config;
	if (config.flags['i'])
		ft_printf("%*s ", d->inode, f.inode);
	if (config.flags['s'])
		ft_printf("%*s ", d->blocks, f.blocks);
	if (config.flags['l'] && d)
	{
		ft_printf("%-*s ", d->mode, f.mode);
		ft_printf("%*s ",d->nlink, f.nlink);
		if (!config.flags['g'])
			ft_printf("%*s ",d->uid * (f.uid_is_int ? 1 : -1), f.uid);
		if (!config.flags['o'])
			ft_printf("%*s ",d->gid * (f.gid_is_int ? 1 : -1), f.gid);
		if ((S_ISCHR(f.lstat.st_mode) || S_ISBLK(f.lstat.st_mode)))
			ft_printf("%*s, %*s ",d->rdev, f.special.rdev, d->dev, f.special.dev);
		else
			ft_printf("%*s ",d->size, f.reg.size);
		ft_printf("%12s ",f.time);
	}
	if ((config.flags['l']) && S_ISLNK(f.lstat.st_mode) && !config.flags['L']) // TODO: better
	{
		path_push(current_path, f.name);
		char *link = getlink(ft_string_c_str(current_path)); // TODO: less vector operations
		path_pop(current_path);
		if (link)
			ft_printf("%s -> %s", f.name, link);
		else
			ft_printf("%s", f.name);
	}
	else
		ft_printf("%s", f.name);
	if (f.Fchar)
		ft_putc(f.Fchar, ft_stdout);
	ft_putc('\n', ft_stdout);
	return OK;
}

status ls_all_dir(container *current_path, container *set, container *vectors, size_t depth)
{
	status ret = OK;

	dir_stats tmp = DEFAULT_DIR;
	if (vectors->size <= depth) // create dir_stat
	{
#ifdef VECTOR_STORAGE
		if (ft_vector(T_FILE_METADATA, &tmp.set) != OK)
			return FATAL;
		if (ft_bheap(T_FILE_METADATA, &tmp.tmp_set) != OK)
			return FATAL;
#else
		if (ft_btree(T_FILE_METADATA, &tmp.set) != OK)
			return FATAL;
#endif
		if (ft_vector_push_back(vectors, &tmp) != OK)
			return FATAL;
	}
	for_in(it, *set)
	{
		t_file *f = (t_file *)it.metadata.dereference(&it);
		if (*ft_string_c_str(current_path) && (!ft_strcmp(f->name, "..") || !ft_strcmp(f->name, ".")))
			continue;
		if (f->is_dir) {
			dir_stats *current_dir = ft_vector_at(vectors, depth);
			if (f->name[0] == '/')
				ft_string_clear(current_path);
			path_push(current_path, f->name);
			if(S_ISLNK(f->lstat.st_mode) && config.flags['L'])
			{
				char *link = getlink(ft_string_c_str(current_path));
				if (!link)
					goto end_dir;
				if (!ft_strncmp(link, ft_string_c_str(current_path), ft_strlen(link)))
				{
					ft_fprintf(ft_stderr, "%s: %s: not listing already-listed directory\n", config.program_name, ft_string_c_str(current_path));
					goto end_dir;
				}
			}
			SWITCH_STATUS(get_dir(current_path, current_dir),, { path_pop(current_path); ret = KO; continue; }, { return FATAL; })
			SWITCH_STATUS(ft_ls_dir(current_path, current_dir, vectors, depth),, { ret = KO; }, { return FATAL; })
			end_dir:
			if (f->name[0] == '/')
				ft_string_clear(current_path);
			else
				path_pop(current_path);
			// reset dir_stat:
			tmp = DEFAULT_DIR;
			tmp.set = ((dir_stats *)ft_vector_at(vectors, depth))->set;
			tmp.set.clear(&tmp.set);
#ifdef VECTOR_STORAGE
			tmp.tmp_set = ((dir_stats *)ft_vector_at(vectors, depth))->tmp_set;
			tmp.tmp_set.clear(&tmp.tmp_set);
#endif
			*((dir_stats *)ft_vector_at(vectors, depth)) = tmp;
		}
	}
	return ret;
}

status	ft_ls_dir(container *current_path, dir_stats *dir, container *vectors, size_t depth)
{
	status ret = OK;
	if (config.flags['l'] || config.flags['s'])
		ft_printf("total %lu\n", dir->total_blocks);
#ifdef BHEAP_STORAGE
	while (dir->set.size > 0)
	{
		ft_ls_file(*((t_file*)ft_bheap_top(&dir->set)), dir, current_path);

		ft_bheap_pop(&dir->set);
	}
#elif defined(VECTOR_STORAGE)
//printf("cousdcoasudfsdoufsaduofa\n");
	for_in(it, dir->set)
	{
//		printf(">>>> %s\n", ((t_file*)ft_vector_iterator_reference(&it))->name);
		ft_ls_file(*(t_file*)ft_vector_iterator_dereference(&it), dir, current_path);
	}

#else
	for_in(it, dir->set)
		ft_ls_file(*(t_file*)it.metadata.dereference(&it), dir, current_path);
#endif
	if (config.flags['R'])
		SWITCH_STATUS(ls_all_dir(current_path, &dir->set, vectors, depth + 1), , ret = KO, return FATAL);
	return ret;
}