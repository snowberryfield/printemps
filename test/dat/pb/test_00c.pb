* #variable= 6 #constraint= 3 #product= 9 sizeproduct= 18
min: +1 x1 +2 x2 +4 x3 ;
+1 x1 +2 x2 +4 x3  >= 2 ;
+1 x4 +2 x5 +4 x6  >= 2 ;
+1 x1 x4 +2 x1 x5 +4 x1 x6 +2 x2 x4 +4 x2 x5 +8 x2 x6 +4 x3 x4 +8 x3 x5 +16 x3 x6 = 35;
