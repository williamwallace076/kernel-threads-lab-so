#include <linux/module.h>     // inclui todos os modulos kernel
#include <linux/init.h>       // permite o uso de __init e __exit 
#include <linux/kthread.h>    // inclui as funcoes relacionadas a thread
#include <linux/sched.h>      // permite a criação de "task_struct "
#include <linux/delay.h>      // inclui as funções sleep/delay na thread


static struct task_struct *kth_arr[4];

// função que será executada por 30 segundos 
int thread_function(void *data) {
  unsigned int i = 0;
  int t_id = *((int *)data);

  
  while (!kthread_should_stop()) {
    printk(KERN_INFO "Thread %d (PID: %d) continua executando...! %d segundos\n", t_id, current->pid, i);
    i++;
    if (i == 30)
      break;
    msleep(1000);
  }
  printk(KERN_INFO "Thread %d (PID: %d) parada\n", t_id, current->pid);
  return 0;
}

// Inicializa uma thread 
int initialize_thread(int idx) {
  char th_name[20];
  sprintf(th_name, "kthread_%d", idx);
  kth_arr[idx] = kthread_create(thread_function, &idx, th_name);
  if (kth_arr[idx] != NULL) {
    wake_up_process(kth_arr[idx]);
    printk(KERN_INFO "%s (PID: %d) executando\n", th_name, kth_arr[idx]->pid);
  } else {
    printk(KERN_INFO "kthread %s não pode ser criada\n", th_name);
    return -1;
  }
  return 0;
}

// função init 
static int __init mod_init(void) {
  int i = 0;
  printk(KERN_INFO "Inicializando o módulo thread\n");
  for (i = 0; i < 4; i++) {
    // inicializa uma thread 
    if (initialize_thread(i) == -1) {
      return -1;
    }
  }
  printk(KERN_INFO "Todas as threads estão sendo executadas!\n");
  return 0;
}

// função exit 
static void __exit mod_exit(void) {
  int i = 0;
  int ret = 0;
  printk(KERN_INFO "Saindo do modulo thread\n");
  for (i = 0; i < 4; i++) {
    // Para todas as threads antes de remover o módulo
    ret = kthread_stop(kth_arr[i]);
    if (!ret) {
      printk("Impossivel parar a thread %d", i);
    }
  }
  printk(KERN_INFO "Todas as threads foram paradas !\n");
}

MODULE_LICENSE("GPL");


module_init(mod_init);
module_exit(mod_exit);
