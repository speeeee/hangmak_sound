all:
	gcc -g -o hang hangmaksnd.c sounds.c glob_structs.c stage_util.c sound_util.c gl_util.c glad.c -rdynamic -lglfw3 -lrt -lm -ldl -lX11 -pthread -lXrandr -lXinerama -lXxf86vm -lXcursor -lasound -ljack -lportaudio -lsndfile
