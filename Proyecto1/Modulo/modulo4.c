#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/statfs.h>
#include <linux/namei.h>
static struct proc_dir_entry *entry;
static char *partition = "/"; 

// Calcula el porcentaje de uso de CPU
static unsigned long calculate_cpu_usage(void) {
    static unsigned long prev_idle, prev_total;
    unsigned long idle, total, usage;
    unsigned long delta_idle, delta_total;
    char buf[128];
    unsigned long user, nice, system, irq, softirq, steal;

    struct file *file = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(file)) {
        return 0;
    }

    kernel_read(file, buf, sizeof(buf) - 1, &file->f_pos);
    filp_close(file, NULL);

    sscanf(buf, "cpu  %lu %lu %lu %lu %lu %lu %lu", &user, &nice, &system, &idle, &irq, &softirq, &steal);
    total = user + nice + system + idle + irq + softirq + steal;

    delta_idle = idle - prev_idle;
    delta_total = total - prev_total;
    prev_idle = idle;
    prev_total = total;

    if (delta_total == 0) {
        return 0;
    }

    usage = 100 * (delta_total - delta_idle) / delta_total;
    return usage;
}

static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct sysinfo mem_info;
    struct kstatfs stat;
    struct path path;
    unsigned long cpu_usage;
    char *output;
    size_t len = 0;
    int ret;

    output = kzalloc(4096, GFP_KERNEL);
    if (!output)
        return -ENOMEM;

    // Estadísticas de CPU
    cpu_usage = calculate_cpu_usage();
    len += snprintf(output + len, 4096 - len, "Porcentaje de uso de CPU: %lu%%\n", cpu_usage);

    // Estadísticas de memoria
    si_meminfo(&mem_info);
    len += snprintf(output + len, 4096 - len, "Memoria Total: %lu kB\n", mem_info.totalram << (PAGE_SHIFT - 10));
    len += snprintf(output + len, 4096 - len, "Memoria Disponible: %lu kB\n", mem_info.freeram << (PAGE_SHIFT - 10));

    // Estadísticas de almacenamiento
    ret = kern_path(partition, LOOKUP_FOLLOW, &path); 
    if (ret == 0) {
        if (vfs_statfs(&path, &stat) == 0) {
            len += snprintf(output + len, 4096 - len, "Espacio Total en %s: %llu MB\n", partition,
                            (unsigned long long)(stat.f_blocks * stat.f_bsize) >> 20);
            len += snprintf(output + len, 4096 - len, "Espacio Libre en %s: %llu MB\n", partition,
                            (unsigned long long)(stat.f_bfree * stat.f_bsize) >> 20);
        } else {
            len += snprintf(output + len, 4096 - len, "Error al obtener estadísticas de %s\n", partition);
        }
        path_put(&path);
    } else {
        len += snprintf(output + len, 4096 - len, "Error al resolver la ruta %s\n", partition);
    }

    // Copia los datos al espacio de usuario
    ssize_t ret_val = simple_read_from_buffer(buf, count, pos, output, len);
    kfree(output);
    return ret_val;
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
};

static int __init modulo4_init(void) {
    entry = proc_create("modulo4", 0444, NULL, &proc_fops);
    if (!entry) {
        return -ENOMEM;
    }

    printk(KERN_INFO "Módulo modulo4 cargado\n");
    return 0;
}

static void __exit modulo4_exit(void) {
    proc_remove(entry);
    printk(KERN_INFO "Módulo modulo4 descargado\n");
}

module_init(modulo4_init);
module_exit(modulo4_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zamora");
MODULE_DESCRIPTION("Módulo para mostrar estadísticas del sistema");
