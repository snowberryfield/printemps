---
title: PRINTEMPS
layout: sub
---

# Benchmark Result
## Condition
| Item               | Value                                                                                                                                              |
|:-------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|
| Last Updated       | Aug. 18, 2026                                                                                                                                      |
| PRINTEMPS Version  | 2.9.0                                                                                                                                              |
| Computational Time | 600sec for each instance                                                                                                                           |
| Instance Set       | Pure integer "open" instances of [MIPLIB 2017 Collection Set](https://miplib.zib.de/tag_collection.html) (52 instances)                            |
| Operating System   | Ubuntu 24.04                                                                                                                                       |
| Compiler           | g++13.2.0                                                                                                                                          |
| CPU                | Intel(R) Core(TM) i7-10700 CPU @ 2.90GHz                                                                                                        |
| RAM                | 32GB                                                                                                                                               |
| Solver             | The [standalone solver](https://snowberryfield.github.io/printemps/#standalone-solver) was run with the [option JSON file](benchmark_option.json). |

Back to [Benchmark top](../../../).

## Result
|no.|name|n.vars|n.binvars|n.intvars|n.contvars|n.constrs|found feas.|obj.(viol.)|known best|
|:-:|:--|--:|--:|--:|--:|--:|:-:|--:|--:|
|1|[tokyometro](https://miplib.zib.de/instance_details_tokyometro.html)|4537|2496|2041|0|7719|<font color=green>Yes</font>|9446.6|8263.099999999999
|2|[ex1010-pi](https://miplib.zib.de/instance_details_ex1010-pi.html)|25200|25200|0|0|1468|<font color=green>Yes</font>|244.0|233.0
|3|[d20200](https://miplib.zib.de/instance_details_d20200.html)|4000|4000|0|0|1502|<font color=green>Yes</font>|12458.0|12237.0
|4|[ivu06-big](https://miplib.zib.de/instance_details_ivu06-big.html)|2277736|2277736|0|0|1177|<font color=green>Yes</font>|188.6677|140.74
|5|[comp12-2idx](https://miplib.zib.de/instance_details_comp12-2idx.html)|11863|11820|43|0|16803|<font color=green>Yes</font>|377.0|277.0
|6|[rmine21](https://miplib.zib.de/instance_details_rmine21.html)|162547|162547|0|0|1441651|<font color=gray>N/A</font>|<font color=gray>N/A</font>|-10618.75083837232
|7|[ivu06](https://miplib.zib.de/instance_details_ivu06.html)|787239|787239|0|0|1177|<font color=green>Yes</font>|197.31|142.86
|8|[bley_xs1noM](https://miplib.zib.de/instance_details_bley_xs1noM.html)|3243|2360|883|0|3290|<font color=green>Yes</font>|4351326.0|3873690.77
|9|[bley_xs1](https://miplib.zib.de/instance_details_bley_xs1.html)|3243|2360|883|0|3290|<font color=green>Yes</font>|4385678.0|3855895.859999982
|10|[ivu59](https://miplib.zib.de/instance_details_ivu59.html)|2569996|2569996|0|0|3436|<font color=green>Yes</font>|3054.754|927.8947518206729
|11|[pb-gfrd-pnc](https://miplib.zib.de/instance_details_pb-gfrd-pnc.html)|27888|27888|0|0|874|<font color=gray>No</font>|<font color=red>(5412.0)</font>|8844.0
|12|[pb-grow22](https://miplib.zib.de/instance_details_pb-grow22.html)|25124|25124|0|0|1320|<font color=green>Yes</font>|0.0|-449536.0
|13|[t1722](https://miplib.zib.de/instance_details_t1722.html)|36630|36630|0|0|338|<font color=green>Yes</font>|115776.0|108953.0
|14|[datt256](https://miplib.zib.de/instance_details_datt256.html)|262144|262144|0|0|11077|<font color=gray>No</font>|<font color=red>(126.0)</font>|None
|15|[sorrell7](https://miplib.zib.de/instance_details_sorrell7.html)|2048|2048|0|0|78848|<font color=green>Yes</font>|-187.0|-198.0
|16|[t1717](https://miplib.zib.de/instance_details_t1717.html)|73885|73885|0|0|551|<font color=green>Yes</font>|213794.0|158260.0
|17|[ns1828997](https://miplib.zib.de/instance_details_ns1828997.html)|27275|27275|0|0|81725|<font color=green>Yes</font>|41.0|7.99999999999967
|18|[neos-3594536-henty](https://miplib.zib.de/instance_details_neos-3594536-henty.html)|22924|0|22924|0|21280|<font color=gray>No</font>|<font color=red>(46.0)</font>|401223.0
|19|[neos-4531126-vouga](https://miplib.zib.de/instance_details_neos-4531126-vouga.html)|169996|169996|0|0|7694|<font color=gray>No</font>|<font color=red>(4.0)</font>|13216.63396836053
|20|[neos-3603137-hoteo](https://miplib.zib.de/instance_details_neos-3603137-hoteo.html)|4003|3913|90|0|10510|<font color=gray>No</font>|<font color=red>(20.0)</font>|None
|21|[neos-3355323-arnon](https://miplib.zib.de/instance_details_neos-3355323-arnon.html)|10248|10104|144|0|21216|<font color=gray>No</font>|<font color=red>(36.0)</font>|None
|22|[neos-2974461-ibar](https://miplib.zib.de/instance_details_neos-2974461-ibar.html)|214730|200352|14378|0|214107|<font color=green>Yes</font>|637327600.0|468906174.771
|23|[z26](https://miplib.zib.de/instance_details_z26.html)|17937|17937|0|0|850513|<font color=green>Yes</font>|-1116.0|-1198.0
|24|[rmine25](https://miplib.zib.de/instance_details_rmine25.html)|326599|326599|0|0|2953849|<font color=gray>N/A</font>|<font color=gray>N/A</font>|-15541.66928749976
|25|[bab3](https://miplib.zib.de/instance_details_bab3.html)|393800|393800|0|0|23069|<font color=gray>No</font>|<font color=red>(96.0)</font>|-656214.9542
|26|[graph40-40-1rand](https://miplib.zib.de/instance_details_graph40-40-1rand.html)|102600|102600|0|0|360900|<font color=green>Yes</font>|-7.0|-9.0
|27|[graph40-20-1rand](https://miplib.zib.de/instance_details_graph40-20-1rand.html)|31243|31243|0|0|99067|<font color=green>Yes</font>|-10.0|-15.0
|28|[graph40-80-1rand](https://miplib.zib.de/instance_details_graph40-80-1rand.html)|283648|283648|0|0|1050112|<font color=green>Yes</font>|-2.0|-7.0
|29|[kottenpark09](https://miplib.zib.de/instance_details_kottenpark09.html)|2893026|2892333|693|0|325547|<font color=gray>N/A</font>|<font color=gray>N/A</font>|1715.0
|30|[supportcase30](https://miplib.zib.de/instance_details_supportcase30.html)|1024|1024|0|0|1028|<font color=gray>No</font>|<font color=red>(6.0)</font>|None
|31|[ds-big](https://miplib.zib.de/instance_details_ds-big.html)|174997|174997|0|0|1042|<font color=green>Yes</font>|1972.209|194.58674798
|32|[scpk4](https://miplib.zib.de/instance_details_scpk4.html)|100000|100000|0|0|2000|<font color=green>Yes</font>|325.0|318.0
|33|[scpm1](https://miplib.zib.de/instance_details_scpm1.html)|500000|500000|0|0|5000|<font color=green>Yes</font>|598.0|540.0
|34|[scpn2](https://miplib.zib.de/instance_details_scpn2.html)|1000000|1000000|0|0|5000|<font color=green>Yes</font>|573.0|485.0
|35|[scpj4scip](https://miplib.zib.de/instance_details_scpj4scip.html)|99947|99947|0|0|1000|<font color=green>Yes</font>|131.0|128.0
|36|[scpl4](https://miplib.zib.de/instance_details_scpl4.html)|200000|200000|0|0|2000|<font color=green>Yes</font>|271.0|259.0
|37|[rmine11](https://miplib.zib.de/instance_details_rmine11.html)|12292|12292|0|0|97389|<font color=green>Yes</font>|-2464.507|-2508.404144
|38|[supportcase22](https://miplib.zib.de/instance_details_supportcase22.html)|7129|7129|0|0|260602|<font color=gray>No</font>|<font color=red>(1.0)</font>|110.0
|39|[ramos3](https://miplib.zib.de/instance_details_ramos3.html)|2187|2187|0|0|2187|<font color=green>Yes</font>|214.0|186.0
|40|[rmine13](https://miplib.zib.de/instance_details_rmine13.html)|23980|23980|0|0|197155|<font color=green>Yes</font>|-2390.274|-3495.3706624382535
|41|[neos-4360552-sangro](https://miplib.zib.de/instance_details_neos-4360552-sangro.html)|10272|9696|576|0|46012|<font color=gray>No</font>|<font color=red>(4.0)</font>|-8.0
|42|[neos-3355120-tarago](https://miplib.zib.de/instance_details_neos-3355120-tarago.html)|7800|4700|3100|0|86633|<font color=gray>No</font>|<font color=red>(979597.9)</font>|-11115971.493905
|43|[neos-3682128-sandon](https://miplib.zib.de/instance_details_neos-3682128-sandon.html)|7880|7870|10|0|14920|<font color=green>Yes</font>|36852960.0|34666767.4743958
|44|[f2000](https://miplib.zib.de/instance_details_f2000.html)|4000|4000|0|0|10500|<font color=gray>No</font>|<font color=red>(10.0)</font>|1810.0
|45|[proteindesign121hz512p19](https://miplib.zib.de/instance_details_proteindesign121hz512p19.html)|2589931|2589840|91|0|301|<font color=green>Yes</font>|3412.0|3382.0
|46|[circ10-3](https://miplib.zib.de/instance_details_circ10-3.html)|2700|2700|0|0|42620|<font color=green>Yes</font>|404.0|256.0
|47|[pythago7825](https://miplib.zib.de/instance_details_pythago7825.html)|3745|3745|0|0|14672|<font color=gray>No</font>|<font color=red>(6.0)</font>|None
|48|[cdc7-4-3-2](https://miplib.zib.de/instance_details_cdc7-4-3-2.html)|11811|11811|0|0|14478|<font color=green>Yes</font>|-290.0|-307.0
|49|[rmine15](https://miplib.zib.de/instance_details_rmine15.html)|42438|42438|0|0|358395|<font color=green>Yes</font>|-1341.592|-5018.819990999996
|50|[pizza78i](https://miplib.zib.de/instance_details_pizza78i.html)|385067|143886|241181|0|443776|<font color=gray>N/A</font>|<font color=gray>N/A</font>|564039.0
|51|[pizza27i](https://miplib.zib.de/instance_details_pizza27i.html)|311316|115831|195485|0|359835|<font color=gray>N/A</font>|<font color=gray>N/A</font>|701882.0
|52|[a2864-99blp](https://miplib.zib.de/instance_details_a2864-99blp.html)|200787|200787|0|0|22117|<font color=green>Yes</font>|-257.0|-257.0
