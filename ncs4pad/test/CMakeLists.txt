LIST (APPEND app_list
    bitblt
    container
    eventhandler
    transround
    exseparator
    container-nesting
    textpiece
    buttonpanel
    animationedit
    panel-update
    iconflow
    museritem
    tableview
    itempiece
    widgethostpiece
    autoscroll
    autotable
    myfilldc
    gaussblur
    mousemove
    subdc
    scrollview
    #tslib
    #listitemmanager
    #groupitemmanager
    #wrappaint
    #tableviewitem
    )

###############################################

FOREACH (i ${ncs4pad_source_list})
    LIST (APPEND abs_ncs4pad_src_list ${PROJECT_SOURCE_DIR}/ncs4pad/${i} ${PROJECT_SOURCE_DIR}/common/ResourceManager.cc)
ENDFOREACH (i ${ncs4pad_source_list})

ADD_LIBRARY (cellncs4pad STATIC ${abs_ncs4pad_src_list})

LIST (APPEND APPLIBS
    cellncs4pad
    ${_LIBS}
    )

MACRO (APP _name)
    add_executable(test-${_name} test-${_name}.c)
    target_link_libraries(test-${_name} ${APPLIBS})
ENDMACRO (APP _name)

FOREACH (i ${app_list})
    APP (${i})
ENDFOREACH (i)
