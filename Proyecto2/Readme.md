# Proyecto 2 Sistemas Operativo 2
## DOTITULO


### **1. Syscall `tamalloc`**

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

