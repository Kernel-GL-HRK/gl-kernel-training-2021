--- ./arch/arm/boot/dts/am335x-boneblack.dts    2021-03-20 01:59:12.226831034 +0200
+++ ./am335x-boneblack.dts  2021-03-21 16:28:24.624765846 +0200
@@ -2013,9 +2013,9 @@
                            };
                        };
 
-                       cape_eeprom3@57 {
-                           compatible = "atmel,24c256";
-                           reg = <0x57>;
+                       cape_eeprom3@50 {
+                           compatible = "atmel,24c32";
+                           reg = <0x50>;
                            #address-cells = <0x1>;
                            #size-cells = <0x1>;
 
@@ -2023,6 +2023,11 @@
                                reg = <0x0 0x100>;
                            };
                        };
+
+                       rtc0@68 {
+                           compatible = "maxim,ds3231";
+                           reg = <0x68>;
+                       };
                    };
                };
 
