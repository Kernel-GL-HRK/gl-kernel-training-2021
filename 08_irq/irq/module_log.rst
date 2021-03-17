
.. code-block:: bash

	/modules # insmod gpio_irq.ko
	[ 2413.433838] 'gpio_irq' module initialized
	/modules # [ 2414.950501] 'var' state is 1
	[ 2415.087758] 'var' state is 2
	[ 2415.628699] 'var' state is 3
	[ 2415.795473] 'var' state is 4
	[ 2416.050941] 'var' state is 5
	[ 2416.212376] 'var' state is 6
	[ 2416.512007] 'var' state is 7
	[ 2416.632755] 'var' state is 8
	[ 2416.922357] 'var' state is 9
	[ 2417.054116] 'var' state is 10
	[ 2417.416982] 'var' state is 11
	[ 2417.558707] 'var' state is 12
	[ 2417.832376] 'var' state is 13
	[ 2417.996766] 'var' state is 14
	[ 2418.243723] 'var' state is 15
	[ 2418.346213] 'var' state is 16
	[ 2418.559367] 'var' state is 17
	[ 2418.682978] 'var' state is 18
	[ 2418.923579] 'var' state is 19
	[ 2419.028503] 'var' state is 20
	/modules # rmmod gpio_irq 
	[ 2429.045348] 'gpio_irq' module released
	/modules # insmod gpio_irq.ko simulate_busy
	[ 2437.615033] 'gpio_irq' module initialized
	/modules # [ 2439.280515] ############################
	[ 2439.284603] Went to bed:)
	[ 2442.324104] Woke up:)
	[ 2442.326426] 'var' state is 2
	[ 2442.329339] ############################
	[ 2442.333297] Went to bed:)
	[ 2445.364106] Woke up:)
	[ 2445.366424] 'var' state is 4
	[ 2445.369336] ############################
	[ 2445.373291] Went to bed:)
	[ 2448.404094] Woke up:)
	[ 2448.406410] 'var' state is 4
	[ 2452.382962] ############################
	[ 2452.387003] Went to bed:)
	[ 2455.444102] Woke up:)
	[ 2455.446426] 'var' state is 36
	[ 2455.449426] ############################
	[ 2455.453383] Went to bed:)
	[ 2458.484105] Woke up:)
	[ 2458.486419] 'var' state is 40
	[ 2458.489417] ############################
	[ 2458.493372] Went to bed:)
	[ 2461.524091] Woke up:)
	[ 2461.526408] 'var' state is 40
	[ 2462.639543] ############################
	[ 2462.643511] Went to bed:)
	[ 2465.684079] Woke up:)
	[ 2465.686393] 'var' state is 42
	[ 2465.689393] ############################
	[ 2465.693348] Went to bed:)
	[ 2468.724114] Woke up:)
	[ 2468.726449] 'var' state is 42
	[ 2472.730428] ############################
	[ 2472.734474] Went to bed:)
	[ 2475.764098] Woke up:)
	[ 2475.766418] 'var' state is 72
	[ 2475.769418] ############################
	[ 2475.773373] Went to bed:)
	[ 2478.804091] Woke up:)
	[ 2478.806406] 'var' state is 78
	[ 2478.809406] ############################
	[ 2478.813361] Went to bed:)
	[ 2481.844110] Woke up:)
	[ 2481.846425] 'var' state is 78
	/modules # rmmod gpio_irq 
	[ 2499.739920] 'gpio_irq' module released
	/modules # 
