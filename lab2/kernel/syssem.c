#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include <stdlib.h>
 
static int sems[500];
static struct spinlock sems_lock; // Candado cerrado
 
void semaphores_init(void) //inicializa el semaforo
 
{
 initlock(&sems_lock, "Semaphores Lock"); //inicializa el candado
 for (int i = 0; i < 500; i++) // //inicializa el arreglo de semaforos
 {
   sems[i] = -1; //
 }
}
 
uint64
sys_sem_open(void)
{
 int id;   // Identificacioon
 int size; // Tamaño inicial del semaforo
 argint(0, &id);
 argint(1, &size);
 if (id < 0 || id >= 500)
 { // Prevent buffer overflow; Otherwise give full kernel access :D
   return 0;
 }
 acquire(&sems_lock); // Adquirir el candado // Bucles (giros) hasta que se adquiere el bloqueo.
 sems[id] = size;     // Inicializar el semaforo
 release(&sems_lock); // Libera sems lock
 return 1;
}
 
uint64
sys_sem_close(void)
{
 int id;
 argint(0, &id); // Arg en poscion 0,que le pongo la id
 if (id < 0 || id >= 500)
 { // Prevent buffer overflow; Otherwise give full kernel access :D
   return 0;
 }
 acquire(&sems_lock); // Adquirir el candado // Bucles (giros) hasta que se adquiere el bloqueo.
 sems[id] = -1;       // Cierra el semaforo
 release(&sems_lock); // Libera sems lock
 wakeup(&sems);
 return 0;
}
 
uint64 sys_sem_up(void)
{
 int id;
 argint(0, &id); // Arg en poscion 0, que le pongo la ID
 if (id < 0 || id >= 500)
 { // Prevent buffer overflow; Otherwise give full kernel access :D
   return 0;
 }
 acquire(&sems_lock); // Adquirir el candado
 if (sems[id] == -1)  // Bucles (giros) hasta que se adquiere el bloqueo.
 {
   release(&sems_lock); // Libera sems lock
   return 0;
 }
 sems[id] = sems[id] + 1; // Incrementa el semaforo
 release(&sems_lock);     // Libera sems lock
 wakeup(&sems);           // Despertartamos a todos los procesos en el canal de sems
                          // I.E, en el canal identificado por la dirección de memoria de sems.
 return 1;
}
 
uint64
sys_sem_down(void)
{
 int id; // Identificacion
 argint(0, &id);// Arg en poscion 0, que le pongo la ID
 if (id < 0 || id >= 500)
 { // Prevent buffer overflow; Otherwise give full kernel access :D
   return 0;
 }
 acquire(&sems_lock); // Adquirir el candado
 while (1) //bucle simpre true
 {
   if (sems[id] == -1) //si el semaforo es igual a -1
   {
     release(&sems_lock); // Libera sems lock
     return 0;
   }
   if (sems[id] > 0) // si es mayor  a 0 el semaforo
   {
     sems[id] = sems[id] - 1; //decrementa el ssemaforo
     release(&sems_lock);    // Libera sems lock
     return 1;          
   }
   if (sems[id] == 0) //si el semaforo es igual a 0
   {
     sleep(&sems, &sems_lock); // dormir en el canal de sems
   }
  }
 }

