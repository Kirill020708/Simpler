#pragma once


#define qsFPmargin 94 // 0 300
#define qsFPseeMargin -10 // -100 100

#define corrplexityMargin 118 // 0 300

#define rfpBaseD2 29 // 0 70
#define rfpImprovingD2 14 // 0 40
#define rfpCorrplexityD2 24 // 0 40
#define rfpWorseningD2 0 // 0 40
#define rfpCutnodeD2 1 // 0 40
#define rfpBaseD1 1 // 0 70
#define rfpImprovingD1 2 // 0 40
#define rfpCorrplexityD1 2 // 0 40
#define rfpWorseningD1 4 // 0 40
#define rfpCutnodeD1 3 // 0 40
#define rfpBaseD0 4 // 0 70
#define rfpImprovingD0 0 // 0 40
#define rfpCorrplexityD0 0 // 0 40
#define rfpWorseningD0 6 // 0 40
#define rfpCutnodeD0 4 // 0 40
#define rfpFail 383 // 0 1024

#define nmpBaseMargin 29 // 0 80
#define nmpDepthMargin 1560 // 0 10240
#define nmpDepth2Margin 1057 // 0 10240
#define nmpRbase 4386 // 0 10240 341
#define nmpRdepth 121 // 0 1024
#define nmpRimproving 569 // 0 3072
#define nmpRcutnode 1013 // -4096 4096
#define nmpRmargin 245 // 50 500
#define nmpRmarginClamp 5246 // 512 10240

#define razorBaseD2 152 // 100 400
#define razorImprovingD2 33 // 0 70
#define razorBaseD1 3 // 0 300
#define razorImprovingD1 5 // 0 70
#define razorBaseD0 153 // 0 400
#define razorImprovingD0 6 // 0 70
#define razorFPmargin 59 // 0 150
#define razorRFPmargin 208 // 0 500

#define probcutBase 169 // 0 500
#define probcutImproving 34 // 0 100
#define probcutFail 956 // 0 1024

#define singextMarginDepth 1012 // 0 1024
#define dextMarginBase 14996 // 0 15360
#define dextMarginHistory 10 // 0 10
#define trextMarginBase 75 // 0 80

#define lmpBaseD2 1281 // 0 4096
#define lmpImprovingD2 294 // 0 1024
#define lmpTTcaptureD2 544 // 0 1024
#define lmpBaseD1 14 // 0 4096
#define lmpImprovingD1 37 // 0 1024
#define lmpTTcaptureD1 168 // 0 1024
#define lmpBaseD0 2807 // 0 5120
#define lmpImprovingD0 118 // 0 1024
#define lmpTTcaptureD0 70 // 0 1024
#define lmpHistoryThreshold -311 // -1280 1280

#define historyBaseD2 82 // 0 800
#define historyImprovingD2 7 // 0 200
#define historyTTcaptureD2 0 // 0 200
#define historyBaseD1 270 // 0 800
#define historyImprovingD1 66 // 0 200
#define historyTTcaptureD1 85 // 0 200
#define historyBaseD0 91 // 0 800
#define historyImprovingD0 2 // 0 200
#define historyTTcaptureD0 1 // 0 200

#define fpBaseD2 133 // 0 500
#define fpImprovingD2 9 // 0 200
#define fpTTcaptureD2 89 // 0 200
#define fpHistoryD2 31 // 0 200
#define fpBaseD1 43 // 0 500
#define fpImprovingD1 20 // 0 200
#define fpTTcaptureD1 1 // 0 200
#define fpHistoryD1 15 // 0 200
#define fpBaseD0 2 // 0 500
#define fpImprovingD0 2 // 0 200
#define fpTTcaptureD0 17 // 0 200
#define fpHistoryD0 0 // 0 200

#define seeBaseD0 79 // 0 500
#define seeHistoryD0 0 // 0 200
#define seeBaseD1 77 // 0 500
#define seeHistoryD1 101 // 0 200
#define seeBaseD2 93 // 0 500
#define seeHistoryD2 1 // 0 200

#define lmrDivisor 307 // 0 2048
#define lmrBase 791 // 0 4096
#define lmrPv 1747 // 0 4096
#define lmrHistory 1163 // 0 4096
#define lmrImproving 185 // 0 4096
#define lmrTTcapture 835 // 0 4096
#define lmrCutnode 742 // 0 4096
#define lmrTtpv 467 // 0 4096
#define lmrCapture 1161 // 0 4096
#define lmrSee 1672 // 0 4096
#define lmrKiller 1716 // 0 4096

#define historyBonusD0 337 // -1600 1600
#define historyBonusD1 396 // 0 1600
#define historyBonusD2 162 // 0 1600
#define historyMaluseD0 227 // -1600 1600
#define historyMaluseD1 287 // 0 1600
#define historyMaluseD2 28 // 0 1600

#define aspirationWindowSize 25 // 5 200
#define aspirationWindowMult 2048 // 1500 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 1213 // 0 2048
#define historyScorePieceTo 1346 // 0 2048
#define historyScorePly1 1136 // 0 2048
#define historyScorePly2 1056 // 0 2048
#define historyScorePly4 992 // 0 2048

#define corrhistPawn 361 // 0 1024
#define corrhistMinor 79 // 0 1024
#define corrhistColor 30 // 0 1024
#define corrhistFromTo 163 // 0 1024
#define corrhistPly1 179 // 0 1024

#define seePawn 95 // 0 300
#define seeKnight 290 // 0 800
#define seeBishop 411 // 0 800
#define seeRook 374 // 0 1200
#define seeQueen 1227 // 0 2500

#define qsBadCapturesMargin 125 // 0 500
#define badCapturesBase 218 // 0 500
#define badCapturesHistory 98 // 0 500

#define moveOrderHistoryScore 355 // 0 1600
#define moveOrderPawn 68 // 0 300
#define moveOrderKnight 436 // 0 800
#define moveOrderBishop 284 // 0 800
#define moveOrderRook 595 // 0 1200
#define moveOrderQueen 1034 // 0 2500


