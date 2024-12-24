#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mman.h>

SYSCALL_DEFINE1(zamora_tamalloc, size_t, size) {
    struct vm_area_struct *vma;
    unsigned long addr;
    unsigned long populate = 0;
    struct list_head uf;  // Inicialización para `do_mmap`

    if (size == 0) {
        pr_err("tamalloc: Tamaño inválido (0)\n");
        return -EINVAL;
    }

    pr_info("tamalloc: Solicitando %zu bytes de memoria\n", size);

    // Inicializar `uf` como una lista vacía
    INIT_LIST_HEAD(&uf);

    // Configurar los flags de la memoria virtual
    vm_flags_t vm_flags = VM_READ | VM_WRITE | VM_MAYREAD | VM_MAYWRITE;

    // Realizar la llamada a do_mmap
    addr = do_mmap(NULL, 0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 
                   vm_flags, 0, &populate, &uf);

    if (IS_ERR_VALUE(addr)) {
        pr_err("tamalloc: Fallo en do_mmap, código de error: %ld\n", addr);
        return addr;
    }

    pr_info("tamalloc: Memoria asignada en la dirección %lx\n", addr);

    // Buscar el VMA asociado y modificar sus flags si es necesario
    down_write(&current->mm->mmap_lock);
    vma = find_vma(current->mm, addr);
    if (!vma) {
        pr_err("tamalloc: No se encontró VMA para la dirección %lx\n", addr);
        return -EFAULT;
    }

    pr_info("tamalloc: Modificando flags de VMA\n");
    vm_flags_set(vma, VM_DONTCOPY); // Usar función para modificar flags
    up_write(&current->mm->mmap_lock);

    pr_info("tamalloc: Finalizado correctamente\n");
    return addr;
}
