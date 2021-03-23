#!/bin/bash

# 10_cdev task demo script

# Greeting  
echo "GL Kernel Training 2021"
echo "10_cdev task character device demo"

print_help () {
        echo "w - write msg"
        echo "r - read msg"
        echo "c - clear buffer"
        echo "s - get buffer size"
        echo "v - get buffer volume"
        echo "d - print debug info"
        echo "h - print help"
        echo "q - quit"
}

chrdev=/dev/txtmsg0
sys_clear_buffer=/sys/kernel/gl_cdev/buffer_clean
sys_debug=/sys/kernel/gl_cdev/debug
proc_buffer_size=/proc/gl_cdev/buffer_size
proc_buffer_volume=/proc/gl_cdev/buffer_volume
min_buffer_size=32
def_buffer_size=1024

# Enter buffer size loop
while :
do
        # Read desired buffer size
        echo "Enter chrdev buffer size (enter to default 1K): "
        read buffer_size

        if [[ $buffer_size ]] && [ $buffer_size -eq $buffer_size 2>/dev/null ]
        then
                if [ $buffer_size -lt $min_buffer_size ]
                then    
                        echo "Buffer size cannot be less than $min_buffer_size!"
                        continue
                else
                        echo "Setting buffer size to $buffer_size..."
                        break
                fi
        else
                if [[ -z $buffer_size ]]
                then    
                        buffer_size=$def_buffer_size
                        echo "Setting default buffer size..."
                        break
                else
                        echo "Invalid input!"
                        continue 
                fi
        fi
done

# Remove module (if previously not removed)
rmmod cdev_task_mod 2> /dev/null

# Insert kernel module
insmod /komod/cdev_task_mod.ko buffer_size=$buffer_size

print_help

# Enter main loop
while :
do      
        read choise
        case $choise in  
                w)      
                        echo "Enter message: "
                        read input
                        echo $input > $chrdev
                        echo "Message written!"
                ;;
                r)
                        echo $(cat $chrdev)
                ;;
                c)
                        echo 1 > $sys_clear_buffer
                        echo "Buffer has been cleaned!"
                ;;
                s)
                        echo "Buffer size: $(cat $proc_buffer_size) bytes"
                ;;
                v)
                        echo "Buffer volume: $(cat $proc_buffer_volume) bytes"
                ;;
                d)
                        echo -n "$(cat $sys_debug)"
                ;;
                h)
                        print_help
                ;;
                q)
                        rmmod cdev_task_mod 2> /dev/null
                        exit
                ;;
                *)
                        echo "Invalid input!"
                        print_help
                ;;
	esac

        echo ""
done
