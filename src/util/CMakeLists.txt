file (GLOB LIB_SOURCES "*.cc")
add_library (util STATIC ${LIB_SOURCES})
add_library (util-timing STATIC ${LIB_SOURCES})
target_compile_definitions(util-timing PUBLIC TIME_FIXPOINT=1)
add_library (util-timing-2 STATIC ${LIB_SOURCES})
target_compile_definitions(util-timing-2 PUBLIC TIME_FIXPOINT=2)
