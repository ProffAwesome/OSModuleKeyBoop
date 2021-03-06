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

#define KEY_PRESSED 1
#define KEY_RELEASED 0

//Based on this table
//http://www.comptechdoc.org/os/linux/howlinuxworks/linux_hlkeycodes.html
//With all special keys (e.g. alt, ctrl etc.) stripped
static const char* keys[] = { "\0", "", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "", "",
                        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "", "", "a", "s", "d", "f",
                        "g", "h", "j", "k", "l", ";", "'", "`", "", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".",
                        "/", "", "", " "};

static const char* keysShift[] =
                        { "\0", "", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "", "",
                        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "", "", "A", "S", "D", "F",
                        "G", "H", "J", "K", "L", ":", "\"", "~", "", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">",
                        "?", "", "", " "};

struct tty_struct *tty;
bool encode = false;

int hello_notify(struct notifier_block *nb, unsigned long code, void *_param) {
  struct keyboard_notifier_param *param = _param;
  char *output = '\0';
  char *input = '\0';

  //No action is performed on release
  if (!param->down == KEY_PRESSED)
    return NOTIFY_OK;

  if (param->shift == KEY_PRESSED && (int)(param->value) == 107){ //The end key
    encode = !encode;	//start/stop encoding
    printk(KERN_INFO "Encryption set to %s\n", (encode ? "true" : "false"));
  }  

  //If input is a key, encode setting is enabled, and its a letter
  if (code == KBD_KEYCODE && encode) {
    
    if ((int)(param->value) < 57){
      tty_buffer_clear(tty->port);
      if (param->shift == KEY_PRESSED)
        output = keys[(int)(param->value)];
      else
        output = keysShift[(int)(param->value)];
    
      tty->ops->write(tty, output, sizeof(*output)); 
      
      //tty_insert_flip_string(tty->port, output, sizeof(*output));

      //struct tty_ldisk *ld = NULL;
      //tty_flip_buffer_push(tty->port);
      //return NOTIFY_QUIT;
    }
    //printk(KERN_INFO "Buffer: %d==%s\n", (int)(param->value), input);
  }  
  return NOTIFY_OK;
}

static struct notifier_block nb = {
  .notifier_call = hello_notify
};

static int hello_init(void)
{
  //Open console for writing a beep
  tty = get_current_tty();

  printk(KERN_INFO "Opened tty %s with space\n", tty->driver->name);  
 
  /*//Testing purposes:
  char* testput = "tests";
 
  printk(KERN_INFO "Space pre char write: %d\n", tty_buffer_space_avail(tty->port));  
  tty_insert_flip_string(tty->port, testput, sizeof(testput)); 
  printk(KERN_INFO "Space post char write: %d\n", tty_buffer_space_avail(tty->port));
   
  tty_buffer_clear(tty->port);

  printk(KERN_INFO "Space post clear: %d\n", tty_buffer_space_avail(tty->port));
 */
  register_keyboard_notifier(&nb);
  printk(KERN_INFO "Initialized keyboard trace\n");
 // register_keyboard_notifier(&nb);
  return 0;
}

static void hello_release(void)
{
  printk(KERN_INFO "Removed keyboard trace\n");
  unregister_keyboard_notifier(&nb);
  
  /*
  char* testput = "bye";

  printk(KERN_INFO "Space pre char write: %d\n", tty_buffer_space_avail(tty->port));
  tty_insert_flip_string(tty->port, testput, sizeof(testput));
  printk(KERN_INFO "Space post char write: %d\n", tty_buffer_space_avail(tty->port)); */
}

MODULE_LICENSE("GPL");

module_init(hello_init);
module_exit(hello_release);
