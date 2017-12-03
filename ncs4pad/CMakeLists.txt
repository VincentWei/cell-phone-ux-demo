mg_add_sub_source_dirs (
    include
    src
    )

mg_commit_source_files ()

if (enable_ncs4pad_test)
    set (ncs4pad_source_list ${_source_list_})
    add_subdirectory (test)
endif (enable_ncs4pad_test)
