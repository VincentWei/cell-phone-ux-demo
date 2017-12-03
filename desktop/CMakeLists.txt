#append source to _sources_list_
list (APPEND desktop_sources
      DesktopToolbar.cc
      ActivityDesktop.cc
      DesktopCommonProc.cc
      IPhoneDesktopController.cc
      IPhoneDesktopLayout.cc
      IPhoneDesktopView.cc
      IPhoneDesktopItem.cc
      IPhoneDesktopState.cc
      IPhoneDesktopStateNormal.cc
      IPhoneDesktopStateShaking.cc
      IPhoneDesktopStateShifting.cc
      IPhoneDesktopStateDraging.cc
      IPhoneDesktopStateInertia.cc
      IPhoneDesktopStateTransposition.cc
      IPhoneDesktopStatePickup.cc
      IPhoneDesktopStatePageTurning.cc
      IPhoneDesktopTransitionInterface.cc
      TransitionStandStillOnItem.cc
      TransitionClickOnItem.cc
      TransitionSpeedMeasure.cc
      TransitionCollideItem.cc
      TransitionPressOnItem.cc
      TransitionFilterMouseMove.cc
      IPhoneDesktopStateApp.cc
      ScrollPageTrigger.cc
      speedmeter.c
     )

mg_add_source_files (${desktop_sources})

#commit _source_list_ to parent
mg_commit_source_files ()
