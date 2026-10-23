## Informe 
- Bien las respuestas de las preguntas
- Se le puede asignar un quantum "injusto" a un proceso.
- El RR no es estrictamente RR en xv6
- Las gráficas pueden mejorar mucho. Mezclar unidades de medida está prohibido. Además se puede sintetizar información para no llenar de gráficas el informe.
- Muy buenos análisis de los resultados
- Ojo con starvation. Se produce porque hay una regla del mlfq que es muy estricta: si un proceso A tiene mayor prioridad que un proceso B se ejecuta A y no B. O sea que siempre que haya un proceso en prioridad alta los de baja no se ejecutan (salvo que hagas priority boost o algo así)
- Faltó decir cómo implementaron el MLFQ

## Repo 
- OK

## Código
- OK
- Hardcodearon 3 queues en vez de hacer un arreglo de NPRIO queues
- Código comentado

## Funcionalidad
- Bien `proc.h` y `procdump`
- Bien los cambios de prioridad
- Bien que se tomaron el trabajo de hacer bien las colas
- Bien que pensaron en locks
