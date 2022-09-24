#include "ft_ls.h"
#include <locale.h>

t_config config = {};
status	ls_arguments(container *dirs, dir_stats *files)
{
	container current_path;
	if (ft_string(&current_path, "") != OK)
		return FATAL;
	status ret = OK;

	//files
	if (files->set.size > 0)
		for_in(it, files->set)
			SWITCH_STATUS(ft_ls_file(*(t_file*)it.metadata.dereference(&it), files, &current_path),,ret = KO, goto end)

	container vectors;
	if (ft_vector(POINTER_TYPE(dir_stats), &vectors) != OK)
	{
		ret = FATAL;
		goto end;
	}
	// directories:
	if (dirs->size > 0)
		SWITCH_STATUS(ls_all_dir(&current_path, dirs, &vectors, 0), , ret = KO, ret = FATAL;)
	for_val_in(dir_stats *e, vectors)
	{
		e->set.destroy(&e->set);
#ifdef VECTOR_STORAGE
		e->tmp_set.destroy(&e->tmp_set);
#endif
	}
	end:
	vectors.destroy(&vectors);
	current_path.destroy(&current_path);
	return ret;
}

int main(int argc, char **argv) // -Ri /
{
	dir_stats	files = DEFAULT_DIR;
	container 	dirs = {};
	status ret = FATAL;

	// init data_structures:
	if (ft_vector(T_FILE_METADATA, &dirs) != OK ||
		ft_vector(T_FILE_METADATA, &files.set) != OK)
		goto end;

	// init stdout and stderr (with a huge buffer):
	if (ft_setvbuf(ft_stdout, NULL, _IOFBF, 10000) ||
		ft_setvbuf(ft_stderr, NULL, _IOFBF, 10000))
		goto end;

	ret = OK;
	// parse command line:
	SWITCH_STATUS(parse_command(&dirs, &files, argc, argv),,ret = KO, ret = FATAL; goto end;)
		// launch ls:
	if (ls_arguments(&dirs, &files) != OK)
		ret = KO;

	// free and end:
	end:
	dirs.destroy(&dirs);
	files.set.destroy(&files.set);
	ft_fclose(ft_stdout);
	ft_fclose(ft_stderr);
	get_id_cache(0, DESTROY_ID_TABLE, NULL, NULL, NULL);
	return (ret == OK ? 0 : 1);
}