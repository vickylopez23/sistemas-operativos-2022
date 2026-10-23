# Grupo: 20

# Integrantes:

- Victoria Lopez, Email: victorialopez23@mi.unc.edu.ar
- Tomas Ponce Gessi, Email: tomaspgessi@mi.unc.edu.ar
- Jeremias Baez, Email: jeremias.baez@mi.unc.edu.ar
- Eleonora Constanza Gómez Vera, Email:constanzagomez@mi.unc.edu.ar

# Introducción

En este laboratorio desarrollamos una copia básica de BASH, implementando ejecución de comandos simples, múltiples pipelines, redirectores y ejecución en segundo plano o primero.

# Modularización:

El proyecto está divido en 6 módulos, dichos módulos son:

- Módulo MyBash
- Módulo Command
- Módulo Parsing
- Modulo Parser
- Módulo Builtin
- Módulo Execute

## Modulo MyBash:

Es el módulo que contiene la función main y quien comienza el programa. Contiene el ciclo principal del bash que maneja el parseo, ejecución y limpieza.

Particularmente en nuestra implementación este módulo se basa en ejecutar el ciclo hasta el fin de MyBash. Primero empieza imprimiendo el prompt de MyBash, luego parsea el Standard Input, si hay una pipeline valida la ejecuta, limpia los **procesos zombies** y si no se llego al final de Standard Input repite el proceso.

Los módulos del programa están interconectados por el siguiente diagrama de flujo:


![](https://preview.redd.it/013ckzxxqpo91.png?width=800&format=png&auto=webp&s=7fd2a8a753d3315721fc177df127df40b5e43a76)


### Funciones Privadas:

- main
- show_prompt

## Módulo Parser:

Es un módulo provisto por los profesores que contiene una API que permite parsear fácilmente un archivo en busca de comandos y pipelines. Particularmente utilizado para parsear el Standard Input del proceso de MyBash.

### Funciones Públicas:

- parser_new
- parser_destroy
- parser_next_argument
- parser_op_background
- parser_skip_blanks
- parser_op_pipe
- parser_garbage
- parser_last_garbage
- parser_at_eof

## Módulo Parsing:

Es el módulo encargado de procesar la entrada de MyBash, es decir, procesa los caracteres ingresados por el Standard Input utilizando el módulo Parser, dicho parseo obtiene las estructuras abstractas definidas en el módulo Command en base a los caracteres leídos y luego devuelve una estructura de tipo Pipeline con la información leída. Ante cualquier error de sintaxis en la pipeline tales como comandos vacíos, pipelines vacíos o incluso basura en la entrada el módulo se encarga de hacerle saber al usuario de su error para que los corrija.

### Funciones Públicas:

- execute_pipeline

### Funciones Privadas:

- parse_scommand

## Módulo Command:

Es el primer módulo que desarrollamos, este contiene los TADs (Tipos Abstractos de Datos) y las funciones que interactúan con ellos.

Tenemos 2 TADs:

### TAD SCommand: 
Representa la estructura abstracta de un comando simple, contiene el comando, sus argumentos y si los posee sus redirectores del Standard Input y el Standard Output

### TAD Pipeline: 
Representa la estructura abstracta de una pipeline, la cual contiene una lista de SCommand que se supone que deben ser conectados entre sí y un booleano representando si la pipeline debe ser ejecutada en segundo plano o no.

### Funciones Públicas (TAD SCommand):

- **scommand_new**
- **scommand_destroy**
- **scommand_push_back**
- **scommand_pop_front**
- **scommand_set_rendir_out**
- **scommand_set_rendir_in**
- **scommand_is_empty**
- **scommand_lenght**
- **scommand_front**
- **scommand_get_rendir_in**
- **scommand_get_rendir_out**
- **scommand_to_string**

### Funciones Públicas (TAD Pipeline):

- **pipeline_new**
- **pipeline_destroy**
- **pipeline_push_back**
- **pipeline_pop_front**
- **pipeline_set_wait**
- **pipeline_is_empty**
- **pipeline_length**
- **pipeline_front**
- **pipeline_get_wait**
- **pipeline_to_string**

## Módulo Parsing:

Se encarga de realizar el **parseo de una pipeline**. Tiene en cuenta las posibilidades en las que el Standard Input puede llegar a encontrarse, es decir, si el usuario ingresa un comando vacío, o una secuencia de comandos vacíos conectados por pipes o cualquier otra combinación que no tenga un sentido sintáctico definido estandarizado por la Shell Grammar de hasta nivel 2 (comandos simples y pipelines) entonces se envia un mensaje de error.

### Funciones Privadas:

**parse_scommand**: Se encarga de **parsear un comando simple**, esto es un comando, sus argumentos y opcionalmente redirectores, no incluye las pipes | o el operador de background &. Devuelve un TAD de tipo SCommand con el comando parseado, utilizamos al módulo Parsing para realizar el parseo.

### Funciones Públicas:

**parse_pipeline**: Se encarga de **parsear una pipeline**, esto es una serie de comandos simples unidos por un pipe | y opcionalmente un operador de background &. Utiliza a parse_scommand para parsear los comandos simples SCommand y si a continuación se encuentra una pipe | parsea el siguiente comando simple.

## Módulo Builtin:

Este módulo se encarga de realizar la **ejecución de comandos especiales**. Dichos comandos no pertenecen al sistema operativo sino a la propia MyBash, es decir, son comandos internos.

Dichos comandos internos son:

- **CD**: Permiten mover el CWD (Current Working Directory) del proceso
- **EXIT**: Finaliza la ejecución de MyBash de manera limpia.
- **HELP**: Imprime en pantalla un mensaje de ayuda de como usar la terminal y quienes son sus creadores

### Funciones Públicas:

- **builtin_is_internal**: Determina si un TAD SCommand es un comando interno o no.
- **builtin_alone**: Determina si un pipeline tiene un único comando y si dicho comando es interno.
- **builtin_run**: Ejecuta un comando interno.

## Módulo Execute:

Execute es el módulo encargado de la ejecución de los pipelines. Para entender mejor cómo implementarlo primero hay que tener en claro algunos conceptos.

Un comando simple es un proceso que admite argumentos modificadores, y está dotado con la posibilidad de redirigir su entrada y salida estándar a un directorio.
Cada vez que ejecutamos un comando simple, lo que hace la shell como proceso padre, es generar un proceso hijo que terminará haciendo la tarea especificada.

Una pipeline, es una serie de comandos simples conectados por el operador “|”
donde la pipe funciona como un medio de transmisión de información. Y cada vez que ejecutamos una pipeline con “n” comandos simples, la shell genera “n” procesos hijos, donde el primer proceso lee del descriptor de archivo de entrada de su proceso padre (shell), y el último escribe en el descriptor de archivo de salida también de la shell.

![](https://preview.redd.it/42p7x4yfbpo91.png?width=1226&format=png&auto=webp&s=81ea16fb66a614da4bba99e03321ccf05255be41)

Entonces, básicamente modularizamos en dos partes principales, la **ejecución del comando simple** por un lado y el manejo de tuberías para la **ejecución de una pipeline** por otro. Para esto planteamos las siguientes funciones:

### Funciones del Módulo:

- **execute_scommand**: Toma un comando simple, prepara los **redirectores** mediante la llamada de init_scommand_redirects() y ejecuta la syscall **execvp()** con los argumentos, que nos provee el comando simple scmd, para realizar la tarea.
- **init_scommand_redirects**: Si el comando simple tiene redirectores, entonces los toma. Si es redirección de entrada se guarda el descriptor del archivo para luego utilizarlo como Standard Input.
  En caso de ser redirección de salida, si el archivo existe, se guarda el descriptor del archivo para luego utilizarlo como Standard Output. Sino se lo crea.

![](https://preview.redd.it/vem8sf2xqpo91.png?width=486&format=png&auto=webp&s=b00b6f35dda036d58bcbf56a47ec3f851b1022e5)

  
- **scommand_to_array_pointer**: Toma un comando simple y lo convierte en un arreglo de strings (char \*) con el último elemento igual a NULL. Esto es para ser usado por execvp

- **execute_pipe**: En primera instancia, genera un proceso hijo mediante la syscall **fork()**.En el caso de ser necesario hace el manejo de tuberías el cual, como se verá en execute_pipeline, va a depender de su ubicación en la pipeline.Seguidamente, se llama a execute_scommand() para completar el trabajo del comando simple. Por último se cierran las puntas de las tuberías duplicadas por el proceso hijo.

### Función Pública principal del Módulo:

- **execute_pipeline**: Esta es la llamada principal del módulo, se encarga de toda la ejecución de una pipeline completa.

En principio, se determina si se trata de un comando interno o no. En el caso de que si se delega la ejecución del mismo al módulo builtin.

Caso contrario comienza la acción de Execute, primero determinamos dos casos: si queremos ejecutar un único comando simple o una pipeline.

En el primero simplemente se llama a execute_pipe() con los descriptores de archivos estándar de entrada y salida. Si el comando viene acompañado del operador &, entonces se lo deja ejecutando en segundo plano, en caso contrario se realiza la syscall **waitpid()** para que la shell espere a la finalización de la ejecución del proceso hijo antes de permitir la ejecución de otro.

En el segundo caso nos encontramos con la ejecución de múltiples comandos simples y la realización de la ***“plomería”***.

Para conectar comandos simples mediante una pipe identificamos 3 modos:

1. Si es el _primer comando_ a ejecutar entonces lee de la entrada estándar de la shell y escribe del lado de escritura de la tubería.
   fd_in ≡ STDIN_FILENO (descriptor de archivo de Standard Input)
   p[1] ≡ lado de escritura de la pipe
   execute_pipe(fd_in, p[1], scmd, p);

2. Si es un _comando intermedio_, lee del lado de lectura de la pipe anterior y escribe del lado de escritura de la tubería.
   fd_in ≡ lado de lectura de la pipe
   p[1] ≡ lado de escritura de la pipe
   execute_pipe(fd_in, p[1], scmd, p);

3. Si es el _último comando_ a ejecutar, lee del lado de lectura de la tubería anterior y escribe en la salida estándar de la shell.
   fd_in ≡ lado de lectura de la pipe
   fd ≡ STDOUT_FILENO (descriptor de archivo de Standard Output)
   execute_pipe(fd_in, STDOUT_FILENO, scmd, p);

Con esta idea, para armar una pipeline de n comandos simples, generamos un bucle para los primeros n-1 comandos, que corresponde al número de pipes. Por cada ciclo i se realiza la siguiente rutina:

- Se crea una pipe.
- Se ejecuta el primer comando de la pipeline teniendo en cuenta el modo.
- Se cierra la punta de escritura de la pipe.
- Se guarda en una variable la punta de lectura de la pipe, para el siguiente ciclo.
- Se elimina el comando ejecutado de la pipeline.
- Se guarda el siguiente comando de la pipeline en una variable, para el siguiente ciclo.

Ya habiendo iniciado la ejecución de la pipeline, basta ver si será en background o foreground.

Para ejecutar una pipeline en foreground hay que hacer un **wait()** por cada proceso hijo creado. Para ello, creamos una lista donde guardamos el mapeo de los id de cada proceso, para luego mediante un bucle hacer un waitpid() de cada elemento de la lista.

En caso de ser en background no hacemos nada y retornamos a la función que llamó a execute_pipeline, es decir main en el módulo MyBash.

### Diagrama de dependencias:

![](https://i.redd.it/rjuhkq3ogpo91.png)


# Técnicas de Programación Utilizadas:

### Programación defensiva:

Para evitar errores del usuario y para defendernos de las distintas situaciones que se nos podían presentar, como si nos llegaban más de dos comandos, si un comando es vacío, si queda basura irrelevante, etc

### Modularización:

Para mantener una estructura fácil de desarrollar y escalar utilizamos la modularización como técnica para dividir y estructurar el código. Esto consiste en dividir el código en módulos autocontenidos que se encargan de una área del programa y solo esa área.

### Librerías Utilizadas:

Fundamentalmente utilizamos la librería GLib y la librería estándar de C.

# Herramientas de Programación:

Utilizamos GCC para compilar, GDB para debugear y Valgrind para detectar memory leaks. A su vez utilizamos Git para manejo de versiones del código y un entorno de desarrollo avanzado como Visual Studio Code.

# Proceso de desarrollo:

Primero empezamos implementando de manera muy básica los TADs, después nos dividimos y fuimos haciendo el parsing y viendo cómo arrancar a organizar y hacer el execute, ya que este era el más complejo de los módulos porque incluye saber cómo manejar la syscall, manejar redirectores, procesos y los montones de errores posibles.

Al principio implementamos esto de manera simple sin testear el código, sino más bien determinando la semántica de las líneas de código, abordando los posibles errores que creíamos que podíamos tener. Más adelante finalizamos el desarrollo de los módulos Command y Parsing con todos los tests completos y utilizamos hasta entonces los códigos compilados de dichos módulos para avanzar con execute.

A partir de este momento nos concentramos grupalmente en el módulo Execute y de manera secundaria con el módulo Builtin. Por un lado nos aseguramos de tener una estructura básica del módulo Execute, qué funciones realizan qué cosas y como consejo del profesor modularizamos internamente Execute para que cada función tenga su trabajo. Con esto realizado cada uno estuvo encargado de hacer su parte de Execute, por un lado los redirectores, por otro el manejo de procesos hijos y por otro la ejecución de una pipeline de N pipes, no nos limitamos inicialmente a solo 2.

Luego de haber implementado Execute y haber resuelto los tests continuamos con Builtin dado que era mucho más sencillo y finalmente empezamos a pulir el código para entregar, esto incluye el manejo de procesos zombies y casos bordes hasta entonces aún no considerados.

En términos de problemas lo que más nos costó fue el manejo de las system calls y la incomprensión de la documentación de algunas de ellas. A su vez, el hecho de no haber desarrollado de manera veloz los módulos Command y Parsing hizo que nos demoraramos mucho en Execute pues no fue hasta una semana después que nos dimos cuenta de usar los archivos compilados y nos pusimos a full a finalizar esos dos módulos. Esto hizo que no pudiéramos testear la funcionalidad de las system calls con ejemplos concretos.

# Conclusión:

El proyecto estuvo muy bueno, nada que ver con cualquier otro proyecto de materias pasadas, esto ya se asemejaba mucho más a proyectos reales y por ende fue mucho más interesante que laboratorios pasados.

Logramos implementar toda la especificación que se nos presentó, incluyendo los ejercicios estrellas de ejecución de múltiples pipes e imprimir detalles extras en el prompt. Algo que nos falto y no hubiese estado tan complicado fue implementar la ejecución de comandos secuenciales con &&, pero aun asi estuvo buenísimo, y ni hablar cuando el programo paso los tests al 100% :D
