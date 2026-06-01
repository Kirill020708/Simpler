#pragma once


#define qsFPmargin 133 // 0 300
#define qsFPseeMargin 22 // -100 100

#define qsBetaFail 619 // 0 1024

#define corrplexityMargin 66 // 0 300

#define rfpBaseD2 37 // 0 70
#define rfpImprovingD2 14 // 0 40
#define rfpCorrplexityD2 16 // 0 40
#define rfpWorseningD2 1 // 0 40
#define rfpCutnodeD2 0 // 0 40
#define rfpBaseD1 3 // 0 70
#define rfpImprovingD1 1 // 0 40
#define rfpCorrplexityD1 2 // 0 40
#define rfpWorseningD1 4 // 0 40
#define rfpCutnodeD1 1 // 0 40
#define rfpBaseD0 5 // 0 70
#define rfpImprovingD0 1 // 0 40
#define rfpCorrplexityD0 2 // 0 40
#define rfpWorseningD0 1 // 0 40
#define rfpCutnodeD0 2 // 0 40
#define rfpFail 540 // 0 1024

#define nmpBaseMargin 12 // 0 80
#define nmpDepthMargin 947 // 0 10240
#define nmpDepth2Margin 884 // 0 10240
#define nmpRbase 5552 // 0 10240 341
#define nmpRdepth 204 // 0 1024
#define nmpRimproving 31 // 0 3072
#define nmpRcutnode -378 // -4096 4096
#define nmpRmargin 230 // 50 500
#define nmpRmarginClamp 6362 // 512 10240

#define razorBaseD2 149 // 100 400
#define razorImprovingD2 28 // 0 70
#define razorBaseD1 8 // 0 300
#define razorImprovingD1 2 // 0 70
#define razorBaseD0 205 // 0 400
#define razorImprovingD0 3 // 0 70
#define razorFPmargin 39 // 0 150
#define razorRFPmargin 176 // 0 500

#define probcutBase 188 // 0 500
#define probcutImproving 60 // 0 100
#define probcutFail 952 // 0 1024

#define singextMarginDepth 874 // 0 1024
#define dextMarginBase 15326 // 0 15360
#define dextMarginHistory 9 // 0 10
#define trextMarginBase 76 // 0 80

#define lmpBaseD2 1291 // 0 4096
#define lmpImprovingD2 182 // 0 1024
#define lmpTTcaptureD2 434 // 0 1024
#define lmpBaseD1 254 // 0 4096
#define lmpImprovingD1 80 // 0 1024
#define lmpTTcaptureD1 80 // 0 1024
#define lmpBaseD0 3175 // 0 5120
#define lmpImprovingD0 46 // 0 1024
#define lmpTTcaptureD0 156 // 0 1024
#define lmpHistoryThreshold 203 // -1280 1280

#define historyBaseD2 101 // 0 800
#define historyImprovingD2 7 // 0 200
#define historyTTcaptureD2 2 // 0 200
#define historyBaseD1 400 // 0 800
#define historyImprovingD1 30 // 0 200
#define historyTTcaptureD1 56 // 0 200
#define historyBaseD0 25 // 0 800
#define historyImprovingD0 34 // 0 200
#define historyTTcaptureD0 22 // 0 200

#define fpBaseD2 208 // 0 500
#define fpImprovingD2 5 // 0 200
#define fpTTcaptureD2 86 // 0 200
#define fpHistoryD2 87 // 0 200
#define fpBaseD1 75 // 0 500
#define fpImprovingD1 29 // 0 200
#define fpTTcaptureD1 12 // 0 200
#define fpHistoryD1 17 // 0 200
#define fpBaseD0 24 // 0 500
#define fpImprovingD0 0 // 0 200
#define fpTTcaptureD0 20 // 0 200
#define fpHistoryD0 6 // 0 200

#define seeQBaseD0 3 // 0 500
#define seeQHistoryD0 27 // 0 200
#define seeQBaseD1 147 // 0 500
#define seeQHistoryD1 64 // 0 200
#define seeQBaseD2 74 // 0 500
#define seeQHistoryD2 2 // 0 200

#define seeNBaseD0 0 // 0 500
#define seeNHistoryD0 5 // 0 200
#define seeNBaseD1 76 // 0 500
#define seeNHistoryD1 81 // 0 200
#define seeNBaseD2 107 // 0 500
#define seeNHistoryD2 29 // 0 200

#define lmrDivisor 299 // 0 2048
#define lmrBase 402 // 0 4096
#define lmrPv 1353 // 0 4096
#define lmrHistory 1770 // 0 4096
#define lmrImproving 644 // 0 4096
#define lmrTTcapture 1076 // 0 4096
#define lmrExact 1285 // 0 4096
#define lmrCutnode 850 // 0 4096
#define lmrTtpv 899 // 0 4096
#define lmrCorrplex 216 // 0 4096
#define lmrCapture 1021 // 0 4096
#define lmrSee 2316 // 0 4096
#define lmrKiller 1374 // 0 4096

#define historyBonusD0 -214 // -1600 1600
#define historyBonusD1 176 // 0 1600
#define historyBonusD2 182 // 0 1600
#define historyMaluseD0 448 // -1600 1600
#define historyMaluseD1 200 // 0 1600
#define historyMaluseD2 2 // 0 1600

#define aspirationWindowSize 21 // 15 200
#define aspirationWindowMult 2733 // 1800 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 735 // 0 2048
#define historyScorePieceTo 840 // 0 2048
#define historyScorePly1 880 // 0 2048
#define historyScorePly2 888 // 0 2048
#define historyScorePly4 915 // 0 2048

#define corrhistPawn 279 // 0 1024
#define corrhistMinor 130 // 0 1024
#define corrhistColor 173 // 0 1024
#define corrhistFromTo 154 // 0 1024
#define corrhistPly1 131 // 0 1024

#define seePawn 101 // 0 300
#define seeKnight 374 // 0 800
#define seeBishop 246 // 0 800
#define seeRook 601 // 0 1200
#define seeQueen 980 // 0 2500

#define qsBadCapturesMargin 107 // 0 500
#define badCapturesBase 206 // 0 500
#define badCapturesHistory 74 // 0 500

#define moveOrderHistoryScore 403 // 0 1600
#define moveOrderPawn 92 // 0 300
#define moveOrderKnight 226 // 0 800
#define moveOrderBishop 168 // 0 800
#define moveOrderRook 419 // 0 1200
#define moveOrderQueen 934 // 0 2500

#define matScalePawn 99 // 0 200
#define matScaleKnight 344 // 100 500
#define matScaleBishop 346 // 100 500
#define matScaleRook 540 // 300 800
#define matScaleQueen 859 // 500 1400
#define matScaleBase 28789 // 10000 40000
