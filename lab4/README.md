# Grupo: 20
### Integrantes:
Victoria Lopez, Email: victorialopez23@mi.unc.edu.ar
Tomas Ponce Gessi, Email: tomaspgessi@mi.unc.edu.ar
Jeremias Baez, Email: jeremias.baez@mi.unc.edu.ar


## 1. Cuando se ejecuta el main con la opción -d, ¿qué se está mostrando en la pantalla?

El modo de debugging permite que se impriman en pantalla mensajes informativos acerca de lo que está ocurriendo con el sistema de archivos, tales como cada llamada a las funciones de la interfaz de FUSE, con sus respectivos argumentos y demás. Básicamente brinda información que permite determinar problemas y/o soluciones.

## 2. ¿Hay alguna manera de saber el nombre del archivo guardado en el cluster 157?

Suponiendo que en el cluster 157 se encuentra una entrada de directorio y esta entrada se encuentra en los primeros bytes del cluster entonces esta seria la forma: 
	
fat_volume vol = get_fat_volume();
u32 bytes_read = fat_table_bytes_per_cluster(vol->table);
u8 file_name= alloca(bytes_read);
off_t cluster_offset = fat_table_cluster_offset(vol->table, 157); 
full_pread(vol->table->fd, file_name, bytes_read, cluseter_offset);

Esto sería en el mejor de los casos, pero teniendo en cuenta que el cluster podría tener cualquier tipo de dato no es el algoritmo ideal. En dicho caso lo correcto es ir por todos los archivos desde el directorio / y ver si alguno de ellos utiliza el cluster 157, en el caso de que si ya se tiene el nombre, y en el caso de que ninguno use ese cluster entonces el cluster está probablemente libre.

## 3. ¿Dónde se guardan las entradas de directorio? ¿Cuántos archivos puede tener adentro un directorio en FAT32?

Se guardan en los clusters de datos del disco, cada entrada tiene un tamaño de 32 bytes. Un directorio puede tener aproximadamente 65536 entradas en FAT32, en la implementación del laboratorio un directorio puede tener como máximo 16 entradas de 32 bytes antes de llenar un cluster de 512 bytes.

## 4. Cuando se ejecuta el comando ls -l, el sistema operativo, ¿llama a algún programa de usuario? ¿a alguna llamada al sistema? ¿Cómo se conecta esto con FUSE? ¿Qué funciones de su código se ejecutan finalmente?

Cuando se llama ls -l o algún comando similar el Sistema Operativo se fija qué filesystem está montando en el path especificado y llama a la función que se encarga de realizar la operación. Dicha función depende del filesystem montado, y en el caso de FUSE lo que sucede es que se llama al módulo de FUSE dentro del kernel y luego FUSE ejecuta la función que definimos para manejar dicha operación, básicamente devolviendo el control y manejo del FS al usuario.

Particularmente se ejecutan las siguientes funciones:

- OpenDir (System Call) - Básicamente abre el directorio en el cual se encuentra para leerlo
- GetAttr - Obtiene los atributos de dicho directorio 
- ReadDir  (System Call) - Lee las entradas del directorio
- Close (System Call) - Cierra el directorio

Son funciones en espacio de usuario pertenecientes a la interfaz fat_fuse la cual se comunica con el módulo FUSE en el kernel. Entonces el sistema operativo, cuando se ejecuta ls -l, llama a las funciones antes mencionadas, se las transfiere a FUSE y son ejecutadas desde fat_fuse_ops.

##  5. ¿Por qué tienen que escribir las entradas de directorio manualmente pero no tienen que guardar la tabla FAT cada vez que la modifican?

En realidad si se modifican los datos en disco de la FAT cuando modificamos la FAT en memoria si es que eso queremos. Cuando modificamos las entradas de directorio de un archivo si es necesario llamar a una función particular que escribe dichas entradas en el disco pero, en el caso de la FAT, cuando la modificamos ya las llamadas a función que realizan la modificación contienen dentro código que guarda los cambios en el dispositivo.

## 6. Para los sistemas de archivos FAT32, la tabla FAT, ¿siempre tiene el mismo tamaño? En caso de que sí, ¿qué tamaño tiene? 

No, la FAT no siempre tiene el mismo tamaño, esto depende de la cantidad de clusters de datos que hay en el disco. Por ejemplo en un disco de 16GB  con clusters de 512 bytes la FAT tendrá aproximadamente 128 MiB y en un disco de 1 TB con 512 bytes la FAT tendrá aproximadamente 8 GiB
