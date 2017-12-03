mg_add_source_files(
    mspeedmeter.c
    )

IF (enable_scrollview)
    mg_add_source_files(
        chipmunk-utils.c
        physics-animation.c
        )
ENDIF (enable_scrollview)

mg_commit_source_files()
