# Order to launch:
Ctrl + shift + v to change to the view mode

1. main
   - compile: make 
   - launch ./server
2. retard
   - compile: gcc src/retard.c -o bin/retard -lm
   - launch ./retard 127.0.0.1
3. client
   - compile: gcc src/clientJoy.c -o bin/client -lm 
   - launch ./client 127.0.0.1


Dans le code changer:
- [ ] 

> **_NOTE:_**  Change on the code:  *main.cpp* line *119* for the correct IP
> **_NOTE:_**  Change on the code:  dans makefile  la line 13 pour le bone chemin du dossier CoppeliaSim_Edu_V4_5_1_rev4_Ubuntu22_04