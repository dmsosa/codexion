# Developer forewords 


on ce project j'ai besoin de faire un petit exercise q se apelle le philsoophers problem, mais je ne sai pas rien encore sur le multithreading et que est ce que c'est dans le premier moment, donc, je vourrai de pouvoir faire un petit programme con qui je pui gagneir un petit experience avec le utilisartion des fils de execusion. On ce project, j'ai besoin de faire un fil de execusio, comment ca se peux creer??

# Exercise numero 3 (troi)

Ce programme recoit des mots en arguments, cree un thread pour chaque mot, transforme chaque mot en majuscules, puis recupere le resultat de chaque thread et les affiche dans le terminal.

Par exemple.

./a.out hola salut servus


produit un résultat similaire à :
Thread 1: top of stack near 0x...; argv_string=hola
Thread 2: top of stack near 0x...; argv_string=salut
Thread 3: top of stack near 0x...; argv_string=servus

Joined with thread 1; returned value was HOLA
Joined with thread 2; returned value was SALUT
Joined with thread 3; returned value was SERVUS



avant d'ecrire une seule ligne de code, je veux lasse les choses tres clares, par exemple...

Le main thread et en charge de:
1. faire il parsing
2. allouer la memoire pour coders et dongles.
3. cree le premier thread, il e, le monitor thread

Le monitor thread faire les choses suivants:
1. prepare la condition de attendance
2. initialiser les mutex de log et de running
3. une fois la condition est pres, initialicer le bucle while, 
le bucle while executerai pendant 

Ca que mon thread a besoin de faire c'est 
permettre les coders de prendre les dongles necessaires pour commencr a compiler.

qu'est que c'est, prendre une dongle?
dongle.occupied = true
I lock, read it
if dongle.occupied
add_to_queue
dongle.queue[i] = coder
donglemutex.unlock
if now_ms() >= self.last_compile + self.burnout_time
	self.burned_out
	lock_log
	print I burned!
	unlock_lock

qu'est ce que une coder doit faire?


La question suivant et tres personelle, je t'invite a choisir la structur que vous aime plus.

DESING ET ARQUITECTURE DU PROJET