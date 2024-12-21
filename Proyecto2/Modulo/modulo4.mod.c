#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x20167f54, "filp_open" },
	{ 0x554df603, "kernel_read" },
	{ 0x54b277c5, "filp_close" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x7f99ca70, "proc_remove" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0x2679eb4d, "kmalloc_caches" },
	{ 0x34a4d2a1, "kmalloc_trace" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x40c7247c, "si_meminfo" },
	{ 0x52e67a18, "kern_path" },
	{ 0x619cb7dd, "simple_read_from_buffer" },
	{ 0x37a0cba, "kfree" },
	{ 0x6d00b59b, "vfs_statfs" },
	{ 0xbad8399d, "path_put" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xcdd3b2be, "proc_create" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x9bb6f1bb, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "BB92FC720B6794CDAD989EF");
