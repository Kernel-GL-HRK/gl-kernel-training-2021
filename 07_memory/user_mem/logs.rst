
.. code-block:: bash

    oshlikhta@kbp1-lhp-a10111:~/pro_camp/task_7/user_mem$ make build                                                                                                                                                                  
    gcc -Wall -Werror -Wpedantic -O2 -std=gnu11 -I inc -c user_mem.c -o user_mem.o
    gcc -o usermem user_mem.o 
    oshlikhta@kbp1-lhp-a10111:~/pro_camp/task_7/user_mem$ ./usermem 
    ************************************************
    *****************MALLOC TESTING*****************
    ****************size = (2^x)+0******************
    ************************************************
    Iter No0
    Start adress is 0x55da386e86b0
    Alloc time = 605ns      Free time = 443ns
    Iter No1
    Start adress is 0x55da386e86b0
    Alloc time = 283ns      Free time = 154ns
    Iter No2
    Start adress is 0x55da386e86b0
    Alloc time = 105ns      Free time = 102ns
    Iter No3
    Start adress is 0x55da386e86b0
    Alloc time = 119ns      Free time = 103ns
    Iter No4
    Start adress is 0x55da386e86b0
    Alloc time = 259ns      Free time = 102ns
    Iter No5
    Start adress is 0x55da386e86d0
    Alloc time = 228ns      Free time = 101ns
    Iter No6
    Start adress is 0x55da386e8700
    Alloc time = 163ns      Free time = 111ns
    Iter No7
    Start adress is 0x55da386e8750
    Alloc time = 320ns      Free time = 104ns
    Iter No8
    Start adress is 0x55da386e87e0
    Alloc time = 390ns      Free time = 156ns
    Iter No9
    Start adress is 0x55da386e88f0
    Alloc time = 255ns      Free time = 105ns
    Iter No10
    Start adress is 0x55da386e8b00
    Alloc time = 541ns      Free time = 239ns
    Iter No11
    Start adress is 0x55da386e8f10
    Alloc time = 9986ns     Free time = 1033ns
    Iter No12
    Start adress is 0x55da386e8f10
    Alloc time = 396ns      Free time = 347ns
    Iter No13
    Start adress is 0x55da386e8f10
    Alloc time = 7143ns     Free time = 402ns
    Iter No14
    Start adress is 0x55da386e8f10
    Alloc time = 5378ns     Free time = 379ns
    Iter No15
    Start adress is 0x55da386e8f10
    Alloc time = 5124ns     Free time = 220ns
    Iter No16
    Start adress is 0x55da386e8f10
    Alloc time = 4019ns     Free time = 127ns
    Iter No17
    Start adress is 0x55da386e8f10
    Alloc time = 3867ns     Free time = 142ns
    Iter No18
    Start adress is 0x7f726a8e8010
    Alloc time = 13642ns    Free time = 18879ns
    Iter No19
    Start adress is 0x7f726a8a8010
    Alloc time = 6988ns     Free time = 6579ns
    Iter No20
    Start adress is 0x7f726a828010
    Alloc time = 5667ns     Free time = 6337ns
    Iter No21
    Start adress is 0x7f726a12b010
    Alloc time = 9286ns     Free time = 9949ns
    Iter No22
    Start adress is 0x7f7269f2b010
    Alloc time = 7467ns     Free time = 7952ns
    Iter No23
    Start adress is 0x7f7269b2b010
    Alloc time = 7485ns     Free time = 7512ns
    Iter No24
    Start adress is 0x7f726932b010
    Alloc time = 6824ns     Free time = 7490ns
    Iter No25
    Start adress is 0x7f726832b010
    Alloc time = 6986ns     Free time = 7713ns
    Iter No26
    Start adress is 0x7f726632b010
    Alloc time = 6958ns     Free time = 8045ns
    Iter No27
    Start adress is 0x7f726232b010
    Alloc time = 6757ns     Free time = 8786ns
    Iter No28
    Start adress is 0x7f725a32b010
    Alloc time = 11768ns    Free time = 17774ns
    Iter No29
    Start adress is 0x7f724a32b010
    Alloc time = 15740ns    Free time = 17250ns
    Iter No30
    Start adress is 0x7f722a32b010
    Alloc time = 10529ns    Free time = 23359ns
    Iter No31
    Start adress is 0x7f71ea32b010
    Alloc time = 9727ns     Free time = 22682ns
    Iter No32
    Start adress is 0x7f716a32b010
    Alloc time = 9012ns     Free time = 22340ns
    Iter No33
    Start adress is 0x7f706a32b010
    Alloc time = 8067ns     Free time = 21266ns
    Iter No34
    Start adress is 0x7f6e6a32b010
    Alloc time = 9309ns     Free time = 22608ns
    Iter No35
    Can't allocate:(
    Aver allocating time is 3037ns  Aver freeing time is 3823ns
    ************************************************
    *****************CALLOC TESTING*****************
    ****************size = (2^x)+0******************
    ************************************************
    Iter No0
    Start adress is 0x55da386e8f10
    Alloc time = 1193ns     Free time = 278ns
    Iter No1
    Start adress is 0x55da386e8f30
    Alloc time = 249ns      Free time = 123ns
    Iter No2
    Start adress is 0x55da386e8f50
    Alloc time = 250ns      Free time = 108ns
    Iter No3
    Start adress is 0x55da386e8f70
    Alloc time = 171ns      Free time = 99ns
    Iter No4
    Start adress is 0x55da386e8f90
    Alloc time = 296ns      Free time = 138ns
    Iter No5
    Start adress is 0x55da386e8fb0
    Alloc time = 297ns      Free time = 103ns
    Iter No6
    Start adress is 0x55da386e8fe0
    Alloc time = 444ns      Free time = 197ns
    Iter No7
    Start adress is 0x55da386e9030
    Alloc time = 897ns      Free time = 101ns
    Iter No8
    Start adress is 0x55da386e90c0
    Alloc time = 391ns      Free time = 145ns
    Iter No9
    Start adress is 0x55da386e91d0
    Alloc time = 474ns      Free time = 158ns
    Iter No10
    Start adress is 0x55da386e93e0
    Alloc time = 495ns      Free time = 101ns
    Iter No11
    Start adress is 0x55da386e97f0
    Alloc time = 635ns      Free time = 549ns
    Iter No12
    Start adress is 0x55da386e97f0
    Alloc time = 719ns      Free time = 121ns
    Iter No13
    Start adress is 0x55da386e97f0
    Alloc time = 6092ns     Free time = 123ns
    Iter No14
    Start adress is 0x55da386e97f0
    Alloc time = 5595ns     Free time = 188ns
    Iter No15
    Start adress is 0x55da386e97f0
    Alloc time = 20141ns    Free time = 264ns
    Iter No16
    Start adress is 0x55da386e97f0
    Alloc time = 39768ns    Free time = 264ns
    Iter No17
    Start adress is 0x55da386e97f0
    Alloc time = 86828ns    Free time = 259ns
    Iter No18
    Start adress is 0x55da386e97f0
    Alloc time = 184787ns   Free time = 285ns
    Iter No19
    Start adress is 0x55da386e97f0
    Alloc time = 49129ns    Free time = 296ns
    Iter No20
    Start adress is 0x55da386e97f0
    Alloc time = 522812ns   Free time = 304ns
    Iter No21
    Start adress is 0x55da386e97f0
    Alloc time = 755563ns   Free time = 379ns
    Iter No22
    Start adress is 0x55da386e97f0
    Alloc time = 1424759ns  Free time = 274ns
    Iter No23
    Start adress is 0x55da386e97f0
    Alloc time = 2848130ns  Free time = 595ns
    Iter No24
    Start adress is 0x55da386e97f0
    Alloc time = 5688293ns  Free time = 605ns
    Iter No25
    Start adress is 0x7f726832b010
    Alloc time = 23816ns    Free time = 21348ns
    Iter No26
    Start adress is 0x7f726632b010
    Alloc time = 7948ns     Free time = 9389ns
    Iter No27
    Start adress is 0x7f726232b010
    Alloc time = 7173ns     Free time = 8946ns
    Iter No28
    Start adress is 0x7f725a32b010
    Alloc time = 7231ns     Free time = 12318ns
    Iter No29
    Start adress is 0x7f724a32b010
    Alloc time = 11210ns    Free time = 15863ns
    Iter No30
    Start adress is 0x7f722a32b010
    Alloc time = 13363ns    Free time = 23214ns
    Iter No31
    Start adress is 0x7f71ea32b010
    Alloc time = 13455ns    Free time = 25643ns
    Iter No32
    Start adress is 0x7f716a32b010
    Alloc time = 12600ns    Free time = 24615ns
    Iter No33
    Start adress is 0x7f706a32b010
    Alloc time = 8625ns     Free time = 21592ns
    Iter No34
    Start adress is 0x7f6e6a32b010
    Alloc time = 7645ns     Free time = 21546ns
    Iter No35
    Can't allocate:(
    Aver allocating time is 186531ns        Aver freeing time is 3024ns
    ************************************************
    *****************MALLOC TESTING*****************
    ****************size = (2^x)+1******************
    ************************************************
    Iter No0
    Start adress is 0x55da386e8f70
    Alloc time = 322ns      Free time = 163ns
    Iter No1
    Start adress is 0x55da386e8f70
    Alloc time = 208ns      Free time = 169ns
    Iter No2
    Start adress is 0x55da386e8f70
    Alloc time = 203ns      Free time = 99ns
    Iter No3
    Start adress is 0x55da386e8f70
    Alloc time = 179ns      Free time = 98ns
    Iter No4
    Start adress is 0x55da386e8f70
    Alloc time = 309ns      Free time = 102ns
    Iter No5
    Start adress is 0x55da386e8fb0
    Alloc time = 179ns      Free time = 104ns
    Iter No6
    Start adress is 0x55da386e8fe0
    Alloc time = 125ns      Free time = 103ns
    Iter No7
    Start adress is 0x55da386e9030
    Alloc time = 195ns      Free time = 102ns
    Iter No8
    Start adress is 0x55da386e90c0
    Alloc time = 143ns      Free time = 104ns
    Iter No9
    Start adress is 0x55da386e91d0
    Alloc time = 143ns      Free time = 103ns
    Iter No10
    Start adress is 0x55da386e93e0
    Alloc time = 300ns      Free time = 104ns
    Iter No11
    Start adress is 0x55da386e97f0
    Alloc time = 1498ns     Free time = 282ns
    Iter No12
    Start adress is 0x55da386e97f0
    Alloc time = 357ns      Free time = 117ns
    Iter No13
    Start adress is 0x55da386e97f0
    Alloc time = 435ns      Free time = 263ns
    Iter No14
    Start adress is 0x55da386e97f0
    Alloc time = 412ns      Free time = 167ns
    Iter No15
    Start adress is 0x55da386e97f0
    Alloc time = 408ns      Free time = 125ns
    Iter No16
    Start adress is 0x55da386e97f0
    Alloc time = 264ns      Free time = 120ns
    Iter No17
    Start adress is 0x55da386e97f0
    Alloc time = 305ns      Free time = 118ns
    Iter No18
    Start adress is 0x55da386e97f0
    Alloc time = 323ns      Free time = 115ns
    Iter No19
    Start adress is 0x55da386e97f0
    Alloc time = 230ns      Free time = 115ns
    Iter No20
    Start adress is 0x55da386e97f0
    Alloc time = 218ns      Free time = 115ns
    Iter No21
    Start adress is 0x55da386e97f0
    Alloc time = 430ns      Free time = 120ns
    Iter No22
    Start adress is 0x55da386e97f0
    Alloc time = 283ns      Free time = 236ns
    Iter No23
    Start adress is 0x55da386e97f0
    Alloc time = 249ns      Free time = 122ns
    Iter No24
    Start adress is 0x55da386e97f0
    Alloc time = 358ns      Free time = 199ns
    Iter No25
    Start adress is 0x7f726832b010
    Alloc time = 12011ns    Free time = 10309ns
    Iter No26
    Start adress is 0x7f726632b010
    Alloc time = 10843ns    Free time = 9781ns
    Iter No27
    Start adress is 0x7f726232b010
    Alloc time = 7368ns     Free time = 9027ns
    Iter No28
    Start adress is 0x7f725a32b010
    Alloc time = 6801ns     Free time = 10532ns
    Iter No29
    Start adress is 0x7f724a32b010
    Alloc time = 6946ns     Free time = 13870ns
    Iter No30
    Start adress is 0x7f722a32b010
    Alloc time = 7935ns     Free time = 21585ns
    Iter No31
    Start adress is 0x7f71ea32b010
    Alloc time = 7970ns     Free time = 21280ns
    Iter No32
    Start adress is 0x7f716a32b010
    Alloc time = 13320ns    Free time = 24270ns
    Iter No33
    Start adress is 0x7f706a32b010
    Alloc time = 7932ns     Free time = 21910ns
    Iter No34
    Start adress is 0x7f6e6a32b010
    Alloc time = 8017ns     Free time = 21297ns
    Iter No35
    Can't allocate:(
    Aver allocating time is 1543ns  Aver freeing time is 2655ns
    ************************************************
    *****************CALLOC TESTING*****************
    ****************size = (2^x)+1******************
    ************************************************
    Iter No0
    Start adress is 0x55da386e8260
    Alloc time = 853ns      Free time = 202ns
    Iter No1
    Start adress is 0x55da386e97f0
    Alloc time = 557ns      Free time = 148ns
    Iter No2
    Start adress is 0x55da386e9810
    Alloc time = 380ns      Free time = 309ns
    Iter No3
    Start adress is 0x55da386e9810
    Alloc time = 478ns      Free time = 233ns
    Iter No4
    Start adress is 0x55da386e9810
    Alloc time = 457ns      Free time = 219ns
    Iter No5
    Start adress is 0x55da386e9830
    Alloc time = 595ns      Free time = 286ns
    Iter No6
    Start adress is 0x55da386e9860
    Alloc time = 420ns      Free time = 236ns
    Iter No7
    Start adress is 0x55da386e98b0
    Alloc time = 1038ns     Free time = 174ns
    Iter No8
    Start adress is 0x55da386e9940
    Alloc time = 677ns      Free time = 186ns
    Iter No9
    Start adress is 0x55da386e9a50
    Alloc time = 571ns      Free time = 260ns
    Iter No10
    Start adress is 0x55da386e9c60
    Alloc time = 1311ns     Free time = 113ns
    Iter No11
    Start adress is 0x55da386ea070
    Alloc time = 634ns      Free time = 246ns
    Iter No12
    Start adress is 0x55da386ea070
    Alloc time = 848ns      Free time = 176ns
    Iter No13
    Start adress is 0x55da386ea070
    Alloc time = 1449ns     Free time = 187ns
    Iter No14
    Start adress is 0x55da386ea070
    Alloc time = 2024ns     Free time = 181ns
    Iter No15
    Start adress is 0x55da386ea070
    Alloc time = 3637ns     Free time = 198ns
    Iter No16
    Start adress is 0x55da386ea070
    Alloc time = 8598ns     Free time = 175ns
    Iter No17
    Start adress is 0x55da386ea070
    Alloc time = 16735ns    Free time = 140ns
    Iter No18
    Start adress is 0x55da386ea070
    Alloc time = 35000ns    Free time = 182ns
    Iter No19
    Start adress is 0x55da386ea070
    Alloc time = 65441ns    Free time = 175ns
    Iter No20
    Start adress is 0x55da386ea070
    Alloc time = 138779ns   Free time = 153ns
    Iter No21
    Start adress is 0x55da386ea070
    Alloc time = 267413ns   Free time = 145ns
    Iter No22
    Start adress is 0x55da386ea070
    Alloc time = 569569ns   Free time = 244ns
    Iter No23
    Start adress is 0x55da386ea070
    Alloc time = 1252329ns  Free time = 619ns
    Iter No24
    Start adress is 0x55da386ea070
    Alloc time = 12690595ns Free time = 858ns
    Iter No25
    Start adress is 0x7f726832b010
    Alloc time = 22497ns    Free time = 35295ns
    Iter No26
    Start adress is 0x7f726632b010
    Alloc time = 10502ns    Free time = 10393ns
    Iter No27
    Start adress is 0x7f726232b010
    Alloc time = 9991ns     Free time = 9548ns
    Iter No28
    Start adress is 0x7f725a32b010
    Alloc time = 6490ns     Free time = 7815ns
    Iter No29
    Start adress is 0x7f724a32b010
    Alloc time = 7304ns     Free time = 13522ns
    Iter No30
    Start adress is 0x7f722a32b010
    Alloc time = 9814ns     Free time = 22757ns
    Iter No31
    Start adress is 0x7f71ea32b010
    Alloc time = 6229ns     Free time = 14941ns
    Iter No32
    Start adress is 0x7f716a32b010
    Alloc time = 5457ns     Free time = 14139ns
    Iter No33
    Start adress is 0x7f706a32b010
    Alloc time = 4822ns     Free time = 14107ns
    Iter No34
    Start adress is 0x7f6e6a32b010
    Alloc time = 4940ns     Free time = 14118ns
    Iter No35
    Can't allocate:(
    Aver allocating time is 240451ns        Aver freeing time is 2582ns
    ************************************************
    *****************ALLOCA TESTING*****************
    ****************size = (2^x)+0******************
    ************************************************
    Iter No0
    Start adress is 0x7fffb2149340
    Alloc time = 139ns      Free time = 0ns
    Iter No1
    Start adress is 0x7fffb2149330
    Alloc time = 74ns       Free time = 0ns
    Iter No2
    Start adress is 0x7fffb2149330
    Alloc time = 71ns       Free time = 0ns
    Iter No3
    Start adress is 0x7fffb2149330
    Alloc time = 71ns       Free time = 0ns
    Iter No4
    Start adress is 0x7fffb2149330
    Alloc time = 53ns       Free time = 0ns
    Iter No5
    Start adress is 0x7fffb2149320
    Alloc time = 53ns       Free time = 0ns
    Iter No6
    Start adress is 0x7fffb2149300
    Alloc time = 53ns       Free time = 0ns
    Iter No7
    Start adress is 0x7fffb21492c0
    Alloc time = 57ns       Free time = 0ns
    Iter No8
    Start adress is 0x7fffb2149240
    Alloc time = 54ns       Free time = 0ns
    Iter No9
    Start adress is 0x7fffb2149140
    Alloc time = 54ns       Free time = 0ns
    Iter No10
    Start adress is 0x7fffb2148f40
    Alloc time = 53ns       Free time = 0ns
    Iter No11
    Start adress is 0x7fffb2148b40
    Alloc time = 54ns       Free time = 0ns
    Iter No12
    Start adress is 0x7fffb2148340
    Alloc time = 60ns       Free time = 0ns
    Iter No13
    Start adress is 0x7fffb2147340
    Alloc time = 5153ns     Free time = 0ns
    Iter No14
    Start adress is 0x7fffb2145340
    Alloc time = 2554ns     Free time = 0ns
    Iter No15
    Start adress is 0x7fffb2141340
    Alloc time = 4066ns     Free time = 0ns
    Iter No16
    Start adress is 0x7fffb2139340
    Alloc time = 3576ns     Free time = 0ns
    Iter No17
    Start adress is 0x7fffb2129340
    Alloc time = 5675ns     Free time = 0ns
    Iter No18
    Start adress is 0x7fffb2109340
    Alloc time = 3462ns     Free time = 0ns
    Iter No19
    Start adress is 0x7fffb20c9340
    Alloc time = 3277ns     Free time = 0ns
    Iter No20
    Start adress is 0x7fffb2049340
    Alloc time = 3231ns     Free time = 0ns
    Iter No21
    Start adress is 0x7fffb1f49340
    Alloc time = 4879ns     Free time = 0ns
    Iter No22
    Start adress is 0x7fffb1d49340
    Alloc time = 3527ns     Free time = 0ns
    Iter No23
    Segmentation fault (core dumped)
    oshlikhta@kbp1-lhp-a10111:~/pro_camp/task_7/user_mem$ 
