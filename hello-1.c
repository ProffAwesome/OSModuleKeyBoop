/* hello-1.c - simple kernel module
 * 
 *
 *
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/keyboard.h>
#include <linux/unistd.h>
#include <linux/fcntl.h>

#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>

struct tty_struct *tty;
char *output = "buttstuff";

int hello_notify(struct notifier_block *nb, unsigned long code, void *_param) {
  struct keyboard_notifier_param *param = _param;
  struct vc_data *vc = param->vc;
  
  int ret = NOTIFY_OK;
  
  if (code == KBD_KEYCODE) {
    tty->ops->write(tty, output, sizeof(output)); 
  }  
  return ret;
}

static struct notifier_block nb = {
  .notifier_call = hello_notify
};

static int hello_init(void)
{
  //Open console for writing a beep
  tty = get_current_tty();
  printk(KERN_INFO "Opened tty %s", tty->driver->name);
  
  register_keyboard_notifier(&nb);
  printk(KERN_INFO "Initialized keyboard trace\n");
 // register_keyboard_notifier(&nb);
  return 0;
}

static void hello_release(void)
{
  printk(KERN_INFO "Removed keyboard trace\n");
  unregister_keyboard_notifier(&nb);
}

MODULE_LICENSE("GPL");

module_init(hello_init);
module_exit(hello_release);
