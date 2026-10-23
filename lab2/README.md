# Grupo: 20
### Integrantes:
- Victoria Lopez, Email: victorialopez23@mi.unc.edu.ar
- Tomas Ponce Gessi, Email: tomaspgessi@mi.unc.edu.ar
- Jeremias Baez, Email: jeremias.baez@mi.unc.edu.ar


### Introducción
En este laboratorio implementamos el programa ping pong,  y las syscall que involucran un semáforo el cual utilizamos para administrar a pingpong.

Para implementar una system call primero tuvimos que entender cómo funciona esta interfaz y para ello tuvimos que ver qué directorios están involucrados y el flujo al que responden.

[user> user.h]
Acá están definidos todos los prototipos de las system call disponibles para los procesos de usuario.

[kernel> syscall.h]
En este directorio se define una constante numérica para cada system call.

[kernel> syscall.c]
En este archivo se encuentra en particular la función syscall() la cual determina la syscall deseada y la invoca. 

[kernel> usys.s]
(este archivo es generado automáticamente por un perl script usys.pl.)
Posee una stub routine por cada system call, esta rutina es la encargada de hacer el trapeo del modo usuario al modo kernel y realizar la system call deseada a través de syscall().

kernel>sysproc.c
Acá se realiza la implementación de la lógica de la system call, que toma vacio por argumento pero  los argumentos que requiere para el desarrollo son llamados por una serie de funciones pre implementadas: argint() en el caso de requerir un entero.
Pero por razones de prolijidad la implementación de las syscall que componen en el semáforo las realizamos en un nuevo directorio llamado “syssem.c”

### System Calls Utilizadas
__sleep()__
Pone un proceso a dormir cambiando su estado actual de RUNNABLE a  SLEEPING y utiliza el valor “canal” como identificador del proceso para poder despertarlo.

__wakeup()__
Despierta todo proceso en estado SLEEPING que coincida con el valor del canal que se le pasa como argumento.  

__acquire()__
Intenta adquirir un lock, de forma que si ya está en uso hace un bucle hasta que lo logra.
La adquisición es atómica: sin interrupciones.

__release()__
Libera un look,si es que está tomado.

__argint()__
Obtiene el enésimo argumento de la syscall, que corresponde al enésimo registro de usuario en el trapframe.

### Syssem

# Funciones Públicas:
- sem_open
- sem_close
- sem_up
- sen_down

El tipo de dato que utilizamos para la representación del semáforo es un arreglo de enteros donde cada posición del arreglo representa el ID de un semáforo, y el valor contenido en esa posición representa el tamaño del semáforo. 

Decidimos que el arreglo tiene que tener una capacidad máxima de 500 semáforos y que un semáforo no inicializado se encuentra en valor -1.

Una vez que entendimos el comportamiento de los semáforos, hicimos las respectivas implementaciones donde nos encontramos con los siguientes problemas: 

El semáforo es un recurso compartido, por ende tuvimos que ayudarnos de los spinlocks para que el uso del semáforo entre diferentes procesos sea de manera atómica, definiendo el acceso al semáforo como una __sección crítica__.

Por otro lado, el uso de las funciones __sleep() y wakeup()__ comprendian el manejo del argumento void *chan que en un principio no era tan claro el “cómo” utilizarlo, pero resultó ser un valor común entre ambas funciones.


#### Pingpong

Es el programa utilizado para testear el semáforo, la implementación la realizamos con dos semáforos a través de dos procesos, uno de ellos generado por un fork y el otro el proceso padre.
El funcionamiento es muy simple y se puede ver expresado en el siguiente cuadro :

![](https://preview.redd.it/uj6d6jywtor91.png?width=708&format=png&auto=webp&s=78c03f55e5d1a439cd489b6ed22f0a67b64f2d5c)

Esto se repite n veces en un bucle, donde n es el argumento que se pasa por pantalla y n es un entero mayor a 0.

### Conclusión

Fue un laboratorio corto pero con mucho trabajo intelectual. Si bien el manejo de xv6 lo sentimos ajeno en un principio, también nos brindó una mirada más concreta de las bases de un sistema operativo. Además adquirimos los conocimientos suficientes como para implementar una syscall y pudimos ver el indireccionamiento que el sistema operativo utiliza como protección al momento de utilizar esas syscall. 
La parte más compleja definitivamente fue realizar las syscall del semáforo, en especial sem_down y sem_up. 
Y por último, hay que destacar que estamos conformes con el resultado del laboratorio. 












