#include "ft_ls.h"

status	get_stat(t_file *current, char *path, bool follow_link)
{
	if (lstat(path, &current->lstat) == -1)
	{
		current->stat_error = true;
		ft_fprintf(ft_stderr, "%s: cannot access '%s': %m\n", config.program_name, path);
		return KO;
	}
	current->real_mode = current->lstat.st_mode;
	if (S_ISLNK(current->real_mode) && follow_link && stat(path, &current->lstat) == -1)
	{
		current->stat_error = true;
		ft_fprintf(ft_stderr, "%s: cannot access '%s': %m\n", config.program_name, path);
	}
	return OK;
}

status init_file(t_file *current, char *name, char *path, dir_stats *dir)
{
	ft_strcpy(current->name, name);
	if (dir && !current->stat_error) {
		if (config.flags['i'])
			SWITCH_STATUS(get_inode(current, dir), , , return FATAL)
		if (config.flags['s'])
			SWITCH_STATUS(get_blocks(current, dir), , , return FATAL)
		if (config.flags['p'] || config.flags['F'])
		{
			if (config.flags['l'])
				SWITCH_STATUS(get_Fchar(&current->Fchar, current->lstat.st_mode), , , return FATAL)
			else
				SWITCH_STATUS(get_Fchar(&current->Fchar, current->real_mode), , , return FATAL)
		}
		if (config.flags['l']) {
			SWITCH_STATUS(get_mode(current, dir, path), , , return FATAL)
			SWITCH_STATUS(get_nlink(current, dir), , , return FATAL);
			if (!config.flags['g'])
				SWITCH_STATUS(get_uid(current, dir), , , return FATAL)
			if (!config.flags['o'])
				SWITCH_STATUS(get_gid(current, dir), , , return FATAL)
			SWITCH_STATUS(get_size(current, dir), , , return FATAL)
			SWITCH_STATUS(get_time(current), , , return FATAL)
		}
		dir->total_blocks += current->lstat.st_blocks;
	}
	return OK;
}

status get_dir(container *dirname, dir_stats *dir)
{
	struct dirent *elem;
	DIR *dirp;
	dirp = opendir(ft_string_c_str(dirname));
	if (!dirp)
	{
		ft_fprintf(ft_stderr, "%s: cannot open directory '%s': %m\n", config.program_name, ft_string_c_str(dirname));
		return KO;
	}
	if (config.multiple || config.flags['R'])
		ft_printf("%s%s:\n", ft_ftell(ft_stdout) > 0 ? "\n" : "", ft_string_c_str(dirname));
	if (ft_btree(T_FILE_METATYPE, &dir->set) != OK)
	{
		closedir(dirp);
		return FATAL;
	}
	dir->set.btree.multi = true;
	errno = 0;
	while ((elem = readdir(dirp)))
	{
		if (elem->d_name[0] == '.' && !config.flags['a'])
			continue;
		t_file current = DEFAULT_FILE;
		path_push(dirname, elem->d_name);
		if (elem->d_type == DT_UNKNOWN)
		{
			if (lstat(ft_string_c_str(dirname), &current.lstat) == -1) {
//				ft_fprintf(ft_stderr, "%s: cannot access '%s': %m\n", config.program_name, ft_string_c_str(dirname));
//				current.stat_error = true;
//				current.name[0] = '\0';
//				closedir(dirp);
//				return KO;
			}
		}
		get_stat(&current, ft_string_c_str(dirname), config.flags['L']);
		SWITCH_STATUS(init_file(&current, elem->d_name, ft_string_c_str(dirname), dir), , ;, closedir(dirp);return FATAL);
		path_pop(dirname);

		iterator end = ft_btree_end(&dir->set);
		iterator inserted = ft_btree_insert_val(&dir->set, &current);
		if (ft_btree_iterator_compare(dir->set.value_type_metadata, &end, &inserted) == 0)
		{
			closedir(dirp);
			return (FATAL);
		}
		errno = 0;
	}
	closedir(dirp);
	if (errno)
	{
		ft_fprintf(ft_stderr, "%s: reading directory '%s': %m\n", config.program_name, ft_string_c_str(dirname));
		return OK; // a voir
	}

	if (dir->size > dir->rdev + dir->dev + 2)
		dir->rdev = dir->size - dir->dev - 2;
	else if (dir->rdev && dir->dev)
		dir->size = dir->rdev + dir->dev + 2;
	dir->total_blocks = CONV_BLK(dir->total_blocks, config.block_size);

	// changing the data structure to vector (faster iterator) TODO: benchmark
	container vector;
	ft_vector(T_FILE_METATYPE, &vector);
	ft_vector_insert_range(&vector, vector.begin(&vector), dir->set.begin(&dir->set), dir->set.end(&dir->set));
	dir->set.destroy(&dir->set);
	dir->set = vector;
	return OK;
}