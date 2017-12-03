#append all files to _source_list_.
#add_all_source_file ()
list (APPEND include_sources
    register.h
    ActivityFactory.hh
    Activity.hh
    ActivityStack.hh
    DesktopCommonProc.hh
    DeviceSkin.h
    Event.hh
    global.h
    GraphicsView.hh
    NCSActivity.hh
    SignalEventWithParameters.hh
    StateMachine.hh
    TransitionAnimationFinished.hh
    TransitionHover.hh
    TransitionNoInput.hh
    TransitionStandStill.hh
    utility.h
    ActivityDial.hh
    TimeService.hh
    ResourceManager.hh
    )

if (enable_opengles)
    list (APPEND include_sources GLActivity.hh)
endif (enable_opengles)

add_source_file ("${include_sources}")
   
#commit to parent
commit_source_files ()
