include_directories (${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_FILES_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set (SOURCES
	aboutprog.cpp
	gamerules.cpp
	ball.cpp
	clickableshadow.cpp
	footballer.cpp
	game.cpp
	gamemodedialog.cpp
	main.cpp
	mainwindow.cpp
	pitch.cpp
)
     
set (HEADERS
	aboutprog.h
	gamerules.h
	ball.h
	clickableshadow.h
	footballer.h
	game.h
	gamemodedialog.h
	mainwindow.h
	pitch.h
)
     
set (UI
	mainwindow.ui
	gamemodedialog.ui
	aboutprog.ui
	gamerules.ui
)

set (RESOURCES
	resources.qrc
)

qt4_wrap_cpp (SOURCES ${HEADERS})
qt4_wrap_ui (UISOURCES ${UI})
qt4_add_resources(RESOURCES_RCC ${RESOURCES})
add_executable (Diabolo ${SOURCES} ${RESOURCES_RCC} ${UISOURCES})
target_link_libraries (Diabolo ${QT_LIBRARIES})
