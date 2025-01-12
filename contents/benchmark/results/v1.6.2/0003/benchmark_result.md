---
title: PRINTEMPS
layout: sub
---

# Benchmark Result

## Condition
| Item               | Value                                                                                                                                              |
|:-------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|
| Last Updated       | May 25, 2021                                                                                                                                       |
| PRINTEMPS Version  | 1.6.2                                                                                                                                              |
| Computational Time | 3600sec for each instance                                                                                                                          |
| Instance Set       | Pure integer instances of [MIPLIB 2017 Benchmark Set](https://miplib.zib.de/tag_benchmark.html) (89 instances)                                     |
| Operating System   | Ubuntu 18.04                                                                                                                                       |
| Compiler           | g++ 7.5.0                                                                                                                                          |
| CPU                | Intel(R) Xeon(R) CPU E3-1270 v5 @ 3.60GHz                                                                                                          |
| RAM                | 16GB                                                                                                                                               |
| Solver             | The [standalone solver](https://snowberryfield.github.io/printemps/#standalone-solver) was run with the [option JSON file](benchmark_option.json). |

Back to [Benchmark top](../../../).

## Result

| name                                                                                               |  n.vars | n.binvars | n.intvars | n.contvars | n.constrs |         found feas.          |                     obj.(viol.) |       known best |
|:---------------------------------------------------------------------------------------------------|--------:|----------:|----------:|-----------:|----------:|:----------------------------:|--------------------------------:|-----------------:|
| [reblock115](https://miplib.zib.de/instance_details_reblock115.html)                               |    1150 |      1150 |         0 |          0 |      4735 | <font color=green>Yes</font> |                   -35676204.663 |   -36800603.2332 |
| [academictimetablesmall](https://miplib.zib.de/instance_details_academictimetablesmall.html)       |   28926 |     28926 |         0 |          0 |     23294 | <font color=green>Yes</font> |                           654.0 |              0.0 |
| [bab6](https://miplib.zib.de/instance_details_bab6.html)                                           |  114240 |    114240 |         0 |          0 |     29904 |  <font color=gray>No</font>  |   <font color=red>(48.0)</font> |     -284248.2307 |
| [gen-ip002](https://miplib.zib.de/instance_details_gen-ip002.html)                                 |      41 |         0 |        41 |          0 |        24 | <font color=green>Yes</font> |                   -4783.7333916 |     -4783.733392 |
| [gen-ip054](https://miplib.zib.de/instance_details_gen-ip054.html)                                 |      30 |         0 |        30 |          0 |        27 | <font color=green>Yes</font> |                    6858.5132817 |    6840.96564179 |
| [comp21-2idx](https://miplib.zib.de/instance_details_comp21-2idx.html)                             |   10863 |     10792 |        71 |          0 |     14038 | <font color=green>Yes</font> |                           131.0 |             74.0 |
| [comp07-2idx](https://miplib.zib.de/instance_details_comp07-2idx.html)                             |   17264 |     17155 |       109 |          0 |     21235 | <font color=green>Yes</font> |                             6.0 |              6.0 |
| [neos859080](https://miplib.zib.de/instance_details_neos859080.html)                               |     160 |        80 |        80 |          0 |       164 |  <font color=gray>No</font>  |    <font color=red>(1.0)</font> |             None |
| [neos-787933](https://miplib.zib.de/instance_details_neos-787933.html)                             |  236376 |    236376 |         0 |          0 |      1897 | <font color=green>Yes</font> |                            33.0 |             30.0 |
| [air05](https://miplib.zib.de/instance_details_air05.html)                                         |    7195 |      7195 |         0 |          0 |       426 | <font color=green>Yes</font> |                         26448.0 |          26374.0 |
| [mzzv42z](https://miplib.zib.de/instance_details_mzzv42z.html)                                     |   11717 |     11482 |       235 |          0 |     10460 | <font color=green>Yes</font> |                        -19730.0 |         -20540.0 |
| [nu25-pr12](https://miplib.zib.de/instance_details_nu25-pr12.html)                                 |    5868 |      5832 |        36 |          0 |      2313 | <font color=green>Yes</font> |                         54535.0 |          53905.0 |
| [irp](https://miplib.zib.de/instance_details_irp.html)                                             |   20315 |     20315 |         0 |          0 |        39 | <font color=green>Yes</font> |                    12160.283813 |  12159.492835397 |
| [qap10](https://miplib.zib.de/instance_details_qap10.html)                                         |    4150 |      4150 |         0 |          0 |      1820 | <font color=green>Yes</font> |                           340.0 |            340.0 |
| [neos8](https://miplib.zib.de/instance_details_neos8.html)                                         |   23228 |     23224 |         4 |          0 |     46324 | <font color=green>Yes</font> |                         -3606.0 |          -3719.0 |
| [sp98ar](https://miplib.zib.de/instance_details_sp98ar.html)                                       |   15085 |     15085 |         0 |          0 |      1435 | <font color=green>Yes</font> |                    544370364.48 |      529740623.2 |
| [neos-960392](https://miplib.zib.de/instance_details_neos-960392.html)                             |   59376 |     59376 |         0 |          0 |      4744 | <font color=green>Yes</font> |                          -238.0 |           -238.0 |
| [enlight_hard](https://miplib.zib.de/instance_details_enlight_hard.html)                           |     200 |       100 |       100 |          0 |       100 |  <font color=gray>No</font>  |    <font color=red>(1.0)</font> |             37.0 |
| [neos-1582420](https://miplib.zib.de/instance_details_neos-1582420.html)                           |   10100 |     10000 |       100 |          0 |     10180 | <font color=green>Yes</font> |                            98.0 |             91.0 |
| [fast0507](https://miplib.zib.de/instance_details_fast0507.html)                                   |   63009 |     63009 |         0 |          0 |       507 | <font color=green>Yes</font> |                           177.0 |            174.0 |
| [neos-950242](https://miplib.zib.de/instance_details_neos-950242.html)                             |    5760 |      5520 |       240 |          0 |     34224 | <font color=green>Yes</font> |                             4.0 |              4.0 |
| [neos-662469](https://miplib.zib.de/instance_details_neos-662469.html)                             |   18235 |     17907 |       328 |          0 |      1085 | <font color=green>Yes</font> |                        285501.5 |         184380.0 |
| [neos-957323](https://miplib.zib.de/instance_details_neos-957323.html)                             |   57756 |     57756 |         0 |          0 |      3757 | <font color=green>Yes</font> |                   -237.74894815 |     -237.7566815 |
| [nw04](https://miplib.zib.de/instance_details_nw04.html)                                           |   87482 |     87482 |         0 |          0 |        36 | <font color=green>Yes</font> |                         16862.0 |          16862.0 |
| [neos-1354092](https://miplib.zib.de/instance_details_neos-1354092.html)                           |   13702 |     13282 |       420 |          0 |      3135 | <font color=green>Yes</font> |                            50.0 |             46.0 |
| [mzzv11](https://miplib.zib.de/instance_details_mzzv11.html)                                       |   10240 |      9989 |       251 |          0 |      9499 | <font color=green>Yes</font> |                        -18824.0 |         -21718.0 |
| [netdiversion](https://miplib.zib.de/instance_details_netdiversion.html)                           |  129180 |    129180 |         0 |          0 |    119589 | <font color=green>Yes</font> |                          1280.0 |            242.0 |
| [sorrell3](https://miplib.zib.de/instance_details_sorrell3.html)                                   |    1024 |      1024 |         0 |          0 |    169162 | <font color=green>Yes</font> |                           -16.0 |            -16.0 |
| [bnatt400](https://miplib.zib.de/instance_details_bnatt400.html)                                   |    3600 |      3600 |         0 |          0 |      5614 |  <font color=gray>No</font>  |  <font color=red>(1.625)</font> |              1.0 |
| [tbfp-network](https://miplib.zib.de/instance_details_tbfp-network.html)                           |   72747 |     72747 |         0 |          0 |      2436 | <font color=green>Yes</font> |                    37.166666667 |      24.16319444 |
| [glass-sc](https://miplib.zib.de/instance_details_glass-sc.html)                                   |     214 |       214 |         0 |          0 |      6119 | <font color=green>Yes</font> |                            23.0 |             23.0 |
| [ex9](https://miplib.zib.de/instance_details_ex9.html)                                             |   10404 |         0 |     10404 |          0 |     40962 |  <font color=gray>No</font>  |   <font color=red>(40.0)</font> |             81.0 |
| [neos-3024952-loue](https://miplib.zib.de/instance_details_neos-3024952-loue.html)                 |    3255 |         0 |      3255 |          0 |      3705 | <font color=green>Yes</font> |                        102121.0 |          26756.0 |
| [neos-4738912-atrato](https://miplib.zib.de/instance_details_neos-4738912-atrato.html)             |    6216 |      1120 |      5096 |          0 |      1947 | <font color=green>Yes</font> |                    340344402.96 |    283627956.595 |
| [neos-3083819-nubu](https://miplib.zib.de/instance_details_neos-3083819-nubu.html)                 |    8644 |         0 |      8644 |          0 |      4725 | <font color=green>Yes</font> |                       6687822.0 |        6307996.0 |
| [neos-2657525-crna](https://miplib.zib.de/instance_details_neos-2657525-crna.html)                 |     524 |       146 |       378 |          0 |       342 | <font color=green>Yes</font> |                      13.9469932 |         1.810748 |
| [neos-3381206-awhea](https://miplib.zib.de/instance_details_neos-3381206-awhea.html)               |    2375 |       475 |      1900 |          0 |       479 | <font color=green>Yes</font> |                           453.0 |            453.0 |
| [neos-5052403-cygnet](https://miplib.zib.de/instance_details_neos-5052403-cygnet.html)             |   32868 |     32868 |         0 |          0 |     38268 | <font color=green>Yes</font> |                           194.0 |            182.0 |
| [neos-3004026-krka](https://miplib.zib.de/instance_details_neos-3004026-krka.html)                 |   17030 |     16900 |       130 |          0 |     12545 | <font color=green>Yes</font> |                             0.0 |              0.0 |
| [nursesched-sprint02](https://miplib.zib.de/instance_details_nursesched-sprint02.html)             |   10250 |     10230 |        20 |          0 |      3522 | <font color=green>Yes</font> |                            65.0 |             58.0 |
| [nursesched-medium-hint03](https://miplib.zib.de/instance_details_nursesched-medium-hint03.html)   |   34248 |     34170 |        78 |          0 |     14062 | <font color=green>Yes</font> |                           866.0 |            115.0 |
| [sp97ar](https://miplib.zib.de/instance_details_sp97ar.html)                                       |   14101 |     14101 |         0 |          0 |      1761 | <font color=green>Yes</font> |                    679844710.08 |    660705645.759 |
| [graph20-20-1rand](https://miplib.zib.de/instance_details_graph20-20-1rand.html)                   |    2183 |      2183 |         0 |          0 |      5587 | <font color=green>Yes</font> |                            -9.0 |             -9.0 |
| [wachplan](https://miplib.zib.de/instance_details_wachplan.html)                                   |    3361 |      3360 |         1 |          0 |      1553 | <font color=green>Yes</font> |                            -8.0 |             -8.0 |
| [decomp2](https://miplib.zib.de/instance_details_decomp2.html)                                     |   14387 |     14387 |         0 |          0 |     10765 | <font color=green>Yes</font> |                          -160.0 |           -160.0 |
| [highschool1-aigio](https://miplib.zib.de/instance_details_highschool1-aigio.html)                 |  320404 |    319686 |       718 |          0 |     92568 |  <font color=gray>No</font>  | <font color=red>(7270.0)</font> |              0.0 |
| [brazil3](https://miplib.zib.de/instance_details_brazil3.html)                                     |   23968 |     23874 |        94 |          0 |     14646 |  <font color=gray>No</font>  |   <font color=red>(21.0)</font> |             24.0 |
| [eilA101-2](https://miplib.zib.de/instance_details_eilA101-2.html)                                 |   65832 |     65832 |         0 |          0 |       100 | <font color=green>Yes</font> |                      880.920108 |       880.920108 |
| [30n20b8](https://miplib.zib.de/instance_details_30n20b8.html)                                     |   18380 |     18318 |        62 |          0 |       576 | <font color=green>Yes</font> |                           302.0 |            302.0 |
| [supportcase18](https://miplib.zib.de/instance_details_supportcase18.html)                         |   13410 |     13410 |         0 |          0 |       240 | <font color=green>Yes</font> |                            49.0 |             48.0 |
| [supportcase6](https://miplib.zib.de/instance_details_supportcase6.html)                           |  130052 |    130051 |         1 |          0 |       771 | <font color=green>Yes</font> |                    62271.865291 |      51906.47737 |
| [supportcase22](https://miplib.zib.de/instance_details_supportcase22.html)                         |    7129 |      7129 |         0 |          0 |    260602 |  <font color=gray>No</font>  |    <font color=red>(1.0)</font> |             None |
| [supportcase10](https://miplib.zib.de/instance_details_supportcase10.html)                         |   14770 |     14770 |         0 |          0 |    165684 |  <font color=gray>No</font>  |  <font color=red>(463.0)</font> |              7.0 |
| [supportcase19](https://miplib.zib.de/instance_details_supportcase19.html)                         | 1429098 |   1311292 |    117806 |          0 |     10713 |  <font color=gray>No</font>  |    <font color=red>(4.0)</font> |       12677206.0 |
| [n3div36](https://miplib.zib.de/instance_details_n3div36.html)                                     |   22120 |     22120 |         0 |          0 |      4484 | <font color=green>Yes</font> |                        131400.0 |         130800.0 |
| [cvs16r128-89](https://miplib.zib.de/instance_details_cvs16r128-89.html)                           |    3472 |      3472 |         0 |          0 |      4633 | <font color=green>Yes</font> |                           -94.0 |            -97.0 |
| [co-100](https://miplib.zib.de/instance_details_co-100.html)                                       |   48417 |     48417 |         0 |          0 |      2187 | <font color=green>Yes</font> |                      2724301.11 |       2639942.06 |
| [lectsched-5-obj](https://miplib.zib.de/instance_details_lectsched-5-obj.html)                     |   21805 |     21389 |       416 |          0 |     38884 | <font color=green>Yes</font> |                            34.0 |             24.0 |
| [neos-3555904-turama](https://miplib.zib.de/instance_details_neos-3555904-turama.html)             |   37461 |     37461 |         0 |          0 |    146493 | <font color=green>Yes</font> |                           -34.7 |            -34.7 |
| [neos-3988577-wolgan](https://miplib.zib.de/instance_details_neos-3988577-wolgan.html)             |   25870 |     25870 |         0 |          0 |     44662 |  <font color=gray>No</font>  |    <font color=red>(1.0)</font> |             None |
| [neos-2075418-temuka](https://miplib.zib.de/instance_details_neos-2075418-temuka.html)             |  122304 |    122304 |         0 |          0 |    349602 |  <font color=gray>No</font>  |  <font color=red>(379.0)</font> |             None |
| [splice1k1](https://miplib.zib.de/instance_details_splice1k1.html)                                 |    3253 |      3252 |         1 |          0 |      6505 | <font color=green>Yes</font> |                          -394.0 |           -394.0 |
| [hypothyroid-k1](https://miplib.zib.de/instance_details_hypothyroid-k1.html)                       |    2602 |      2601 |         1 |          0 |      5195 | <font color=green>Yes</font> |                         -2851.0 |          -2851.0 |
| [cryptanalysiskb128n5obj16](https://miplib.zib.de/instance_details_cryptanalysiskb128n5obj16.html) |   48950 |     47830 |      1120 |          0 |     98021 |  <font color=gray>No</font>  |   <font color=red>(56.0)</font> |              0.0 |
| [proteindesign121hz512p9](https://miplib.zib.de/instance_details_proteindesign121hz512p9.html)     |  159145 |    159054 |        91 |          0 |       301 | <font color=green>Yes</font> |                          1493.0 |           1473.0 |
| [k1mushroom](https://miplib.zib.de/instance_details_k1mushroom.html)                               |    8211 |      8210 |         1 |          0 |     16419 | <font color=green>Yes</font> |                         -3288.0 |          -3288.0 |
| [cryptanalysiskb128n5obj14](https://miplib.zib.de/instance_details_cryptanalysiskb128n5obj14.html) |   48950 |     47830 |      1120 |          0 |     98021 |  <font color=gray>No</font>  |   <font color=red>(70.0)</font> |             None |
| [proteindesign122trx11p8](https://miplib.zib.de/instance_details_proteindesign122trx11p8.html)     |  127326 |    127248 |        78 |          0 |       254 | <font color=green>Yes</font> |                          1757.0 |           1747.0 |
| [peg-solitaire-a3](https://miplib.zib.de/instance_details_peg-solitaire-a3.html)                   |    4552 |      4552 |         0 |          0 |      4587 |  <font color=gray>No</font>  |    <font color=red>(3.0)</font> |              1.0 |
| [cod105](https://miplib.zib.de/instance_details_cod105.html)                                       |    1024 |      1024 |         0 |          0 |      1024 | <font color=green>Yes</font> |                           -12.0 |            -12.0 |
| [supportcase33](https://miplib.zib.de/instance_details_supportcase33.html)                         |   20203 |     20102 |       101 |          0 |     20489 | <font color=green>Yes</font> |                          -160.0 |           -345.0 |
| [rail02](https://miplib.zib.de/instance_details_rail02.html)                                       |  270869 |    270869 |         0 |          0 |     95791 |  <font color=gray>No</font>  |   <font color=red>(18.0)</font> |     -200.4499077 |
| [ns1952667](https://miplib.zib.de/instance_details_ns1952667.html)                                 |   13264 |         0 |     13264 |          0 |        41 |  <font color=gray>No</font>  |  <font color=red>(110.0)</font> |              0.0 |
| [seymour](https://miplib.zib.de/instance_details_seymour.html)                                     |    1372 |      1372 |         0 |          0 |      4944 | <font color=green>Yes</font> |                           424.0 |            423.0 |
| [ex10](https://miplib.zib.de/instance_details_ex10.html)                                           |   17680 |         0 |     17680 |          0 |     69608 |  <font color=gray>No</font>  |   <font color=red>(44.0)</font> |            100.0 |
| [rococoC10-001000](https://miplib.zib.de/instance_details_rococoC10-001000.html)                   |    3117 |      2993 |       124 |          0 |      1293 | <font color=green>Yes</font> |                         12090.0 |          11460.0 |
| [n2seq36q](https://miplib.zib.de/instance_details_n2seq36q.html)                                   |   22480 |     22480 |         0 |          0 |      2565 | <font color=green>Yes</font> |                         56400.0 |          52200.0 |
| [bnatt500](https://miplib.zib.de/instance_details_bnatt500.html)                                   |    4500 |      4500 |         0 |          0 |      7029 |  <font color=gray>No</font>  |  <font color=red>(3.859)</font> |             None |
| [s100](https://miplib.zib.de/instance_details_s100.html)                                           |  364417 |    364417 |         0 |          0 |     14733 | <font color=green>Yes</font> |                  -0.14797016291 | -0.1697235270583 |
| [opm2-z10-s4](https://miplib.zib.de/instance_details_opm2-z10-s4.html)                             |    6250 |      6250 |         0 |          0 |    160633 | <font color=green>Yes</font> |                        -31426.0 |         -33269.0 |
| [square47](https://miplib.zib.de/instance_details_square47.html)                                   |   95030 |     94987 |        43 |          0 |     61591 | <font color=green>Yes</font> |                            30.0 |   15.99999999979 |
| [square41](https://miplib.zib.de/instance_details_square41.html)                                   |   62234 |     62197 |        37 |          0 |     40160 | <font color=green>Yes</font> |                            21.0 |             15.0 |
| [rail01](https://miplib.zib.de/instance_details_rail01.html)                                       |  117527 |    117527 |         0 |          0 |     46843 |  <font color=gray>No</font>  |   <font color=red>(31.0)</font> |      -70.5699643 |
| [rococoB10-011000](https://miplib.zib.de/instance_details_rococoB10-011000.html)                   |    4456 |      4320 |       136 |          0 |      1667 | <font color=green>Yes</font> |                         21531.0 |          19449.0 |
| [neos-631710](https://miplib.zib.de/instance_details_neos-631710.html)                             |  167056 |    167056 |         0 |          0 |    169576 | <font color=green>Yes</font> |                           207.0 |            203.0 |
| [bab2](https://miplib.zib.de/instance_details_bab2.html)                                           |  147912 |    147912 |         0 |          0 |     17245 |  <font color=gray>No</font>  |   <font color=red>(63.0)</font> |     -357544.3115 |
| [chromaticindex512-7](https://miplib.zib.de/instance_details_chromaticindex512-7.html)             |   36864 |     36864 |         0 |          0 |     33791 | <font color=green>Yes</font> |                             4.0 |              4.0 |
| [chromaticindex1024-7](https://miplib.zib.de/instance_details_chromaticindex1024-7.html)           |   73728 |     73728 |         0 |          0 |     67583 | <font color=green>Yes</font> |                             4.0 |              4.0 |
| [eil33-2](https://miplib.zib.de/instance_details_eil33-2.html)                                     |    4516 |      4516 |         0 |          0 |        32 | <font color=green>Yes</font> |                      934.007916 | 934.007915999999 |