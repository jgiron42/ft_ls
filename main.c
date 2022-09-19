#include "ft_ls.h"
#include <locale.h>

t_config config = {};
status	ls_arguments(container *dirs, dir_stats *files)
{
	container current_path;
	ft_string(&current_path, "");
	status ret = OK;

//	char *tmp = ft_getenv("LC_COLLATE");
//	if (tmp)
//		setlocale(LC_COLLATE, tmp);
	//files
	if (files->set.size > 0)
		for_in(it, files->set)
			SWITCH_STATUS(ft_ls_file(*(t_file*)it.metadata.dereference(&it), files, &current_path),,ret = KO, goto end)

	container vectors;
	ft_vector(POINTER_TYPE(dir_stats), &vectors);
	// directories:
	if (dirs->size > 0)
		SWITCH_STATUS(ls_all_dir(&current_path, dirs, &vectors, 0), , ret = KO, ret = FATAL)
	vectors.destroy(&vectors);
	end:
	current_path.destroy(&current_path);
	return ret;
}

int main(int argc, char **argv)
{
	dir_stats	files = DEFAULT_DIR;
	container 	dirs = {};

	ft_bzero(config.flags, sizeof(config.flags));
	if (ft_btree(ATOMIC_TYPE, &dirs) != OK)
		return FATAL;
	dirs.metadata.compare = &t_file_compare;
	dirs.value_type_metadata.size = sizeof(t_file);
	dirs.value_type_size = sizeof(t_file);
	dirs.btree.multi = true; // enable multimap
	if (ft_btree(ATOMIC_TYPE, &files.set) != OK)
	{
		dirs.destroy(&dirs);
		return FATAL;
	}
	files.set.metadata.compare = &t_file_compare;
	files.set.value_type_metadata.size = sizeof(t_file);
	files.set.value_type_size = sizeof(t_file);
	files.set.btree.multi = true; // enable multimap
	if (parse_command(&dirs, &files, argc, argv) == FATAL)
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