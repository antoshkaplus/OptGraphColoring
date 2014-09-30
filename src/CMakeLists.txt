
set(CURRENT_SOURCE_FILES
    ColoredGraph.cpp
    ColoredGraph.h
    FeasibleSwapper.cpp
    FeasibleSwapper.h
    GC.cpp
    GC.h
    GC_BB_NodeLevel.cpp
    GC_BB_NodeLevel.h
    GC_TabuSearch.cpp
    GC_TabuSearch.h
    InfeasibleSwapper.cpp
    InfeasibleSwapper.h
    main.cpp)


set(SOURCE_FILES)
foreach(file ${CURRENT_SOURCE_FILES}) 
    LIST(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach(file)

set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)

