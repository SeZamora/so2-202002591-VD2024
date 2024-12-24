# Proyecto 2 Sistemas Operativo 2
## Asignador de memoria con lazy-zeroing en Linux

## 1. Diseño del Algoritmo de Asignación de Memoria y Llamadas al Kernel

### **a. Syscall zamora_tamalloc**

**Archivo:** `kernel/usac/sys_tamalloc.c`

La syscall utiliza `do_mmap` para reservar un rango de memoria virtual, sin asignar páginas físicas inmediatamente. Además, asegura que las páginas se inicialicen en cero al primer acceso.

- Usar `do_mmap` con los flags `MAP_PRIVATE | MAP_ANONYMOUS`.
- Configurar `vm_flags` con permisos de lectura y escritura.
- Modificar los flags del `VMA` con `VM_DONTCOPY` para evitar copias innecesarias.

```c
addr = do_mmap(NULL, 0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 
               vm_flags, 0, &populate, &uf);
if (IS_ERR_VALUE(addr)) {
    return addr;
}
```

**Archivo:** `mm/memory.c`

Modificamos la función `__do_fault` para inicializar las páginas en 0 al primer acceso. Esto asegura que las páginas asignadas sean limpias y seguras.

- Verificar si la página no está actualizada (`!PageUptodate`).
- Usar `memset` para inicializar la página a cero.
- Marcar la página como actualizada con `SetPageUptodate`.

```c
if (!PageUptodate(vmf->page)) {
    memset(page_address(vmf->page), 0, PAGE_SIZE);
    SetPageUptodate(vmf->page);
}
```

---

### **b. Syscall: zamora_mem_stats_process**

#### **Descripción**
Esta syscall permite obtener información detallada sobre la memoria utilizada por un proceso específico o por todos los procesos si el PID es igual a `0`. La información recolectada incluye:

- Memoria reservada (KB).
- Memoria comprometida (KB y porcentaje).
- OOM Score (indicador de prioridad para el Out-Of-Memory Killer).


**Archivo:** `kernel/usac/sys_stats_proc.c`

1. **Recolección para un proceso específico:**
   - Se utiliza `find_task_by_vpid` para encontrar el proceso con el PID proporcionado.
   - Se accede a la estructura `mm_struct` del proceso para obtener:
     - `total_vm` (memoria reservada).
     - `get_mm_rss` (memoria comprometida).
   - Se calcula el porcentaje de memoria comprometida respecto a la reservada.
   - Se obtiene el `oom_score` usando métodos del kernel.

2. **Recolección para todos los procesos:**
   - Se itera sobre todos los procesos utilizando `for_each_process`.
   - Para cada proceso, se recopilan las mismas estadísticas que para un PID específico.
   - Los resultados se devuelven en una estructura de datos para cada proceso.



### **c. Syscall: zamora_mem_stats_global**

#### **Descripción**
Esta syscall devuelve un resumen general de la memoria total reservada y comprometida en todo el sistema.


**Archivo:** `kernel/usac/sys_stats_global.c`

1. **Iteración sobre todos los procesos:**
   - Utiliza `for_each_process` para iterar sobre todos los procesos en el sistema.
   - Para cada proceso, se accede a su estructura `mm_struct` para obtener:
     - `total_vm` (memoria reservada).
     - `get_mm_rss` (memoria comprometida).

2. **Cálculo acumulado:**
   - La memoria reservada y comprometida se acumulan en las variables correspondientes.
   - Los valores se convierten a MB antes de ser devueltos.

## 2. Análisis de Resultados

### Tamalloc
- **Memoria Solicitada:** Las pruebas confirmaron que las páginas no se asignan inmediatamente.
- **Memoria Utilizada:** Se verificó que las páginas se inicializan a 0 en el primer acceso, cumpliendo con el objetivo de lazy-zeroing.

### Syscalls de Estadísticas
- Las mediciones para procesos individuales y estadísticas del sistema fueron precisas y consistentes con los valores obtenidos de `/proc`.
- Validación realizada bajo diferentes cargas de trabajo.

## 3. Cronograma de Actividades

El proyecto fue realizado en el siguiente orden:
- **Jueves 19:** Investigacion malloc y comenzar tamalloc
- **Viernes 20:** Realizar tamalloc
- **Sabado 21:** Pruebas tamalloc y empezar syscall zamora_mem_stats_process
- **Domingo 22:** Realizar syscalss zamora_mem_stats_process y zamora_mem_stats_global
- **Lunes 23:** Pruebas de syscalls y documentacion

## 4. Problemas Encontrados

- **Fallo de segmentación al usar Tamalloc:** Incorrecta inicialización de páginas durante un page fault.  

- **Fallo de segmentación al usar Tamalloc:** En un inicio, las páginas asignadas no eran inicializadas correctamente al ocurrir un page fault. Esto provocaba un acceso inválido a memoria cuando el proceso intentaba leer o escribir en esas páginas.  

- **Parámetros incorrectos al llamar a do_mmap**:  inicialmente se usaron parámetros incorrectos para do_mmap

## 5. Mensaje Personal de Conclusión

Este proyecto fue una experiencia desafiante pero gratificante. Implementar Tamalloc con lazy-zeroing y desarrollar syscalls para estadísticas de memoria me permitió profundizar en el funcionamiento interno del kernel de Linux. Resolver problemas como el manejo de page faults y la recolección precisa de datos de memoria reforzó mis habilidades en programación de bajo nivel y depuración.
