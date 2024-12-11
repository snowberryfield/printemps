---
title: PRINTEMPS
layout: sub
---

# Benchmark Result
## Condition
| Item               | Value                                                                                                                                              |
|:-------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|
| Last Updated       | Dec 5, 2024                                                                                                                                      |
| PRINTEMPS Version  | 2.5.0                                                                                                                                              |
| Computational Time | 600sec for each instance                                                                                                                           |
| Instance Set       | Pure integer "open" instances of [MIPLIB 2017 Collection Set](https://miplib.zib.de/tag_collection.html) (57 instances)                            |
| Operating System   | Ubuntu 24.04                                                                                                                                       |
| Compiler           | g++13.2.0                                                                                                                                          |
| CPU                | Intel(R) Core(TM) i7-10700 CPU @ 2.90GHz                                                                                                        |
| RAM                | 32GB                                                                                                                                               |
| Solver             | The [standalone solver](https://snowberryfield.github.io/printemps/#standalone-solver) was run with the [option JSON file](benchmark_option.json). |

Back to [Benchmark top](../../../).

## Result
|no.|name|n.vars|n.binvars|n.intvars|n.contvars|n.constrs|found feas.|obj.(viol.)|known best|
|:-:|:--|--:|--:|--:|--:|--:|:-:|--:|--:|
|1|[tokyometro](https://miplib.zib.de/instance_details_tokyometro.html)|4537|2496|2041|0|7719|<font color=green>Yes</font>|9464.3|8263.099999999999
|2|[ex1010-pi](https://miplib.zib.de/instance_details_ex1010-pi.html)|25200|25200|0|0|1468|<font color=green>Yes</font>|245.0|233.0
|3|[d20200](https://miplib.zib.de/instance_details_d20200.html)|4000|4000|0|0|1502|<font color=green>Yes</font>|12550.0|12237.0
|4|[ivu06-big](https://miplib.zib.de/instance_details_ivu06-big.html)|2277736|2277736|0|0|1177|<font color=green>Yes</font>|184.2697|140.74
|5|[comp12-2idx](https://miplib.zib.de/instance_details_comp12-2idx.html)|11863|11820|43|0|16803|<font color=green>Yes</font>|352.0|277.0
|6|[rmine21](https://miplib.zib.de/instance_details_rmine21.html)|162547|162547|0|0|1441651|<font color=green>Yes</font>|-708.4226|-10618.75083837232
|7|[ivu06](https://miplib.zib.de/instance_details_ivu06.html)|787239|787239|0|0|1177|<font color=green>Yes</font>|194.7667|142.86
|8|[bley_xs1noM](https://miplib.zib.de/instance_details_bley_xs1noM.html)|3243|2360|883|0|3290|<font color=green>Yes</font>|4177829.0|3873690.77
|9|[bley_xs1](https://miplib.zib.de/instance_details_bley_xs1.html)|3243|2360|883|0|3290|<font color=green>Yes</font>|4354211.0|3873690.77
|10|[ivu59](https://miplib.zib.de/instance_details_ivu59.html)|2569996|2569996|0|0|3436|<font color=green>Yes</font>|3036.893|931.0
|11|[pb-gfrd-pnc](https://miplib.zib.de/instance_details_pb-gfrd-pnc.html)|27888|27888|0|0|874|<font color=gray>No</font>|<font color=red>(8173.0)</font>|8844.0
|12|[pb-grow22](https://miplib.zib.de/instance_details_pb-grow22.html)|25124|25124|0|0|1320|<font color=green>Yes</font>|0.0|-417852.0
|13|[t1722](https://miplib.zib.de/instance_details_t1722.html)|36630|36630|0|0|338|<font color=green>Yes</font>|113645.0|108953.0
|14|[datt256](https://miplib.zib.de/instance_details_datt256.html)|262144|262144|0|0|11077|<font color=gray>No</font>|<font color=red>(133.0)</font>|None
|15|[sorrell7](https://miplib.zib.de/instance_details_sorrell7.html)|2048|2048|0|0|78848|<font color=green>Yes</font>|-187.0|-198.0
|16|[t1717](https://miplib.zib.de/instance_details_t1717.html)|73885|73885|0|0|551|<font color=green>Yes</font>|202217.0|158260.0
|17|[ns1828997](https://miplib.zib.de/instance_details_ns1828997.html)|27275|27275|0|0|81725|<font color=green>Yes</font>|28.0|7.99999999999967
|18|[neos-3594536-henty](https://miplib.zib.de/instance_details_neos-3594536-henty.html)|22924|0|22924|0|21280|<font color=gray>No</font>|<font color=red>(22.0)</font>|401223.0
|19|[neos-4531126-vouga](https://miplib.zib.de/instance_details_neos-4531126-vouga.html)|169996|169996|0|0|7694|<font color=gray>No</font>|<font color=red>(7.0)</font>|525030.8846192999
|20|[neos-3603137-hoteo](https://miplib.zib.de/instance_details_neos-3603137-hoteo.html)|4003|3913|90|0|10510|<font color=gray>No</font>|<font color=red>(21.0)</font>|None
|21|[neos-3355323-arnon](https://miplib.zib.de/instance_details_neos-3355323-arnon.html)|10248|10104|144|0|21216|<font color=gray>No</font>|<font color=red>(32.0)</font>|None
|22|[neos-2974461-ibar](https://miplib.zib.de/instance_details_neos-2974461-ibar.html)|214730|200352|14378|0|214107|<font color=green>Yes</font>|637327600.0|468906174.771
|23|[neos-3426085-ticino](https://miplib.zib.de/instance_details_neos-3426085-ticino.html)|4688|879|3809|0|308|<font color=green>Yes</font>|227.0|225.0
|24|[neos-3068746-nene](https://miplib.zib.de/instance_details_neos-3068746-nene.html)|4890|3815|1075|0|4664|<font color=green>Yes</font>|70525910.0|61910283.68794999
|25|[neos-3426132-dieze](https://miplib.zib.de/instance_details_neos-3426132-dieze.html)|11550|2750|8800|0|570|<font color=green>Yes</font>|416.0|407.0
|26|[z26](https://miplib.zib.de/instance_details_z26.html)|17937|17937|0|0|850513|<font color=green>Yes</font>|-1100.0|-1195.0
|27|[rmine25](https://miplib.zib.de/instance_details_rmine25.html)|326599|326599|0|0|2953849|<font color=green>Yes</font>|-253.2469|-15541.66928749976
|28|[bab3](https://miplib.zib.de/instance_details_bab3.html)|393800|393800|0|0|23069|<font color=gray>No</font>|<font color=red>(87.0)</font>|-656214.9542
|29|[graph40-40-1rand](https://miplib.zib.de/instance_details_graph40-40-1rand.html)|102600|102600|0|0|360900|<font color=green>Yes</font>|-8.0|-9.0
|30|[graph40-20-1rand](https://miplib.zib.de/instance_details_graph40-20-1rand.html)|31243|31243|0|0|99067|<font color=green>Yes</font>|-15.0|-15.0
|31|[graph40-80-1rand](https://miplib.zib.de/instance_details_graph40-80-1rand.html)|283648|283648|0|0|1050112|<font color=green>Yes</font>|-5.0|-7.0
|32|[kottenpark09](https://miplib.zib.de/instance_details_kottenpark09.html)|2893026|2892333|693|0|325547|<font color=gray>No</font>|<font color=red>(5334.0)</font>|1715.0
|33|[kosova1](https://miplib.zib.de/instance_details_kosova1.html)|614253|609591|4662|0|304931|<font color=gray>No</font>|<font color=red>(4947.0)</font>|6.0
|34|[supportcase30](https://miplib.zib.de/instance_details_supportcase30.html)|1024|1024|0|0|1028|<font color=gray>No</font>|<font color=red>(8.0)</font>|None
|35|[ds-big](https://miplib.zib.de/instance_details_ds-big.html)|174997|174997|0|0|1042|<font color=green>Yes</font>|1739.223|195.498997075249
|36|[scpk4](https://miplib.zib.de/instance_details_scpk4.html)|100000|100000|0|0|2000|<font color=green>Yes</font>|323.0|318.0
|37|[scpm1](https://miplib.zib.de/instance_details_scpm1.html)|500000|500000|0|0|5000|<font color=green>Yes</font>|600.0|542.0
|38|[scpn2](https://miplib.zib.de/instance_details_scpn2.html)|1000000|1000000|0|0|5000|<font color=green>Yes</font>|571.0|489.0
|39|[scpj4scip](https://miplib.zib.de/instance_details_scpj4scip.html)|99947|99947|0|0|1000|<font color=green>Yes</font>|129.0|128.0
|40|[scpl4](https://miplib.zib.de/instance_details_scpl4.html)|200000|200000|0|0|2000|<font color=green>Yes</font>|272.0|259.0
|41|[rmine11](https://miplib.zib.de/instance_details_rmine11.html)|12292|12292|0|0|97389|<font color=green>Yes</font>|-2473.367|-2508.404144
|42|[supportcase22](https://miplib.zib.de/instance_details_supportcase22.html)|7129|7129|0|0|260602|<font color=gray>No</font>|<font color=red>(1.0)</font>|110.0
|43|[supportcase2](https://miplib.zib.de/instance_details_supportcase2.html)|2912|2912|0|0|597385|<font color=gray>No</font>|<font color=red>(1.0)</font>|65.0
|44|[ramos3](https://miplib.zib.de/instance_details_ramos3.html)|2187|2187|0|0|2187|<font color=green>Yes</font>|216.0|186.0
|45|[rmine13](https://miplib.zib.de/instance_details_rmine13.html)|23980|23980|0|0|197155|<font color=green>Yes</font>|-2371.735|-3495.3706624382535
|46|[neos-4360552-sangro](https://miplib.zib.de/instance_details_neos-4360552-sangro.html)|10272|9696|576|0|46012|<font color=gray>No</font>|<font color=red>(4.0)</font>|-8.0
|47|[neos-3355120-tarago](https://miplib.zib.de/instance_details_neos-3355120-tarago.html)|7800|4700|3100|0|86633|<font color=gray>No</font>|<font color=red>(953800.4)</font>|-11115965.664625322
|48|[neos-3682128-sandon](https://miplib.zib.de/instance_details_neos-3682128-sandon.html)|7880|7870|10|0|14920|<font color=green>Yes</font>|36844960.0|34666770.0
|49|[f2000](https://miplib.zib.de/instance_details_f2000.html)|4000|4000|0|0|10500|<font color=gray>No</font>|<font color=red>(13.0)</font>|1810.0
|50|[proteindesign121hz512p19](https://miplib.zib.de/instance_details_proteindesign121hz512p19.html)|2589931|2589840|91|0|301|<font color=gray>N/A</font>|<font color=gray>N/A</font>|3382.0
|51|[circ10-3](https://miplib.zib.de/instance_details_circ10-3.html)|2700|2700|0|0|42620|<font color=green>Yes</font>|362.0|258.0
|52|[pythago7825](https://miplib.zib.de/instance_details_pythago7825.html)|3745|3745|0|0|14672|<font color=gray>No</font>|<font color=red>(19.0)</font>|None
|53|[cdc7-4-3-2](https://miplib.zib.de/instance_details_cdc7-4-3-2.html)|11811|11811|0|0|14478|<font color=green>Yes</font>|-285.0|-296.0
|54|[rmine15](https://miplib.zib.de/instance_details_rmine15.html)|42438|42438|0|0|358395|<font color=green>Yes</font>|-1495.608|-5018.819990999996
|55|[pizza78i](https://miplib.zib.de/instance_details_pizza78i.html)|385067|143886|241181|0|443776|<font color=gray>N/A</font>|<font color=gray>N/A</font>|564039.0
|56|[pizza27i](https://miplib.zib.de/instance_details_pizza27i.html)|311316|115831|195485|0|359835|<font color=gray>N/A</font>|<font color=gray>N/A</font>|701882.0
|57|[a2864-99blp](https://miplib.zib.de/instance_details_a2864-99blp.html)|200787|200787|0|0|22117|<font color=green>Yes</font>|-257.0|-257.0
