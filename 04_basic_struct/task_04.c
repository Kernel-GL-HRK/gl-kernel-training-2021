#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_DESCRIPTION("Basic module demo: sysfs, kobj, linked_list");
MODULE_AUTHOR("AlexShlikhta");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

#define CLASS_ATTR(_name, _mode, _show, _store) \
struct class_attribute class_attr_##_name = __ATTR(_name, _mode, _show, _store)

#define MAX_LENGTH 100


static LIST_HEAD(my_list);

struct _tmp_head {
    struct list_head head;
    char text[MAX_LENGTH];
};

static struct _tmp_head *tmp;
static char text_buf[ MAX_LENGTH ];
static struct class *hello_class;

///////////////////////////////////////////////////////////////////////////////


/* sysfs show() method. Calls the show() method corresponding to the individual sysfs file */
static ssize_t hello_show( struct class *class, struct class_attribute *attr, char *buf ) {
   
    if( list_empty(&my_list) ) {
        sprintf(buf, "List is empty:(\n");
    }
    else {
        struct _tmp_head *tmp;

        list_for_each_entry (tmp, &my_list, head) {
            pr_info("%s\n", tmp->text);
        }
        sprintf(buf, "List has printed successfuly:)\n");
    }

    return strlen( buf );
}

///////////////////////////////////////////////////////////////////////////////


/* sysfs store() method. Calls the store() method corresponding to the individual sysfs file */
static ssize_t hello_store( struct class *class, struct class_attribute *attr, const char *buf, size_t count ) {

    strncpy( text_buf, buf, count );
    text_buf[ count ] = '\0';

    tmp = kzalloc( sizeof(*tmp), GFP_KERNEL );
    strcpy( tmp->text, text_buf );
    list_add_tail( &(tmp->head), &my_list );   

    pr_info("Word '%s' has written to list successfuly\n", text_buf);

    return count;
}

CLASS_ATTR( list, ( S_IWUSR | S_IRUGO ), &hello_show, &hello_store );

///////////////////////////////////////////////////////////////////////////////
//***************************INIT CALLBACK*************************************
///////////////////////////////////////////////////////////////////////////////

int __init sysfs_mod_init(void) {

   int res;
   hello_class = class_create( THIS_MODULE, "hello-class" );
   if( IS_ERR( hello_class ) ) printk( "bad class create\n" );
   res = class_create_file( hello_class, &class_attr_list );

   pr_info("'task_04' module initialized\n" );
   return res;
}

///////////////////////////////////////////////////////////////////////////////
//***************************EXIT CALLBACK*************************************
///////////////////////////////////////////////////////////////////////////////

void sysfs_mod_cleanup(void) {

    struct list_head *pos = NULL;

    list_for_each(pos, &my_list) {
            struct _tmp_head *tmp = list_entry(pos, struct _tmp_head, head);
            list_del(pos);
            kfree(tmp);
    } 

    class_remove_file( hello_class, &class_attr_list );
    class_destroy( hello_class );
}

module_init( sysfs_mod_init );
module_exit( sysfs_mod_cleanup );
