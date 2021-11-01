#include "my_info.h"

//define procfile variable
#define PROC_NAME "my_info"//procfilename 

//Version show
static void version_show(struct seq_file *m)
{
    seq_puts(m, "\n=============Version=============\n");
    seq_printf(m, "Linux version %s\n", UTS_RELEASE);
    seq_puts(m, "\n=============CPU=============\n");
}

//define CPU show
static void cpu_show(struct seq_file *m, struct cpuinfo_x86 *c, unsigned int cpu)
{
    //put CPU information
    seq_printf(m, "processor\t\t: %u\n"
               "model name\t\t: %s\n"
               "physical id\t\t: %d\n"
               "core id\t\t\t: %d\n"
               "cpu cores\t\t: %d\n"
               "cache size\t\t: %u KB\n"
               "clflush size\t: %u\n"
               "cache_alignment\t\t: %d\n"
               "address sizes\t\t: %u bits physical, %u bits virtual\n\n"
               , cpu, c->x86_model_id[0] ? c->x86_model_id : "unknown", c->phys_proc_id
               , c->cpu_core_id, c->booted_cores, c->x86_cache_size, c->x86_clflush_size
               , c->x86_cache_alignment, c->x86_phys_bits, c->x86_virt_bits);
}

//define memory and time show
static void other_show(struct seq_file *m)
{
    //Memory variable
    struct sysinfo my_sys;
    unsigned long pages[NR_LRU_LISTS];
    int lru;//count
    //Time variable
    struct timespec64 uptime;
    struct timespec64 idle;
    u64 nsec;
    u32 rem;
    int i;
    //get my_sys
    si_meminfo(&my_sys);
    //get other memory
    for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
        pages[lru] = global_node_page_state(NR_LRU_BASE + lru);
    //get Uptime Idletime
    nsec = 0;
    for_each_possible_cpu(i)
    nsec += (__force u64) kcpustat_cpu(i).cpustat[CPUTIME_IDLE];
    get_monotonic_boottime(&uptime);
    idle.tv_sec = div_u64_rem(nsec, NSEC_PER_SEC, &rem);
    idle.tv_nsec = rem;

    seq_puts(m, "\n=============Memory=============\n");
    seq_printf(m, "MemTotal\t\t: %8lu kB\n", my_sys.totalram << (PAGE_SHIFT - 10));
    seq_printf(m, "MemFree\t\t\t: %8lu kB\n", my_sys.freeram << (PAGE_SHIFT - 10));
    seq_printf(m, "Buffers\t\t\t: %8lu kB\n", my_sys.bufferram << (PAGE_SHIFT - 10));
    seq_printf(m, "Active\t\t\t: %8lu kB\n", (pages[LRU_ACTIVE_ANON] + pages[LRU_ACTIVE_FILE])<< (PAGE_SHIFT - 10));
    seq_printf(m, "Inactive\t\t: %8lu kB\n", (pages[LRU_INACTIVE_ANON] + pages[LRU_INACTIVE_FILE]) << (PAGE_SHIFT - 10));
    seq_printf(m, "Shmem\t\t\t: %8lu kB\n", my_sys.sharedram << (PAGE_SHIFT - 10));
    seq_printf(m, "Dirty\t\t\t: %8lu kB\n", global_node_page_state(NR_FILE_DIRTY) << (PAGE_SHIFT - 10));
    seq_printf(m, "Writeback\t\t: %8lu kB\n", global_node_page_state(NR_WRITEBACK) << (PAGE_SHIFT - 10));
    seq_printf(m, "KernelStack\t\t: %8lu kB\n", global_zone_page_state(NR_KERNEL_STACK_KB));
    seq_printf(m, "PageTables\t\t: %8lu kB\n", global_zone_page_state(NR_PAGETABLE) << (PAGE_SHIFT - 10));
    seq_puts(m, "\n=============Time=============\n");
    seq_printf(m, "Uptime\t\t\t: %lu.%02lu(s)\nIdletime\t\t: %lu.%02lu(s)\n"
               , (unsigned long) uptime.tv_sec, (uptime.tv_nsec / (NSEC_PER_SEC / 100))
               , (unsigned long) idle.tv_sec, (idle.tv_nsec / (NSEC_PER_SEC / 100)));
}

// This function is called for each "step" of a sequence.
static int my_info_show(struct seq_file *m, void *v)
{
    //CPU variable
    struct cpuinfo_x86* c = v;
    unsigned int cpu = c -> cpu_index;

    if(cpu == 0)
        version_show(m);
    cpu_show(m, c, cpu);
    return 0;
}

//Called at the begining of a sequence.
static void *my_info_start(struct seq_file *m, loff_t *pos)
{
    *pos = cpumask_next(*pos - 1, cpu_online_mask);
    if ((*pos) < nr_cpu_ids)
        return &cpu_data(*pos);
    return NULL;
}

static void *my_info_next(struct seq_file *m, void *v, loff_t *pos)
{
    (*pos)++;
    return my_info_start(m, pos);
}

// This function is called at the end of a sequence.
static void my_info_stop(struct seq_file *m, void *v)
{
    other_show(m);
    //nothing to do, we use a static value in start()
}

// This structure gather "function" to manage the sequence
const struct seq_operations my_info_ops =
{
    .start = my_info_start,
    .next = my_info_next,
    .stop = my_info_stop,
    .show = my_info_show,
};

// This function is called when the /proc file is open.
static int procfile_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &my_info_ops);
}

static const struct file_operations my_file_ops =
{
    .open = procfile_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

//init module
static int __init my_info_init(void)
{
    struct proc_dir_entry *entry;
    entry = proc_create(PROC_NAME, 0644, NULL, &my_file_ops);
    if (entry == NULL)
    {
        remove_proc_entry(PROC_NAME, NULL);
        pr_debug("Error: Could not build /proc/%s.\n", PROC_NAME);
        return -ENOMEM;
    }
    return 0;
}

//exit module
static void __exit my_info_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk("/proc/%s removed\n", PROC_NAME);
}

module_init(my_info_init);
module_exit(my_info_exit);

//module license declaration
MODULE_LICENSE("GPL");
