## Código fuente del kernel modificado

### 1. Mensaje Personalizado Durante la Inicialización

#### **Ubicación del Cambio**
El cambio se realizó en el archivo:
```
init/main.c
```

Se agregó un mensaje personalizado utilizando la función `pr_notice` en la función `start_kernel`. Este mensaje aparece durante el proceso de arranque del sistema, inmediatamente después de que se imprime el banner del kernel.

#### **Código Modificado**
```c
pr_notice("--------------------------------------------------\n");
pr_notice("---------Bienvenido al Kernel USAC----------------\n");
pr_notice("--------------------------------------------------\n");
```


1. Compilación y carga del kernel.
2. Validación del mensaje en los logs del sistema mediante:
   ```bash
   sudo dmesg | grep "Bienvenido al Kernel USAC"
   ```


### 2. Cambio del Nombre del Sistema

Antes de realizar las syscalls debemos modificar 2 Makefile para que tome en cuenta nuestra nueva carpeta **kerne/usac** donde estaran nuestras syscalls nuevas

- En el **~/Makefile** agregamos `LINUXINCLUDE += -I$(srctree)/kernel/usac/include`

- En el **kernel/Makefile** agregamos `obj-y += usac/`

#### **Ubicación del Cambio**
El cambio se realizó en el archivo:
```
include/linux/uts.h
```


#### **Código Modificado**

```c
#define UTS_SYSNAME "ZamoraOS"
```

1. Compilación y carga del kernel.
2. Validación del nombre del sistema mediante:
   ```bash
   uname -s
   ```
   Salida:
   ```
   ZamoraOS
   ```


## Implementación de nuevas llamadas al sistema

### **1. `zamora_track_syscall_usage`**

#### **Descripción**
Esta syscall realiza un seguimiento del uso de otras syscalls en el sistema (read, write, open, close). Para cada syscall, almacena la cantidad de veces que ha sido invocada y la última vez que fue utilizada.


- Se utiliza una estructura `syscall_usage` para almacenar las estadísticas:
  ```c
  struct syscall_usage {
      unsigned long count; // Número de invocaciones
      struct timespec64 time_last_used; // Última vez utilizada
  };
  ```

- Se define un arreglo estático que almacena estas estadísticas para un máximo de 1024 syscalls.
- La función `track_syscall(int syscall_id)` es llamada desde diferentes puntos del kernel para actualizar las estadísticas correspondientes.

```c
SYSCALL_DEFINE1(zamora_track_syscall_usage, struct syscall_usage __user *, statistics);
```

**Detalles de `SYSCALL_DEFINE1`:**
- `SYSCALL_DEFINE1` es una macro del kernel que simplifica la definición de syscalls.
  - El `1` indica que la syscall tiene un argumento.
  - El primer parámetro es el nombre de la syscall (`zamora_track_syscall_usage`).
  - Los parámetros adicionales son el tipo y nombre de los argumentos de la syscall (`struct syscall_usage __user *, statistics`).


#### **Implementación**
- Se crea la syscall en **syscall_64.tbl**: `552 common zamora_track_syscall_usage sys_zamora_track_syscall_usage`
- Compilar la syscall en el **Kernel/usac/Makefile** `obj-y += syscall2.o`
- Se crea `syscall2.h`para que mi funcion `track_syscall` sea llamada desde otras syscalls
- Cuando el usuario invoca la syscall, se inicializa el almacenamiento de estadísticas.
- Luego, las estadísticas se copian al espacio de usuario utilizando `copy_to_user`.
- La función `track_syscall` debe ser llamada desde los puntos donde se desee rastrear las syscalls específicas.

### **2. `zamora_capture_memory_snapshot`**

#### **Descripción**
Esta syscall captura el estado actual de la memoria del sistema, incluyendo información sobre páginas libres, caché, activas, inactivas y uso de swap.

- Se utiliza una estructura `memory_snapshot` para almacenar los datos de memoria:
  ```c
  struct memory_snapshot {
      unsigned long total_pages;      // Total de páginas físicas
      unsigned long free_pages;       // Páginas libres
      unsigned long cached_pages;     // Páginas en caché
      unsigned long active_pages;     // Páginas activas
      unsigned long inactive_pages;   // Páginas inactivas
      unsigned long swap_pages;       // Páginas usadas en swap
  };
  ```
```c
SYSCALL_DEFINE1(zamora_capture_memory_snapshot, struct memory_snapshot __user *, user_snapshot);
```

**Detalles de `SYSCALL_DEFINE1`:**
- `SYSCALL_DEFINE1` define una syscall con un solo argumento.
  - `user_snapshot` es un puntero al espacio de usuario donde se almacenarán los datos de la memoria.
- Esta macro genera automáticamente un envoltorio para manejar la transición entre el espacio de usuario y el espacio del kernel.
- Garantiza que los argumentos sean validados antes de que se procese la lógica de la syscall.

#### **Implementación**
- Se crea la syscall en **syscall_64.tbl**: `551 common zamora_capture_memory_snapshot sys_zamora_capture_memory_snapshot`
- Compilar la syscall en el **Kernel/usac/Makefile** `obj-y += syscall1.o`
- La función `get_memory_snapshot` recopila información desde varias métricas del kernel:
  - `totalram_pages()` para el total de páginas.
  - `global_node_page_state(NR_FREE_PAGES)` para páginas libres.
  - `global_node_page_state(NR_FILE_PAGES)` para páginas en caché.
  - `global_node_page_state` con `NR_ACTIVE_ANON` y `NR_ACTIVE_FILE` para páginas activas.
  - `global_node_page_state` con `NR_INACTIVE_ANON` y `NR_INACTIVE_FILE` para páginas inactivas.
  - Información sobre swap se obtiene a través de `si_meminfo`.
- Se utiliza `copy_to_user` para transferir los datos recopilados al espacio de usuario.

### **3. `zamora_get_io_throttle`**

#### **Descripción**
Esta syscall obtiene estadísticas de I/O de un proceso específico, como cantidad de bytes leídos y escritos, tanto a nivel de usuario como en disco.

- Se utiliza una estructura `io_stats` para almacenar las estadísticas:
  ```c
  struct io_stats {
      unsigned long bytes_read;         // Bytes leídos
      unsigned long bytes_written;      // Bytes escritos
      unsigned long bytes_read_disk;    // Bytes leídos desde disco
      unsigned long bytes_written_disk; // Bytes escritos a disco
      unsigned long io_wait_time;       // Tiempo en espera de I/O
  };
  ```

```c
SYSCALL_DEFINE2(zamora_get_io_throttle, pid_t, pid, struct io_stats __user *, stats);
```

**Detalles de `SYSCALL_DEFINE2`:**
- `SYSCALL_DEFINE2` define una syscall con dos argumentos.
  - `pid` es el identificador del proceso cuyos datos de I/O se desean obtener.
  - `stats` es un puntero al espacio de usuario donde se almacenarán las estadísticas.
- Esta macro genera automáticamente el código necesario para manejar la transferencia de argumentos entre el espacio de usuario y el espacio del kernel.

#### **Implementación**
- Se crea la syscall en **syscall_64.tbl**: `553 common zamora_get_io_throttle sys_zamora_get_io_throttle`
- Compilar la syscall en el **Kernel/usac/Makefile** `obj-y += syscall3.o`
- La syscall utiliza `find_task_by_vpid` para localizar el proceso asociado al PID proporcionado.
- Las estadísticas se recopilan desde la estructura `task_struct` del proceso, específicamente de `task->ioac` (task_io_accounting).
- `task_lock` y `task_unlock` aseguran que las estadísticas del proceso no se modifiquen mientras se recopilan.
- Los datos recopilados se copian al espacio de usuario mediante `copy_to_user`.

