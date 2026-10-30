Ou on est on ce moment.

### Madrid, Octobre 30th

On doit faire le projet du codexion, pour ca, on a commencer pour bien comprendre le sujet sur les threads, se tu est nouvelle, je racommande:

1. Lir le sujet (es.subject.pdf)
2. Pense-toi, comment est ce que voux creerais les coders et les dongles, qu'est que il sont e quelle est la relation entre elles?
3. 

### Qu'est que c'est prendre une dongle?

Pour prendre une dongle, on faire un change dans l'etat du dongle, avec une change dans le attribute "used", que doit etret vrai.

Pour faire ca, il faut verroullier le mutex du dongle pour assurer que seulement une coder a la fois lise l'etat du dongle actuel:
- is_used
- queue
- 

Similairement, quand'on verifier l'etat des coders, il faut s'assurer que les propertes suivants sont elles verifies seulement pour une coder a la fois:

- Compile_count
- Last_compiled + Burnout_time
- Is_burned_out

De ce facon, on s'assurer que un autre fil n'a pas change les donnes d'autre coder quand'on a lise les propiertes mentionness d'avant.

