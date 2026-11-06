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