
include_directories(AFTER "../ncs4pad/include")
include_directories(AFTER "../ncs4pad/include/pieces")

if (enable_opengles)
    set (gl_activities coverflow-gl flight-gl)
endif (enable_opengles)


mg_add_sub_source_dirs (sample-activity sample-navigation
    common screen-lock phone-call music photo camera calc dial address-book sys-setting sms callhistory
    ${gl_activities}
    )
mg_commit_source_files()
