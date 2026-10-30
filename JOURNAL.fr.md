Madrid, 30 octobre
On doit faire le projet du codexion. Pour ça, on a commencé à bien comprendre le sujet sur les threads. Si tu es nouvelle, je recommande :

Lire le sujet (es.subject.pdf)
Pense à comment tu voudrais créer les coders et les dongles, ce qu'elles sont et quelle est la relation entre elles ?
Qu'est-ce que c'est, « prendre une dongle » ?
Pour prendre une dongle, on fait un changement dans l'état de la dongle, en changeant l'attribut used à true.

Pour faire ça, il faut verrouiller le mutex de la dongle pour s'assurer que seulement une coder à la fois lit l'état de la dongle actuelle :

is_used
queue
…
De même, quand on vérifie l'état des coders, il faut s'assurer que les propriétés suivantes ne sont vérifiées que par une coder à la fois :

compile_count
last_compiled + burnout_time
is_burned_out
De cette façon, on **s'assure qu'**un autre thread n'a pas changé les données d'une autre coder pendant qu'on a lu les propriétés mentionnées plus haut.