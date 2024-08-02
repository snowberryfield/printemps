---
title: PRINTEMPS
layout: sub
---

# Benchmark Result
## Condition
| Item               | Value                                                                                                                                              |
|:-------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|
| Last Updated       | May 5, 2024                                                                                                                                      |
| PRINTEMPS Version  | 2.3.0                                                                                                                                              |
| Computational Time | 600sec for each instance                                                                                                                           |
| Instance Set       | Pure integer "open" instances of [MIPLIB 2017 Collection Set](https://miplib.zib.de/tag_collection.html) (58 instances)                            |
| Operating System   | Ubuntu 22.04                                                                                                                                       |
| Compiler           | g++11.2.0                                                                                                                                          |
| CPU                | Intel(R) Xeon(R) CPU E3-1270 v5 @ 3.60GHz                                                                                                          |
| RAM                | 16GB                                                                                                                                               |
| Solver             | The [standalone solver](https://snowberryfield.github.io/printemps/#standalone-solver) was run with the [option JSON file](benchmark_option.json). |

Back to [Benchmark top](../../../).

## Result
|no.|name|n.vars|n.binvars|n.intvars|n.contvars|n.constrs|found feas.|obj.(viol.)|known best|
|:-:|:--|--:|--:|--:|--:|--:|:-:|--:|--:|
|1|[tokyometro](https://miplib.zib.de/instance_details_tokyometro.html)|4537|2496|2041|0|7719|<font color=green>Yes</font>|10423.5|8263.099999999999
|2|[ex1010-pi](https://miplib.zib.de/instance_details_ex1010-pi.html)|25200|25200|0|0|1468|<font color=green>Yes</font>|245.0|233.0
|3|[d20200](https://miplib.zib.de/instance_details_d20200.html)|4000|4000|0|0|1502|<font color=green>Yes</font>|12581.0|12239.0
|4|[ivu06-big](https://miplib.zib.de/instance_details_ivu06-big.html)|2277736|2277736|0|0|1177|<font color=green>Yes</font>|191.3877|140.74
|5|[comp12-2idx](https://miplib.zib.de/instance_details_comp12-2idx.html)|11863|11820|43|0|16803|<font color=green>Yes</font>|381.0|291.0
|6|[rmine21](https://miplib.zib.de/instance_details_rmine21.html)|162547|162547|0|0|1441651|<font color=green>Yes</font>|-438.3822|-10618.75083837232
|7|[ivu06](https://miplib.zib.de/instance_details_ivu06.html)|787239|787239|0|0|1177|<font color=green>Yes</font>|195.0247|142.86
|8|[bley_xs1noM](https://miplib.zib.de/instance_details_bley_xs1noM.html)|3243|2360|883|0|3290|<font color=green>Yes</font>|4221238.0|3873690.77
|9|[bley_xs1](https://miplib.zib.de/instance_details_bley_xs1.html)|3243|2360|883|0|3290|<font color=green>Yes</font>|4400340.0|3922112.840000001
|10|[ivu59](https://miplib.zib.de/instance_details_ivu59.html)|2569996|2569996|0|0|3436|<font color=green>Yes</font>|3066.042|931.0
|11|[usafa](https://miplib.zib.de/instance_details_usafa.html)|228648|216357|12291|0|1377561|<font color=green>Yes</font>|26804.88|160.1671357657
|12|[pb-gfrd-pnc](https://miplib.zib.de/instance_details_pb-gfrd-pnc.html)|27888|27888|0|0|874|<font color=gray>No</font>|<font color=red>(8039.0)</font>|8844.0
|13|[pb-grow22](https://miplib.zib.de/instance_details_pb-grow22.html)|25124|25124|0|0|1320|<font color=green>Yes</font>|0.0|-417442.0
|14|[t1722](https://miplib.zib.de/instance_details_t1722.html)|36630|36630|0|0|338|<font color=green>Yes</font>|117580.0|108953.0
|15|[datt256](https://miplib.zib.de/instance_details_datt256.html)|262144|262144|0|0|11077|<font color=gray>No</font>|<font color=red>(140.0)</font>|None
|16|[sorrell7](https://miplib.zib.de/instance_details_sorrell7.html)|2048|2048|0|0|78848|<font color=green>Yes</font>|-189.0|-198.0
|17|[t1717](https://miplib.zib.de/instance_details_t1717.html)|73885|73885|0|0|551|<font color=green>Yes</font>|212154.0|158260.0
|18|[ns1828997](https://miplib.zib.de/instance_details_ns1828997.html)|27275|27275|0|0|81725|<font color=green>Yes</font>|29.0|7.99999999999967
|19|[neos-3594536-henty](https://miplib.zib.de/instance_details_neos-3594536-henty.html)|22924|0|22924|0|21280|<font color=gray>No</font>|<font color=red>(60.0)</font>|401224.0
|20|[neos-4531126-vouga](https://miplib.zib.de/instance_details_neos-4531126-vouga.html)|169996|169996|0|0|7694|<font color=gray>No</font>|<font color=red>(6.0)</font>|525030.8846192999
|21|[neos-3603137-hoteo](https://miplib.zib.de/instance_details_neos-3603137-hoteo.html)|4003|3913|90|0|10510|<font color=gray>No</font>|<font color=red>(21.0)</font>|None
|22|[neos-3355323-arnon](https://miplib.zib.de/instance_details_neos-3355323-arnon.html)|10248|10104|144|0|21216|<font color=gray>No</font>|<font color=red>(31.0)</font>|None
|23|[neos-2974461-ibar](https://miplib.zib.de/instance_details_neos-2974461-ibar.html)|214730|200352|14378|0|214107|<font color=green>Yes</font>|637327600.0|468906174.771
|24|[neos-3426085-ticino](https://miplib.zib.de/instance_details_neos-3426085-ticino.html)|4688|879|3809|0|308|<font color=green>Yes</font>|228.0|225.0
|25|[neos-3068746-nene](https://miplib.zib.de/instance_details_neos-3068746-nene.html)|4890|3815|1075|0|4664|<font color=green>Yes</font>|69800640.0|61910283.68794999
|26|[neos-3426132-dieze](https://miplib.zib.de/instance_details_neos-3426132-dieze.html)|11550|2750|8800|0|570|<font color=green>Yes</font>|413.0|407.0
|27|[neos-3322547-alsek](https://miplib.zib.de/instance_details_neos-3322547-alsek.html)|1001000|1001000|0|0|2000|<font color=green>Yes</font>|463.0|400.0
|28|[z26](https://miplib.zib.de/instance_details_z26.html)|17937|17937|0|0|850513|<font color=green>Yes</font>|-1108.0|-1195.0
|29|[rmine25](https://miplib.zib.de/instance_details_rmine25.html)|326599|326599|0|0|2953849|<font color=green>Yes</font>|-160.9361|-15541.66928749976
|30|[bab3](https://miplib.zib.de/instance_details_bab3.html)|393800|393800|0|0|23069|<font color=gray>No</font>|<font color=red>(94.0)</font>|-656214.9542
|31|[graph40-40-1rand](https://miplib.zib.de/instance_details_graph40-40-1rand.html)|102600|102600|0|0|360900|<font color=green>Yes</font>|-7.0|-9.0
|32|[graph40-20-1rand](https://miplib.zib.de/instance_details_graph40-20-1rand.html)|31243|31243|0|0|99067|<font color=green>Yes</font>|-14.0|-15.0
|33|[graph40-80-1rand](https://miplib.zib.de/instance_details_graph40-80-1rand.html)|283648|283648|0|0|1050112|<font color=green>Yes</font>|-4.0|-7.0
|34|[kottenpark09](https://miplib.zib.de/instance_details_kottenpark09.html)|2893026|2892333|693|0|325547|<font color=gray>No</font>|<font color=red>(5334.0)</font>|1715.0
|35|[kosova1](https://miplib.zib.de/instance_details_kosova1.html)|614253|609591|4662|0|304931|<font color=gray>No</font>|<font color=red>(5034.0)</font>|6.0
|36|[supportcase30](https://miplib.zib.de/instance_details_supportcase30.html)|1024|1024|0|0|1028|<font color=gray>No</font>|<font color=red>(16.0)</font>|None
|37|[ds-big](https://miplib.zib.de/instance_details_ds-big.html)|174997|174997|0|0|1042|<font color=green>Yes</font>|1876.989|195.498997075249
|38|[scpk4](https://miplib.zib.de/instance_details_scpk4.html)|100000|100000|0|0|2000|<font color=green>Yes</font>|327.0|319.0
|39|[scpm1](https://miplib.zib.de/instance_details_scpm1.html)|500000|500000|0|0|5000|<font color=green>Yes</font>|602.0|542.0
|40|[scpn2](https://miplib.zib.de/instance_details_scpn2.html)|1000000|1000000|0|0|5000|<font color=green>Yes</font>|590.0|489.0
|41|[scpj4scip](https://miplib.zib.de/instance_details_scpj4scip.html)|99947|99947|0|0|1000|<font color=green>Yes</font>|133.0|128.0
|42|[scpl4](https://miplib.zib.de/instance_details_scpl4.html)|200000|200000|0|0|2000|<font color=green>Yes</font>|273.0|259.0
|43|[rmine11](https://miplib.zib.de/instance_details_rmine11.html)|12292|12292|0|0|97389|<font color=green>Yes</font>|-2469.086|-2508.404144
|44|[supportcase2](https://miplib.zib.de/instance_details_supportcase2.html)|2912|2912|0|0|597385|<font color=gray>No</font>|<font color=red>(1.0)</font>|65.0
|45|[ramos3](https://miplib.zib.de/instance_details_ramos3.html)|2187|2187|0|0|2187|<font color=green>Yes</font>|219.0|186.0
|46|[rmine13](https://miplib.zib.de/instance_details_rmine13.html)|23980|23980|0|0|197155|<font color=green>Yes</font>|-1973.094|-3495.3706624382535
|47|[neos-4360552-sangro](https://miplib.zib.de/instance_details_neos-4360552-sangro.html)|10272|9696|576|0|46012|<font color=gray>No</font>|<font color=red>(4.0)</font>|-8.0
|48|[neos-3355120-tarago](https://miplib.zib.de/instance_details_neos-3355120-tarago.html)|7800|4700|3100|0|86633|<font color=gray>No</font>|<font color=red>(979598.9)</font>|-11115965.664625322
|49|[neos-3682128-sandon](https://miplib.zib.de/instance_details_neos-3682128-sandon.html)|7880|7870|10|0|14920|<font color=green>Yes</font>|35100870.0|34666770.0
|50|[f2000](https://miplib.zib.de/instance_details_f2000.html)|4000|4000|0|0|10500|<font color=gray>No</font>|<font color=red>(18.0)</font>|1810.0
|51|[proteindesign121hz512p19](https://miplib.zib.de/instance_details_proteindesign121hz512p19.html)|2589931|2589840|91|0|301|<font color=gray>N/A</font>|<font color=gray>N/A</font>|3382.0
|52|[circ10-3](https://miplib.zib.de/instance_details_circ10-3.html)|2700|2700|0|0|42620|<font color=green>Yes</font>|358.0|280.0
|53|[pythago7825](https://miplib.zib.de/instance_details_pythago7825.html)|3745|3745|0|0|14672|<font color=gray>No</font>|<font color=red>(14.0)</font>|None
|54|[cdc7-4-3-2](https://miplib.zib.de/instance_details_cdc7-4-3-2.html)|11811|11811|0|0|14478|<font color=green>Yes</font>|-289.0|-296.0
|55|[rmine15](https://miplib.zib.de/instance_details_rmine15.html)|42438|42438|0|0|358395|<font color=green>Yes</font>|-1313.953|-5018.819990999996
|56|[pizza78i](https://miplib.zib.de/instance_details_pizza78i.html)|385067|143886|241181|0|443776|<font color=gray>N/A</font>|<font color=gray>N/A</font>|564039.0
|57|[pizza27i](https://miplib.zib.de/instance_details_pizza27i.html)|311316|115831|195485|0|359835|<font color=gray>N/A</font>|<font color=gray>N/A</font>|701882.0
|58|[a2864-99blp](https://miplib.zib.de/instance_details_a2864-99blp.html)|200787|200787|0|0|22117|<font color=green>Yes</font>|-257.0|-257.0