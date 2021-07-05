project(server)

file(GLOB SERVER_SRC
	src/*.cpp
	src/*.h
)

source_group("all" FILES ${SERVER_SRC})

add_executable(server 
	${SERVER_SRC}
)

target_include_directories(server PUBLIC src)
target_link_libraries(server sky)