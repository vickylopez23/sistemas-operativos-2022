## Informe 
- OK
- No se puede encontrar el nombre de archivo al que pertenece un cluster arbitrario salvo recorriendo todo el árbol de archivos e importantemente, para cada archivo recorrer cada cadena de clusters hasta llegar al que corresponde. Lo que ustedes proponen permite leer el contenido del cluster que no tiene ninguna metadata del archivo al que pertenece
- La fat está mapeada a memoria con `mmap` y `munmap` hace persistente este mapeo al desmontar el volumen (esto está en `fat_volume.c`)

## Repo 
- OK

## Código
- OK
- `children` variable redundante?
- Leen todo el bad cluster, se puede seer solo la primera dentry
- `fat_file_unlink` y `fat_file_rmdir` podrían ser iguales? O colapsarlas en una sola función quizás

## Funcionalidad
- OK