Test without passed parameter:

.. code-block:: bash

    / # insmod ./task_03.ko
    [   78.349446] lab_1: loading out-of-tree module taints kernel.
    [   78.356907] <username> wasn't defined:(
    [   78.360180] Hello, $username!
    [   78.360180] jiffies = 4294745540
    [   78.366911] insmod (99) used greatest stack depth: 13928 bytes left
    / # rmmod task_03.ko
    [   89.164170] Long live the Kernel!
    [   89.164170] Working time = 10 sec

Test with passed parameter:

.. code-block:: bash

    / # insmod ./task_03.ko username=shurik
    [  144.366431] Hello, shurik!
    [  144.366431] jiffies = 4294811547
    / # rmmod task_03
    [  148.524403] Long live the Kernel!
    [  148.524403] Working time = 4 sec
