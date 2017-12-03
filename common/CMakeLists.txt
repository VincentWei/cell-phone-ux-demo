#append source to _sources_list_
list (APPEND common_sources
    utility.c
    Activity.cc
    NCSActivity.cc
    ActivityStack.cc
    DeviceSkin.c
    GraphicsView.cc
    StateMachine.cc
    TransitionStandStill.cc
    TransitionAnimationFinished.cc
    TransitionHover.cc
    TransitionNoInput.cc
    TimeService.cc
    SettingService.cc
    ContentValues.cc
    ContentProvider.cc
    SQLiteDB.cc
    SettingsProvider.cc
    SQLiteCursor.cc
    ContentResolver.cc
    ContactsProvider.cc
    SimpleProviderFactory.cc
    SmsProvider.cc
    SmsService.cc
    ContactService.cc
    MediaStoreService.cc
    MediaStoreProvider.cc
    CallLogProvider.cc
    CallLogService.cc
    ResourceManager.cc
    AppsInfoProvider.cc
    common.cc
    )

if (enable_opengles)
    list (APPEND common_sources GLActivity.cc)
endif (enable_opengles)

mg_add_source_files (${common_sources})

#commit _source_list_ to parent
mg_commit_source_files ()
