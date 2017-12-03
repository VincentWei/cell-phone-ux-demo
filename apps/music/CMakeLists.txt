#append all files to _source_list_.
#add_all_source_file ()
#mg_add_all_source_files()
list (APPEND apps_music_sources 
    ActivityAlbum.cc
    SongTableViewPiece.cc
    AlbumTableViewPiece.cc
    )
mg_add_source_files (${apps_music_sources})
mg_commit_source_files()

