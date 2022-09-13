#include "ft_ls.h"

t_config config = {};
status	ls_arguments(container *dirs, dir_stats *files)
{
	container current_path;
	ft_string(&current_path, "");
	status ret = OK;

	//files
	if (files->set.size > 0)
		for_in(it, files->set)
			SWITCH_STATUS(ft_ls_file(*(t_file*)it.metadata.dereference(&it), files, &current_path),,ret =KO, goto end)

	// directories:
	if (dirs->size > 0)
		SWITCH_STATUS(ls_all_dir(&current_path, dirs), , ret = KO, ret = FATAL)
	end:
	current_path.destroy(&current_path);
	return ret;
}

int main(int argc, char **argv)
{
	dir_stats	files = DEFAULT_DIR;
	container 	dirs = {};

	ft_bzero(config.flags, sizeof(config.flags));
	if (ft_btree(T_FILE_METATYPE, &dirs) != OK)
		return FATAL;
	if (ft_btree(T_FILE_METATYPE, &files) != OK)
	{
		dirs.destroy(&dirs);
		return FATAL;
	}
	dirs.btree.multi = true; // enable multimap
	files.set.btree.multi = true;
	if (parse_command(&dirs, &files, argc, argv) != OK)
	{
		free(config.program_name);
		dirs.destroy(&dirs);
		files.set.destroy(&files.set);
		return (1);
	}
	status ret = ls_arguments(&dirs, &files);
	free(config.program_name);
	dirs.destroy(&dirs);
	files.set.destroy(&files.set);
	ft_fflush(ft_stdout);
	ft_fflush(ft_stderr);
	return (ret == OK ? 0 : 1);
}