
.. code-block:: bash

    /modules # insmod task_04.ko
    [ 5774.850537] 'task_04' module initialized
    /modules # cat /sys/class/hello-class/list
    List is empty:(
    /modules # echo -n lol > /sys/class/hello-class/list
    [ 5795.114872] Word 'lol' has written to list successfuly
    /modules # echo -n kek > /sys/class/hello-class/list
    [ 5802.656205] Word 'kek' has written to list successfuly
    /modules # cat /sys/class/hello-class/list
    [ 5805.953817] lol
    [ 5805.955827] kek
    List has printed successfuly:)
    /modules # echo -n cheburek > /sys/class/hello-class/list
    [ 5816.377714] Word 'cheburek' has written to list successfuly
    /modules # cat /sys/class/hello-class/list
    [ 5820.564927] lol
    [ 5820.566726] kek
    [ 5820.568489] cheburek
    List has printed successfuly:)
    /modules #
