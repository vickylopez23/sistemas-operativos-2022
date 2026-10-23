## Informe 
- Bien
- Markdown medio roto en la visualización de Bitbucket
- En el cuadro podría ser más claro el cambio del valor del semáforo. En los incrementos y decrementos podrían usar `0->1` y `1->0`

## Repo 
- Bien

## Código
- En general bien, sintético y claro
- Archivos vacíos (`semup.c` y `semdown.c`)
- Variable para número de semáforos
- Comentario de buffer overflow?
- Se pueden ahorrar wakeups en `sem_up`

## Funcionalidad
- `pingpong` bien
- Bien registradas las syscalls y uprogs
- `wakeup` en `close`?
