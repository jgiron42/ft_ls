#include "ft_ls.h"

status	get_size(t_file *current, dir_stats *dir)
{
	register int tmp;
	if (S_ISBLK(current->lstat.st_mode) || S_ISCHR(current->lstat.st_mode))
	{
		tmp = ft_sprintf(current->special.dev, "%u", minor(current->lstat.st_rdev));
		if (tmp < 0)
			return FATAL;
		if (tmp > dir->dev)
			dir->dev = tmp;
		tmp = ft_sprintf(current->special.rdev, "%u", major(current->lstat.st_rdev));
		if (tmp < 0)
			return FATAL;
		if (tmp > dir->rdev)
			dir->rdev = tmp;
	}
	else
	{
		tmp = ft_sprintf(current->reg.size, "%lu", current->lstat.st_size);
		if (tmp < 0)
			return FATAL;
		if (tmp > dir->size)
			dir->size = tmp;
	}
	return OK;
}

status get_id_cache(size_t id,  int access_type, char *dst, int *len, bool *is_num)
{
	static container table[2] = {{}, {}};
	static int init = 0;
	errno = 0;
	if (access_type == DESTROY_ID_TABLE)
	{
		if (!init)
			return OK;
		table[0].destroy(table);
		table[1].destroy(table + 1);
		table[0] = (container){};
		table[1] = (container){};
		init = 0;
		return OK;
	}
	if (!init)
	{
		if (ft_btree(POINTER_TYPE(id_table_entry), table) != OK)
			return FATAL;
		table->value_type_metadata.compare = &id_table_entry_compare;
		if (ft_btree(POINTER_TYPE(id_table_entry), table + 1) != OK)
			return FATAL;
		table[1].value_type_metadata.compare = &id_table_entry_compare;
		init = 1;
	}
	container *current_table = table + access_type;
	iterator it = current_table->metadata.container.associative.insert(current_table, (id_table_entry[1]){{.id = (size_t)id, .len = -1}});
	if (!it.metadata.compare(it.metadata, &it, (iterator[1]){current_table->metadata.container.end(current_table)}))
		return FATAL;
	id_table_entry *ptr = ft_btree_iterator_dereference(&it);
	if (ptr->len == -1) {
		char *tmp = NULL;
		if (access_type == GET_GID && !config.flags['n']) {
			register struct group *grp = getgrgid(id);
			if (grp)
				tmp = grp->gr_name;
		} else if (access_type == GET_UID && !config.flags['n']) {
			register struct passwd *pwd = getpwuid(id);
			if (pwd)
				tmp = pwd->pw_name;
		}
		if (tmp) {
			ft_strcpy(ptr->s, tmp);
			ptr->len = (int) ft_strlen(tmp);
			ptr->is_num = false;
		} else {
			ptr->len = ft_sprintf(ptr->s, "%zu", id);
			ptr->is_num = true;
		}
	}
	ft_strcpy(dst, ptr->s);
	*len = ptr->len;
	*is_num = ptr->is_num;
	return OK;
}

status	get_uid(t_file *current, dir_stats *dir)
{
	int tmp;
	int ret = get_id_cache(current->lstat.st_uid, GET_UID, current->uid, &tmp, &current->uid_is_int);
	if (tmp > dir->uid)
		dir->uid = tmp;
	return ret;
}

status	get_gid(t_file *current, dir_stats *dir)
{
	int tmp;
	int ret = get_id_cache(current->lstat.st_gid, GET_GID, current->gid, &tmp, &current->gid_is_int);
	if (tmp > dir->gid)
		dir->gid = tmp;
	return ret;
}

status	get_nlink(t_file *current, dir_stats *dir)
{
	register int tmp;
	tmp = ft_sprintf(current->nlink, "%lu", current->lstat.st_nlink);
	if (tmp < 0)
		return FATAL;
	if (tmp > dir->nlink)
		dir->nlink = tmp;
	return OK;
}

status	get_inode(t_file *current, dir_stats *dir)
{
	register int tmp;
	tmp = ft_sprintf(current->inode, "%lu", current->lstat.st_ino);
	if (tmp < 0)
		return FATAL;
	if (tmp > dir->inode)
		dir->inode = tmp;
	return OK;
}

status	get_blocks(t_file *current, dir_stats *dir)
{
	register int tmp;
	tmp = ft_sprintf(current->blocks, "%lu", CONV_BLK(current->lstat.st_blocks, config.block_size));
	if (tmp < 0)
		return FATAL;
	if (tmp > dir->blocks)
		dir->blocks = tmp;
	return OK;
}

status	get_time(t_file *current)
{
	//        0123456789012345678901234
	// ctime: DDD MMM dd hh:mm:ss yyyy\n
	// <6 months: MMM dd hh:mm
	// >6 months: MMM dd  yyyy
	char *timestr;
	time_t	t;

	if (config.flags['c'])
		t = current->lstat.st_ctime;
	else if (config.flags['u'])
		t = current->lstat.st_atime;
	else
		t = current->lstat.st_mtime;
	timestr = ctime(&t);
	if (config.current_time > t)
		config.current_time = time(NULL);
	current->time[0] = timestr[4];
	current->time[1] = timestr[5];
	current->time[2] = timestr[6];
	current->time[3] = ' ';
	current->time[4] = timestr[8];
	current->time[5] = timestr[9];
	current->time[6] = ' ';
	if (config.current_time - t < 31556952 / 2) // value for a year stolen from https://github.com/wertarbyte/coreutils/blob/master/src/ls.c
	{
		current->time[7] = timestr[11];
		current->time[8] = timestr[12];
		current->time[9] = ':';
		current->time[10] = timestr[14];
		current->time[11] = timestr[15];
		current->time[12] = 0;
	}
	else
	{
		current->time[7] = ' ';
		current->time[8] = timestr[20];
		current->time[9] = timestr[21];
		current->time[10] = timestr[22];
		current->time[11] = timestr[23];
		current->time[12] = 0;
	}
	return OK;
}

status	get_mode(t_file *current, dir_stats *dir, char *path)
{
	if (!current->stat_error) {
		if (!S_ISLNK(current->lstat.st_mode)) {
			ssize_t ret = getxattr(path, XATTR_NAME_POSIX_ACL_ACCESS, NULL, 0);
			if (((ret < 0 && errno == ENODATA) || ret == 0) && S_ISDIR (current->lstat.st_mode))
				ret = getxattr(path, XATTR_NAME_POSIX_ACL_DEFAULT, NULL, 0);
			current->acl = ret > 0;
		}

		if (S_ISDIR(current->lstat.st_mode))
			current->mode[0] = 'd';
		else if (S_ISLNK(current->lstat.st_mode))
			current->mode[0] = 'l';
		else if (S_ISCHR(current->lstat.st_mode))
			current->mode[0] = 'c';
		else if (S_ISBLK(current->lstat.st_mode))
			current->mode[0] = 'b';
		else if (S_ISFIFO(current->lstat.st_mode))
			current->mode[0] = 'p';
		else if (S_ISSOCK(current->lstat.st_mode))
			current->mode[0] = 's';
		else
			current->mode[0] = '-';
		current->mode[1] = (current->lstat.st_mode & S_IRUSR) ? 'r' : '-';
		current->mode[2] = (current->lstat.st_mode & S_IWUSR) ? 'w' : '-';
		if (current->lstat.st_mode & S_ISUID)
			current->mode[3] = (current->lstat.st_mode & S_IXUSR) ? 's' : 'S';
		else
			current->mode[3] = (current->lstat.st_mode & S_IXUSR) ? 'x' : '-';
		current->mode[4] = (current->lstat.st_mode & S_IRGRP) ? 'r' : '-';
		current->mode[5] = (current->lstat.st_mode & S_IWGRP) ? 'w' : '-';
		if (current->lstat.st_mode & S_ISGID)
			current->mode[6] = (current->lstat.st_mode & S_IXGRP) ? 's' : 'S';
		else
			current->mode[6] = (current->lstat.st_mode & S_IXGRP) ? 'x' : '-';
		current->mode[7] = (current->lstat.st_mode & S_IROTH) ? 'r' : '-';
		current->mode[8] = (current->lstat.st_mode & S_IWOTH) ? 'w' : '-';
		if ((current->lstat.st_mode & S_ISVTX))
			current->mode[9] = (current->lstat.st_mode & S_IXOTH) ? 't' : 'T';
		else
			current->mode[9] = (current->lstat.st_mode & S_IXOTH) ? 'x' : '-';
		current->mode[10] = current->acl ? '+' : 0;
		current->mode[11] = '\0';
		if (dir->mode < 11)
			dir->mode = current->mode[10] ? 11 : 10;
	}
	else
		switch (current->d_type)
		{
			case DT_DIR:
				current->mode[0] = 'd';
				break;
			case DT_LNK:
			current->mode[0] = 'l';
				break;
			case DT_CHR:
			current->mode[0] = 'c';
				break;
			case DT_BLK:
			current->mode[0] = 'b';
				break;
			case DT_FIFO:
			current->mode[0] = 'p';
				break;
			case DT_SOCK:
			current->mode[0] = 's';
				break;
			case DT_UNKNOWN:
				break;
			case DT_REG:
				current->mode[0] = '-';
				break;
		}
	return OK;
}

status	get_Fchar(char *dst, mode_t mode)
{
	if (config.flags['p'] && S_ISDIR(mode))
		*dst = '/';
	else if (config.flags['F'])
	{
		if (S_ISDIR(mode))
			*dst = '/';
		else if (S_ISFIFO(mode))
			*dst = '|';
		else if (S_ISLNK(mode))
			*dst = '@';
		else if (S_ISREG(mode) &&
				 (S_IXOTH & mode || S_IXGRP & mode || S_IXUSR & mode))
			*dst = '*';
	}
	else
		*dst = 0;
	return OK;
}

