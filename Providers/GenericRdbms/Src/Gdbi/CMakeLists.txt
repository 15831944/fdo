include_directories( BEFORE	${CMAKE_CURRENT_SOURCE_DIR} )

set( Gdbi_STAT_SRCS
   GdbiCommands.cpp
   GdbiConnection.cpp
   GdbiException.cpp
   GdbiQueryResult.cpp
   GdbiStatement.cpp
   stdafx.cpp
)

add_library( Gdbi STATIC ${Gdbi_STAT_SRCS} )
add_dependencies( Gdbi fdordbmsmsg )
