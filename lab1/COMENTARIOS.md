## Informe 
- Muy bueno

## Repo 
- Autoría de commits despareja

## Código
- En general bien
- Overkill usar listas en builtin? 
- Un arreglo para los fd de pipes no sería más cómodo?
- Yo opino que `execute_scommand` no debería ser llamada dentro de `execute_pipe` para separar la plomería de pipelines de la ejecución de comandos simples. Es un anidamiento innecesario.
- Hay sectores en donde debería estar más claro qué código ejecuta el padre y cuál el hijo 
- defines para caracteres especiales en los `*_to_string`
- flags en `open` para crear archivo. Por qué `creat`?

## Funcionalidad
- Testeo 100%
- Parser no detecta algunos comandos mal formados `ls |`, `ls >`
- Punto estrella prompt
