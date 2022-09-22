#include "ft_ls.h"
#include <locale.h>

t_config config = {};
status	ls_arguments(container *dirs, dir_stats *files)
{
	container current_path;
	ft_string(&current_path, "");
	status ret = OK;

	//files
	if (files->set.size > 0)
		for_in(it, files->set)
			SWITCH_STATUS(ft_ls_file(*(t_file*)it.metadata.dereference(&it), files, &current_path),,ret = KO, goto end)

	container vectors;
	ft_vector(POINTER_TYPE(dir_stats), &vectors);
	// directories:
	if (dirs->size > 0)
		SWITCH_STATUS(ls_all_dir(&current_path, dirs, &vectors, 0), , ret = KO, ret = FATAL)
	for_val_in(dir_stats *e, vectors)
	{
		e->set.destroy(&e->set);
#ifdef VECTOR_STORAGE
		e->tmp_set.destroy(&e->tmp_set);
#endif
	}
	vectors.destroy(&vectors);
	end:
	current_path.destroy(&current_path);
	return ret;
}

int main(int argc, char **argv) // -Ri /
{
	dir_stats	files = DEFAULT_DIR;
	container 	dirs = {};

	// init stdout and stderr (with a huge buffer):
	ft_setvbuf(ft_stdout, NULL, _IOFBF, 10000);
	ft_setvbuf(ft_stderr, NULL, _IOFBF, 10000);

	// init data_structures:
	ft_bzero(config.flags, sizeof(config.flags));
	if (ft_btree(T_FILE_METADATA, &dirs) != OK)
		return FATAL;
	dirs.btree.multi = true; // enable multimap
	if (ft_btree(T_FILE_METADATA, &files.set) != OK)
	{
		dirs.destroy(&dirs);
		return FATAL;
	}
	files.set.btree.multi = true; // enable multimap

	// parse command line:
	if (parse_command(&dirs, &files, argc, argv) == FATAL)
	{
		free(config.program_name);
		dirs.destroy(&dirs);
		files.set.destroy(&files.set);
		return (1);
	}

	// launch ls:
	status ret = ls_arguments(&dirs, &files);

	// free and end:
	free(config.program_name);
	dirs.destroy(&dirs);
	files.set.destroy(&files.set);
	ft_fclose(ft_stdout);
	ft_fclose(ft_stderr);
	get_id_cache(0, DESTROY_ID_TABLE, NULL, NULL, NULL);
	return (ret == OK ? 0 : 1);
}