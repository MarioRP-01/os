# Minishell

## Requisitos de la Aplicación

- [x] Ser  capaz  de  reconocer  y  ejecutar  en  foreground  líneas  con  un  solo mandato  y  0  o  más  argumentos.  (0,5 puntos)

- [x] Ser capaz de reconocer y ejecutar en foreground líneas con un solo mandato y 0 omás argumentos, redirección de entrada estándar desde archivo y redirección de salida a archivo. (1 punto) 

- [x] Ser capaz de reconocer y ejecutar en foreground líneas con dos mandatos consusrespectivos argumentos, enlazados con ‘|’, y posible redirección de entradaestándardesde archivo y redirección de salida a archivo. (1 punto)

- [x] Ser  capaz  de  reconocer  y  ejecutar  en  foreground  líneas  con  más  de  dos mandatos  con  sus  respectivos argumentos, enlazados con ‘|’, redirección de entradaestándar desde archivo y redirección de salida a archivo. (2,5 puntos) 

- [x] Ser capaz de ejecutar el mandato  cd (0,5 puntos). Mientras que la mayoría de losmandatos son programas del sistema, cd es un mandato interno que debe ofrecer elpropio intérprete de mandatos. El mandato cd debe permitir  tanto  el  acceso  a  través  de rutas  absolutas  como  relativas,  además  de  la  posibilidad  de  acceder  al directorio especificado en la variable HOME si no recibe ningún argumento, escribiendo laruta absoluta del nuevo directorio actual de trabajo. Para el correcto cambio dedirectorio el mandato cd se debe ejecutar sin pipes.

- [x] Evitar  que  los  comandos  lanzados  en  background  y  el  minishell  mueran  al enviar  la  señal  desde  el  teclado SIGINT, mientras los procesos en foregroundrespondan ante ella. La minishell deberá mostrar una nueva línea y un nuevo prompt cuando reciba dicha señal (1 punto)

- [x] Ser  capa  de  ejecutar  el  mandato  exit  (0,5  puntos).  exit  es  un  mandato interno  que  cuando  se  ejecuta  la minishell termina de manera ordenada y se muestrael prompt de la Shell desde dónde se ejecutó.