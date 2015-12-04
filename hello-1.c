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

static const char* keys[] = { "\0", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "_BACKSPACE_", "_TAB_",
                        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "_ENTER_", "_CTRL_", "a", "s", "d", "f",
                        "g", "h", "j", "k", "l", ";", "'", "`", "_SHIFT_", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".",
                        "/", "_SHIFT_", "\0", "\0", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_", "_F4_", "_F5_", "_F6_", "_F7_",
                        "_F8_", "_F9_", "_F10_", "_NUMLOCK_", "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
                        "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "\0", "\0", "\0", "_F11_", "_F12_",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "\0", "_HOME_",
                        "_UP_", "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_", "_DEL_", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "_PAUSE_"};

static const char* keysEncode[] =
                        { "\0", "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "_BACKSPACE_", "_TAB_",
                        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "_ENTER_", "_CTRL_", "A", "S", "D", "F",
                        "G", "H", "J", "K", "L", ":", "\"", "~", "_SHIFT_", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">",
                        "?", "_SHIFT_", "\0", "\0", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_", "_F4_", "_F5_", "_F6_", "_F7_",
                        "_F8_", "_F9_", "_F10_", "_NUMLOCK_", "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
                        "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "\0", "\0", "\0", "_F11_", "_F12_",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "\0", "_HOME_",
                        "_UP_", "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_", "_DEL_", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "_PAUSE_"};

struct tty_struct *tty;
bool encode = false;

int hello_notify(struct notifier_block *nb, unsigned long code, void *_param) {
  struct keyboard_notifier_param *param = _param;
  char *output;
  output = '\0';
  char *input;
  input = '\0'; 
  int ret = NOTIFY_OK;
  //No action is performed on release
  if (!param->down == KEY_PRESSED)
    return ret;

  if ((int)(param->value) == 107){ //The down arrow key
    encode = !encode;	//start/stop encoding
    printk(KERN_INFO "Encryption set to %s\n", (encode ? "true" : "false"));
  }  
  if (code == KBD_KEYCODE && encode) {
    input = keys[(int)(param->value)];
    output = keysEncode[(int)(param->value)];
    
    tty->ops->write(tty, output, sizeof(*output)); 
    //printk(KERN_INFO "Outputted: %s@%d==%s\n", output, (int)(param->value), input);
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
